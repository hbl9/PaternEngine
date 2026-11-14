#include "Network.h"
#include <WS2tcpip.h>
#include <iostream>
#include <cstring>
#include <chrono>

Network::Network()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		std::cout << "[Network] Error WSAStartup\n";
}

Network::~Network()
{
	CloseSocket();
	WSACleanup();
}

double Network::Now() const { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

bool Network::StartHost(std::uint16_t port)
{
	CloseSocket();

	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (!IsRunning())
	{
		std::cout << "[Network] Error socket()\n";
		return false;
	}

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "[Network] Error bind(), maybe the target port is busy\n";
		CloseSocket();
		return false;
	}

	u_long mode = 1;
	ioctlsocket(_socket, FIONBIO, &mode);

	_isHost = true;
	_localId = 0;

	std::cout << "[Net] Host enable port: " << port << '\n';
	return true;
}

bool Network::StartClient(const std::string& hostIp, std::uint16_t port)
{
	CloseSocket();

	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (!IsRunning())
	{
		std::cout << "[Network] Error socket()\n";
		return false;
	}

	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(port);
	if (inet_pton(AF_INET, hostIp.c_str(), &_serverAddr.sin_addr) != 1)
	{
		std::cout << "[Network] Incorrect IP: " << hostIp << '\n';
		CloseSocket();
		return false;
	}

	u_long mode = 1;
	ioctlsocket(_socket, FIONBIO, &mode);

	_isHost = false;
	_localId = INVALID_ID;
	_hostTimedOut = false;
	_lastHostPacketTime = Now();

	std::cout << "[Network] Connecting -> " << hostIp << ':' << port << '\n';

	// Hello: request an ID
	std::uint8_t buf[4];
	std::memcpy(buf, &SYS_MAGIC_HELLO, 4);
	SendRaw(buf, 4, _serverAddr);

	return true;
}

void Network::Update(float deltaTime, NetworkEvents& outEvents)
{
	outEvents.received.clear();
	outEvents.peerDisconnected.clear();
	outEvents.peerConnected.clear();
	outEvents.hostTimeOut = false;
	outEvents.idAssigned = false;

	if (IsRunning())
	{
		Poll(outEvents);

		if (_isHost) PollDisconnects(outEvents);
		else if (!_hostTimedOut && Now() - _lastHostPacketTime > CLIENT_TIMEOUT)
		{
			std::cout << "[Network] The Host is not responding to requests\n";
			_hostTimedOut = true;
			outEvents.hostTimeOut = true;
			outEvents.peerDisconnected.push_back(0);
		}
	}
}

void Network::Poll(NetworkEvents& outEvents)
{
	if (IsRunning())
	{
		std::uint8_t buf[1024];

		while (true)
		{
			sockaddr_in from{};
			int fromLen = sizeof(from);
			int n = recvfrom(_socket, reinterpret_cast<char*>(buf), sizeof(buf), 0, reinterpret_cast<sockaddr*>(&from), &fromLen);
			if (n <= 0) break;

			if (!_isHost && from.sin_addr.s_addr == _serverAddr.sin_addr.s_addr && from.sin_port == _serverAddr.sin_port)
				_lastHostPacketTime = Now();

			if (n < 4 || HandleSystemPacket(buf, n, from, outEvents)) continue;

			RawPacket pkt;
			pkt.data.assign(buf, buf + n);
			pkt.fromId = 0;

			if (_isHost)
				for (auto& p : _peers)
					if (p.addr.sin_addr.s_addr == from.sin_addr.s_addr && p.addr.sin_port == from.sin_port)
					{
						p.lastSeen = Now();
						pkt.fromId = p.id;
						break;
					}
			outEvents.received.push_back(std::move(pkt));

			if (_isHost && pkt.fromId != 0)
				for (auto& p : _peers)
					if (p.id != pkt.fromId)
						SendRaw(buf, n, p.addr);
		}
	}
}

void Network::PollDisconnects(NetworkEvents& outEvents)
{
	if (_isHost && IsRunning())
	{
		double now = Now();
		auto p = _peers.begin();
		while (p != _peers.end())
		{
			if (now - p->lastSeen > CLIENT_TIMEOUT)
			{
				std::cout << "[Network] Client ID=" << p->id << " timed out\n";
				outEvents.peerDisconnected.push_back(p->id);

				std::uint8_t b[8];
				std::memcpy(b, &SYS_MAGIC_DISCONNECT, 4);
				std::memcpy(b + 4, &p->id, 4);
				for (auto& it : _peers)
					if (p->id != it.id)
						SendRaw(b, 8, it.addr);

				p = _peers.erase(p);
			}
			else
				p++;
		}
	}
}

