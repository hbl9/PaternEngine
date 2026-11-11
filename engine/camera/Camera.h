#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

enum class CameraMovement
{
	Forward, Backward,
	Left, Right,
	Up, Down
};

class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = -90.0f, float pitch = 0.0f, float speed = 3.0f, float sensitivity = 0.1f, float fov = 45.0f);

	glm::mat4 GetProjectionMatrix(float aspect) const;
	glm::mat4 GetViewMatrix() const;
	float GetFOVDegrees() const;
	glm::vec3 GetPos() const;
	float GetYaw() const;
	float GetPitch() const;
	void GetRayFromScreen(float ndcX, float ndcY, float aspect, glm::vec3& outOrigin, glm::vec3& outDir) const;

	void ProcessKeyboard(CameraMovement direction, float deltaTime, bool shiftPressed);
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

	void SetNearFar(float nearZ, float farZ);

private:
	glm::vec3 _position{ 1.0f }, _front{ 1.0f }, _up{ 1.0f }, _right{ 1.0f }, _worldUp{ 1.0f };
	float _yaw, _pitch, _speed, _sensitivity, _fov, _multipleShift = 2.0f;
	glm::mat4 _viewMatrix{ 1.0f };
	mutable glm::mat4 _projection{ 1.0f };
	mutable float _lastAspect = -1.0f;
	float _nearZ = 0.1f, _farZ = 100.0f;

	void UpdateCameraVectors();
	void UpdateViewMatrix();
	void UpdateProjection() const;
};