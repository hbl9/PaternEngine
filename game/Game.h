#pragma once

#include "../engine/game/IGame.h"
#include "../engine/scene/SceneHandle.h"
#include <string>
#include <cstdint>
#include <unordered_map>

class SceneObject;
struct PlayerState;

class Game : public IGame
{
public:
	Game(bool isHost = false, const char* connectIp = nullptr);
	~Game();

	void OnInit(Scene& scene) override;
	void OnUpdate(float deltaTime, Scene& scene, InputManager& im) override;
	void OnShutdown() override;

private:
	std::unordered_map<std::uint32_t, std::uint64_t> _remotePlayersId;

	std::uint64_t _playerId = SceneHandle::INVALID;
	std::uint64_t _dickId = SceneHandle::INVALID;

	bool _isHost = false;
	std::string _connectIp;
	float _sendTimer = 0.0f;

	void ApplyRemoteState(Scene& scene, const PlayerState& ps);
	void RemoveRemotePlayer(Scene& scene, std::uint32_t id);
	void SendLocalState();
};