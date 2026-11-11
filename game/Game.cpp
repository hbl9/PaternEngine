#include "Game.h"
#include "../engine/camera/ICameraController.h"
#include "../engine/graphics/MeshFactory.h"
#include "../engine/graphics/ResourceManager.h"
#include "../engine/network/Network.h"
#include "../engine/scene/SceneObject.h"
#include "../engine/scene/Scene.h"
#include "network/GameProtocol.h"
#include <iostream>

Game::Game(bool isHost, const char* connectIp) : _isHost(isHost), _connectIp(connectIp ? connectIp : "") {}
Game::~Game() { if (_network && _network->IsRunning()) _network->BroadcastDisconnect(_network->GetLocalId()); }

void Game::OnInit(Scene& scene)
{
	if (_network)
	{
		if (_isHost) _network->StartHost();
		else if (!_connectIp.empty()) _network->StartClient(_connectIp);
	}

	scene.GetResources().CreateMesh("cube", MeshFactory::CreateCube());
	scene.GetResources().CreateMesh("plane", MeshFactory::CreatePlane(10.0f, 10.0f));
	scene.GetResources().CreateShader("basic", "assets/shaders/basic.vert", "assets/shaders/basic.frag");
	scene.GetResources().CreateShader("plane", "assets/shaders/plane.vert", "assets/shaders/plane.frag");
	scene.GetResources().CreateShader("player", "assets/shaders/player.vert", "assets/shaders/player.frag");

	std::uint64_t planeId = scene.CreateObject("Plane", "plane", "plane");
	if (SceneObject* plane = scene.GetObjById(planeId); plane != nullptr)
	{
		plane->SetColor(0.55f, 0.55f, 0.55f);
		plane->SetUniform("uColor", plane->GetColor());
	}

	_playerId = scene.CreateObject("player", "cube", "player", _cameraController->GetPos());
	if (SceneObject* player = scene.GetObjById(_playerId); player != nullptr)
	{
		player->SetColor(glm::vec3(0.8f, 0.5f, 0.2f));
		player->SetUniform("uColor", player->GetColor());
		player->SetLocalScale({ 0.2f, 0.2f, 0.2f });
	}

	_dickId = scene.CreateDot("Dick");
	scene.CreateObject("cube", "cube", "basic", glm::vec3(-0.5f, 0.5f, 0.0f), _dickId);
	scene.CreateObject("cube", "cube", "basic", glm::vec3(0.5f, 0.5f, 0.0f), _dickId);
	scene.CreateObject("cube", "cube", "basic", glm::vec3(0.0f, 1.5f, 0.0f), _dickId);
	scene.CreateObject("cube", "cube", "basic", glm::vec3(0.0f, 2.5f, 0.0f), _dickId);

	scene.AddDestroyListener([this](std::uint64_t id)
		{
			if (id == _playerId) _playerId = SceneHandle::INVALID;
			if (id == _dickId) _dickId = SceneHandle::INVALID;
			for (auto rp = _remotePlayersId.begin(); rp != _remotePlayersId.end();)
				if (rp->second == id) rp = _remotePlayersId.erase(rp);
				else rp++;
		});
}

void Game::OnUpdate(float deltaTime, Scene& scene, InputManager& im)
{
	if (SceneObject* dick = scene.GetObjById(_dickId); dick != nullptr)
		dick->Rotate(deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
	else
		std::cout << "Object dick not found\n";
	if (SceneObject* player = scene.GetObjById(_playerId); player != nullptr)
		player->SetLocalPosition(_cameraController->GetPos());
	else
		std::cout << "Object player not found\n";

	if (_network && _network->IsRunning())
	{
		// Transport network update
		NetworkEvents ne;
		_network->Update(deltaTime, ne);

		// Disconnects
		for (std::uint32_t id : ne.peerDisconnected)
			RemoveRemotePlayer(scene, id);

		// Receiving
		for (const RawPacket& pkt : ne.received)
		{
			PlayerState ps;
			if (GameProtocol::DeserializeState(pkt.data, ps) && ps.id != _network->GetLocalId())
				ApplyRemoteState(scene, ps);
		}

		// Send
		_sendTimer += deltaTime;
		if (_sendTimer >= 1.0f / 30.0f)  // 30 per sec
		{
			_sendTimer = 0.0f;
			SendLocalState();
		}
	}
}

void Game::OnShutdown()
{
	// For save before close game
}

void Game::SendLocalState()
{
	std::uint32_t id = _network->GetLocalId();
	if (id == Network::INVALID_ID) return;
	PlayerState ps;
	ps.id = id;
	ps.pos = _cameraController->GetPos();
	ps.yaw = _cameraController->GetYaw();
	ps.pitch = _cameraController->GetPitch();

	std::vector<std::uint8_t> bytes = GameProtocol::SerializeState(ps);
	_network->BroadcastToPeers(bytes);
}

void Game::ApplyRemoteState(Scene& scene, const PlayerState& ps)
{
	auto rpId = _remotePlayersId.find(ps.id);
	if (rpId == _remotePlayersId.end())
	{
		std::uint64_t objId = scene.CreateObject("remotePlayer", "cube", "player", ps.pos);
		if (SceneObject* obj = scene.GetObjById(objId); obj != nullptr)
		{
			obj->SetColor(glm::vec3(0.2f, 0.5f, 0.8f) * glm::vec3(static_cast<float>(ps.id)));
			obj->SetUniform("uColor", obj->GetColor());
			obj->SetRotationFromYawPitch(ps.yaw, ps.pitch);
			obj->SetLocalScale({ 0.2f, 0.2f, 0.2f });
		}
		_remotePlayersId[ps.id] = objId;
	}
	else if (SceneObject* rp = scene.GetObjById(rpId->second); rp != nullptr)
	{
		rp->SetLocalPosition(ps.pos);
		rp->SetRotationFromYawPitch(ps.yaw, ps.pitch);
	}
}

void Game::RemoveRemotePlayer(Scene& scene, std::uint32_t id)
{
	auto rp = _remotePlayersId.find(id);
	if (rp != _remotePlayersId.end())
	{
		std::cout << "[Network] Player with ID=" << id << " disconnected\n";
		scene.RemoveObject(rp->second);
	}
}