#include "Camera.h"
#include <algorithm>
#include <stdexcept>
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch, float speed, float sensitivity, float fov) :
	_position(pos), _worldUp(up), _yaw(yaw), _pitch(pitch), _speed(speed), _sensitivity(sensitivity), _fov(fov)
{
	UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const { return _viewMatrix; }
float Camera::GetFOVDegrees() const { return _fov; }
glm::vec3 Camera::GetPos() const { return _position; }
float Camera::GetYaw() const { return _yaw; }
float Camera::GetPitch() const { return _pitch; }
glm::mat4 Camera::GetProjectionMatrix(float aspect) const
{
	if (std::abs(_lastAspect - aspect) > 1e-6f)
	{
		_lastAspect = aspect;
		UpdateProjection();
	}
	return _projection;
}

void Camera::SetNearFar(float nearZ, float farZ)
{
	if (nearZ <= 0.0f) throw std::invalid_argument("[Camera] Ближняя граница видимости должна быть положительной");
	if (nearZ >= farZ) throw std::invalid_argument("[Camera] Ближняя граница видимости должна быть меньше дальней");

	_nearZ = nearZ;
	_farZ = farZ;

	UpdateProjection();
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime, bool shiftPressed)
{
	float velocity = _speed * deltaTime;
	if (shiftPressed) velocity *= _multipleShift;

	switch (direction)
	{
	case CameraMovement::Forward:  _position += _front * velocity; break;
	case CameraMovement::Backward: _position -= _front * velocity; break;
	case CameraMovement::Left:     _position -= _right * velocity; break;
	case CameraMovement::Right:    _position += _right * velocity; break;
	case CameraMovement::Up:       _position += _worldUp * velocity; break;
	case CameraMovement::Down:     _position -= _worldUp * velocity; break;
	}

	UpdateViewMatrix();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= _sensitivity;
	yoffset *= _sensitivity;

	_yaw += xoffset;
	_pitch += yoffset;

	if (constrainPitch) _pitch = std::clamp(_pitch, -89.0f, 89.0f);

	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	_fov = std::clamp(_fov - yoffset, 25.0f, 60.0f);

	UpdateProjection();
}

void Camera::UpdateViewMatrix() { _viewMatrix = glm::lookAt(_position, _position + _front, _up); }
void Camera::UpdateProjection() const { _projection = glm::perspective(glm::radians(_fov), _lastAspect, _nearZ, _farZ); }
void Camera::UpdateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.y = sin(glm::radians(_pitch));
	front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_front = glm::normalize(front);
	_right = glm::normalize(glm::cross(_front, _worldUp));
	_up = glm::normalize(glm::cross(_right, _front));
	UpdateViewMatrix();
}