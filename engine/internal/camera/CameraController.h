#pragma once

#include <camera/ICameraController.h>
#include <glm/vec3.hpp>

class InputManager;
class Window;
class Camera;

class CameraController : public ICameraController
{
public:
	explicit CameraController(Camera* camera = nullptr);

	glm::vec3 GetPos() const override;
	float GetYaw()     const override;
	float GetPitch()   const override;

	void SetCamera(Camera* camera) override;

	void Update(InputManager& input, Window& window, float deltaTime) override;

private:
	Camera* _camera = nullptr;
	bool _shiftPressed = false;
};