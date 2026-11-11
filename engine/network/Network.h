#pragma once

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <span>

struct RawPacket
{
	std::vector<std::uint8_t> data;
	std::uint32_t fromId = 0;
};

struct NetworkEvents
{
	std::vector<RawPacket> received;
	std::vector<std::uint32_t> peerConnected;
	std::vector<std::uint32_t> peerDisconnected;
	bool hostTimeOut = false;
	bool idAssigned = false;
};

class Network
{
public:
	static constexpr double CLIENT_TIMEOUT = 15.0;
	static constexpr std::uint16_t DEFAULT_PORT = 54545;
	static constexpr std::uint32_t SYS_MAGIC_HELLO = 0x4750484C;	   // System packet
	static constexpr std::uint32_t SYS_MAGIC_ASSIGN = 0x47504153;	   // System packet
	static constexpr std::uint32_t SYS_MAGIC_DISCONNECT = 0x47504C60;  // System packet
	static constexpr std::uint32_t INVALID_ID = 0xFFFFFFFF;
	
	Network();
	~Network();

	Network(const Network&) = delete;
	Network& operator=(const Network&) = delete;

	void SendToHost(const void* data, std::size_t size);				   // Client to Host
	void SendToHost(std::span<const std::uint8_t> bytes);
	void SendToPeer(std::uint32_t peerId, const void* data, std::size_t size);  // Host to Client
	void SendToPeer(std::uint32_t peerId, std::span<const std::uint8_t> bytes);
	void BroadcastToPeers(const void* data, std::size_t size);			   // Host to all Clients
	void BroadcastToPeers(std::span<const std::uint8_t> bytes);
	void BroadcastDisconnect(std::uint32_t leftId);

	void Update(float deltaTime, NetworkEvents& outEvents);

	bool StartHost(std::uint16_t port = DEFAULT_PORT);
	bool StartClient(const std::string& ip, std::uint16_t port = DEFAULT_PORT);

	bool IsRunning() const;
	bool IsHost() const;

	std::uint32_t GetLocalId() const;
	
private:
	struct Peer
	{
		sockaddr_in addr{};
		std::uint32_t id = 0;
		double lastSeen = 0.0;
	};
	
	SOCKET _socket = INVALID_SOCKET;
	bool _isHost = false, _hostTimedOut = false;
	std::uint32_t _localId = 0;
	float _sendTimer = 0.0f;
	double _lastHostPacketTime = 0.0;
	
	sockaddr_in _serverAddr{};
	std::vector<Peer> _peers;
	std::uint32_t _nextClientId = 1;
	
	void CloseSocket();

	double Now() const;
	
	void SendRaw(const void* data, int size, const sockaddr_in& to);

	bool HandleSystemPacket(const std::uint8_t* buf, int n, const sockaddr_in& from, NetworkEvents& outEvents);

	Peer* FindOrRegisterPeer(const sockaddr_in& from, NetworkEvents& outEvents);

	void PollDisconnects(NetworkEvents& outEvents);
	void Poll(NetworkEvents& outEvents);
};