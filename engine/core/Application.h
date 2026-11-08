#pragma once

#include "..\camera\ICameraController.h"
#include "..\game\IGame.h"
#include <memory>

class CameraController;
class InputManager;
class Window;
class Network;
class Camera;
class Scene;

class Application
{
public:
	Application(int width, int height, const char* title);
	~Application();

	bool Init(std::unique_ptr<IGame> game);
	void Run();

private:
	std::unique_ptr<IGame>			  _game = nullptr;
	std::unique_ptr<InputManager>	  _input = nullptr;
	std::unique_ptr<Camera>			  _camera = nullptr;
	std::unique_ptr<ICameraController> _cameraController = nullptr;
	std::unique_ptr<Scene>			  _scene = nullptr;
	std::unique_ptr<Window>			  _window = nullptr;
	std::unique_ptr<Network>		  _network = nullptr;

	float _lastFrame = 0.0f;

	void Render();
	void Update(float deltaTime);
};