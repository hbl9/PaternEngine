#pragma once

class ICameraController;
class InputManager;
class Network;
class Camera;
class Scene;

class IGame
{
public:
	virtual ~IGame() = default;

	virtual void OnInit(Scene&) = 0;
	virtual void OnUpdate(float dt, Scene&, InputManager&) = 0;

	void SetCamera(Camera* cam) { _camera = cam; }
	void SetNetwork(Network* net) { _network = net; }
	void SetCameraController(ICameraController* control) { _cameraController = control; }

protected:
	Camera* _camera = nullptr;
	Network* _network = nullptr;
	ICameraController* _cameraController = nullptr;
};