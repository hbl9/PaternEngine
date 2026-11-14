#pragma once

#include <glm/vec3.hpp>

class InputManager;
class Window;
class Camera;

class ICameraController
{
public:
	virtual ~ICameraController() = default;

	virtual glm::vec3 GetPos() const = 0;
	virtual float GetYaw()	   const = 0;
	virtual float GetPitch()   const = 0;

	virtual void SetCamera(Camera* camera) = 0;

	virtual void Update(InputManager&, Window&, float dt) = 0;
};