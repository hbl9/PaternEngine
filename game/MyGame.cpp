#include "MyGame.h"
#include <platform/InputManager.h>
#include <scene/SceneObject.h>
#include <scene/Scene.h>

void MyGame::OnInit(Scene& scene)
{
	// Create cube in center
	std::uint64_t id = scene.CreateObject("Player", "cube", "plane");
	_player = scene.GetObjById(id);
	if (_player != nullptr)
	{
		_player->SetColor(1.0f, 0.2f, 0.2f);
		_player->SetUniform("uColor", _player->GetColor());
		_player->SetLocalPosition({ 0.0f, 0.5f, 0.0f });
		_player->SetLocalScale({ 0.5f, 0.5f, 0.5f });
	}
}

void MyGame::OnUpdate(float dt, Scene& scene, InputManager& im)
{
	if (_player)
	{
		_player->Rotate(dt, glm::vec3(0.0f, 1.0f, 0.0f));
	}
}

void MyGame::OnShutdown()
{
	// Before close game
}