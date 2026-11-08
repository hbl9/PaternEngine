#pragma once

#include "..\engine\camera\ICameraController.h"
#include "..\engine\game\IGame.h"

#include <string>
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

private:
	std::unordered_map<uint32_t, SceneObject*> _remotePlayers;

	SceneObject* _player = nullptr;
	SceneObject* _dick = nullptr;

	bool _isHost = false;
	std::string _connectIp;
	float _sendTimer = 0.0f;

	void ApplyRemoteState(Scene& scene, const PlayerState& ps);
	void RemoveRemotePlayer(Scene& scene, uint32_t id);
	void SendLocalState();
};