bool Network::HandleSystemPacket(const std::uint8_t* buf, int n, const sockaddr_in& from, NetworkEvents& outEvents)
{
	std::uint32_t magic = 0;
	std::memcpy(&magic, buf, 4);

	if (magic == SYS_MAGIC_HELLO)
	{
		if (_isHost) FindOrRegisterPeer(from, outEvents);
		return true;
	}
	if (magic == SYS_MAGIC_ASSIGN)
	{
		if (_isHost || n < 8) return true;

		std::uint32_t id = 0;
		std::memcpy(&id, buf + 4, 4);
		if (_localId == INVALID_ID)
		{
			_localId = id;
			outEvents.idAssigned = true;
			std::cout << "[Network] Assigned ID=" << _localId << '\n';
		}
		return true;
	}
	if (magic == SYS_MAGIC_DISCONNECT)
	{
		if (n < 8) return true;

		std::uint32_t leftId = 0;
		std::memcpy(&leftId, buf + 4, 4);
		outEvents.peerDisconnected.push_back(leftId);

		if (_isHost)
		{
			for (auto p = _peers.begin(); p != _peers.end(); p++)
				if (p->id == leftId)
				{
					std::uint8_t b[8];
					std::memcpy(b, &SYS_MAGIC_DISCONNECT, 4);
					std::memcpy(b + 4, &leftId, 4);
					for (auto& it : _peers)
						if (it.id != leftId)
							SendRaw(b, 8, it.addr);
					_peers.erase(p);
					break;
				}
		}
		return true;
	}
	return false;
}

Network::Peer* Network::FindOrRegisterPeer(const sockaddr_in& from, NetworkEvents& outEvents)
{
	for (auto& p : _peers)
		if (p.addr.sin_addr.s_addr == from.sin_addr.s_addr && p.addr.sin_port == from.sin_port)
			return &p;

	Peer pr;
	pr.addr = from;
	pr.id = _nextClientId++;
	pr.lastSeen = Now();
	_peers.push_back(pr);
	std::cout << "[Network] New Client, ID=" << pr.id << '\n';
	outEvents.peerConnected.push_back(pr.id);

	std::uint8_t b[8];
	std::memcpy(b, &SYS_MAGIC_ASSIGN, 4);
	std::memcpy(b + 4, &pr.id, 4);
	SendRaw(b, 8, from);

	return &_peers.back();
}

void Network::SendToHost(const void* data, std::size_t size) { if (IsRunning() && !IsHost()) SendRaw(data, static_cast<int>(size), _serverAddr); }
void Network::SendToHost(std::span<const std::uint8_t> bytes) { SendToHost(bytes.data(), bytes.size()); }
void Network::SendToPeer(std::uint32_t peerId, const void* data, std::size_t size)
{
	if (IsRunning() && _isHost)
		for (auto& p : _peers)
			if (p.id == peerId)
			{
				SendRaw(data, static_cast<int>(size), p.addr);
				return;
			}
}
void Network::SendToPeer(std::uint32_t peerId, std::span<const std::uint8_t> bytes) { SendToPeer(peerId, bytes.data(), bytes.size()); }
void Network::BroadcastToPeers(const void* data, std::size_t size)
{
	if (IsRunning())
		if (_isHost)
			for (auto& p : _peers)
				SendRaw(data, static_cast<int>(size), p.addr);
		else
			SendRaw(data, static_cast<int>(size), _serverAddr);
}
void Network::BroadcastToPeers(std::span<const std::uint8_t> bytes) { BroadcastToPeers(bytes.data(), bytes.size()); }
void Network::BroadcastDisconnect(std::uint32_t leftId)
{
	if (IsRunning())
	{
		std::uint8_t b[8];
		std::memcpy(b, &SYS_MAGIC_DISCONNECT, 4);
		std::memcpy(b + 4, &leftId, 4);

		if (_isHost)
			for (auto& p : _peers)
				SendRaw(b, 8, p.addr);
		else
			SendRaw(b, 8, _serverAddr);
	}
}

bool Network::IsHost() const { return _isHost; }
bool Network::IsRunning() const { return _socket != INVALID_SOCKET; }

std::uint32_t Network::GetLocalId() const { return _localId; }

void Network::CloseSocket()
{
	if (IsRunning())
	{
		closesocket(_socket);
		_socket = INVALID_SOCKET;
	}
	_peers.clear();
	_isHost = false;
	_hostTimedOut = false;
	_localId = INVALID_ID;
	_nextClientId = 1;
}

void Network::SendRaw(const void* data, int size, const sockaddr_in& to)
{
	if (IsRunning()) sendto(_socket, reinterpret_cast<const char*>(data), size, 0, reinterpret_cast<const sockaddr*>(&to), sizeof(to));
}