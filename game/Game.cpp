#include "..\engine\graphics\MeshFactory.h"
#include "..\engine\network\Network.h"
#include "..\engine\scene\Scene.h"
#include "Game.h"
#include "network/GameProtocol.h"

Game::Game(bool isHost, const char* connectIp) :
	_isHost(isHost), _connectIp(connectIp ? connectIp : "") {
}

Game::~Game()
{
	if (_network && _network->IsRunning())
		_network->BroadcastDisconnect(_network->GetLocalId());
}

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

	SceneObject* plane = scene.CreateObject("Plane", "plane", "plane");
	plane->SetColor(0.55f, 0.55f, 0.55f);
	plane->SetUniform("uColor", plane->GetColor());

	_player = scene.CreateObject("player", "cube", "player", _cameraController->GetPos());
	_player->SetColor(glm::vec3(0.8f, 0.5f, 0.2f));
	_player->SetUniform("uColor", _player->GetColor());
	_player->SetLocalScale({ 0.2f, 0.2f, 0.2f });

	_dick = scene.CreateDot("Dick");
	scene.CreateObject("cube", "cube", "basic", glm::vec3(-0.5f, 0.5f, 0.0f), _dick);
	scene.CreateObject("cube", "cube", "basic", glm::vec3(0.5f, 0.5f, 0.0f), _dick);
	scene.CreateObject("cube", "cube", "basic", glm::vec3(0.0f, 1.5f, 0.0f), _dick);
	scene.CreateObject("cube", "cube", "basic", glm::vec3(0.0f, 2.5f, 0.0f), _dick);
}

void Game::OnUpdate(float deltaTime, Scene& scene, InputManager& im)
{
	_dick->Rotate(deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
	_player->SetLocalPosition(_cameraController->GetPos());

	if (_network && _network->IsRunning())
	{
		// Transport network update
		NetworkEvents ne;
		_network->Update(deltaTime, ne);

		// Disconnects
		for (uint32_t id : ne.peerDisconnected)
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

void Game::SendLocalState()
{
	uint32_t id = _network->GetLocalId();
	if (id == Network::INVALID_ID) return;
	PlayerState ps;
	ps.id = id;
	ps.pos = _cameraController->GetPos();
	ps.yaw = _cameraController->GetYaw();
	ps.pitch = _cameraController->GetPitch();

	std::vector<uint8_t> bytes = GameProtocol::SerializeState(ps);
	_network->BroadcastToPeers(bytes);
}

void Game::ApplyRemoteState(Scene& scene, const PlayerState& ps)
{
	auto rp = _remotePlayers.find(ps.id);
	if (rp == _remotePlayers.end())
	{
		SceneObject* obj = scene.CreateObject("remotePlayer", "cube", "player", ps.pos);
		obj->SetColor(glm::vec3(0.2f, 0.5f, 0.8f) * glm::vec3(static_cast<float>(ps.id)));
		obj->SetUniform("uColor", obj->GetColor());
		obj->SetRotationFromYawPitch(ps.yaw, ps.pitch);
		obj->SetLocalScale({ 0.2f, 0.2f, 0.2f });
		_remotePlayers[ps.id] = obj;
	}
	else
	{
		rp->second->SetLocalPosition(ps.pos);
		rp->second->SetRotationFromYawPitch(ps.yaw, ps.pitch);
	}
}

void Game::RemoveRemotePlayer(Scene& scene, uint32_t id)
{
	auto rp = _remotePlayers.find(id);
	if (rp != _remotePlayers.end())
	{
		std::cout << "[Network] Player with ID=" << id << " disconnected\n";
		scene.RemoveObject(rp->second);
		_remotePlayers.erase(rp);
	}
}