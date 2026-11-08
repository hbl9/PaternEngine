#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "CameraController.h"
#include "../../engine/camera/Camera.h"
#include "../../engine/platform/Window.h"
#include "../../engine/platform/InputManager.h"

CameraController::CameraController(Camera* camera) : _camera(camera) {}

glm::vec3 CameraController::GetPos() const { return _camera->GetPos(); }
float CameraController::GetYaw()     const { return _camera->GetYaw(); }
float CameraController::GetPitch()   const { return _camera->GetPitch(); }

void CameraController::SetCamera(Camera* camera) { _camera = camera; }

void CameraController::Update(InputManager& input, Window& window, float deltaTime)
{
	if (!window.IsCursorCaptured()) return;

	if (glfwGetKey(window.GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) _shiftPressed = true;
	else _shiftPressed = false;

	if (glfwGetKey(window.GetWindow(), GLFW_KEY_W) == GLFW_PRESS) _camera->ProcessKeyboard(CameraMovement::Forward, deltaTime, _shiftPressed);
	if (glfwGetKey(window.GetWindow(), GLFW_KEY_S) == GLFW_PRESS) _camera->ProcessKeyboard(CameraMovement::Backward, deltaTime, _shiftPressed);
	if (glfwGetKey(window.GetWindow(), GLFW_KEY_A) == GLFW_PRESS) _camera->ProcessKeyboard(CameraMovement::Left, deltaTime, _shiftPressed);
	if (glfwGetKey(window.GetWindow(), GLFW_KEY_D) == GLFW_PRESS) _camera->ProcessKeyboard(CameraMovement::Right, deltaTime, _shiftPressed);
	if (glfwGetKey(window.GetWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) _camera->ProcessKeyboard(CameraMovement::Down, deltaTime, _shiftPressed);
	if (glfwGetKey(window.GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) _camera->ProcessKeyboard(CameraMovement::Up, deltaTime, _shiftPressed);

	float dx = 0.0f, dy = 0.0f;
	if (input.ConsumeMouseDelta(dx, dy))
		_camera->ProcessMouseMovement(dx, dy);

	float scroll;
	if (input.ConsumeScroll(scroll))
		_camera->ProcessMouseScroll(scroll);
}