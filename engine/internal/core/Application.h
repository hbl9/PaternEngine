#pragma once

#include <camera/ICameraController.h>
#include <game/IGame.h>
#include <cstdint>
#include <memory>
#include <string>

class InputManager;
class EditorLayer;
class Network;
class Window;
class Camera;
class Scene;

class Application
{
public:
	Application(int width, int height, const std::string& title, bool showEditor);
	~Application();

	bool Init(std::unique_ptr<IGame> game = nullptr, std::unique_ptr<ICameraController> cameraController = nullptr);
	void Run();

	std::uint64_t PickObjectAt(float mouseX, float mouseY);

private:
	std::unique_ptr<IGame>			   _game			 = nullptr;
	std::unique_ptr<InputManager>	   _input			 = nullptr;
	std::unique_ptr<Camera>			   _camera			 = nullptr;
	std::unique_ptr<ICameraController> _cameraController = nullptr;
	std::unique_ptr<Scene>			   _scene			 = nullptr;
	std::unique_ptr<Window>			   _window			 = nullptr;
	std::unique_ptr<Network>		   _network			 = nullptr;
	std::unique_ptr<EditorLayer>	   _editor			 = nullptr;
	bool _showEditor = true;
	float _lastFrame = 0.0f;

	void Render();
	void Update(float deltaTime);
};