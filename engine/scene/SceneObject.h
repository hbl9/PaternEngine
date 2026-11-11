#pragma once

#include "SceneHandle.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <unordered_map>
#include <cstdint>
#include <variant>
#include <memory>
#include <string>
#include <vector>

class ShaderProgram;
class Mesh;

struct Material
{
	ShaderProgram* Shader = nullptr;
	std::unordered_map<std::string, std::variant<float, int, bool, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4>> Uniforms;
};

class SceneObject
{
public:
	SceneObject(std::uint64_t id, const char* name, Mesh* mesh, ShaderProgram* shader);
	SceneObject(std::uint64_t id, const char* name);
	~SceneObject();

	void SetWorldPosition(const glm::vec3& pos);
	void SetLocalPosition(const glm::vec3& pos);
	void SetLocalRotation(float angleRad, const glm::vec3& axis);
	void SetLocalRotation(const glm::mat4& rot);
	void SetLocalRotation(const glm::quat& q);
	void SetRotationFromYawPitch(float yawDeg, float pitchDeg);
	void SetLocalScale(const glm::vec3& scale);
	void SetParent(SceneObject* parent);
	void SetColor(const glm::vec3& color);
	void SetColor(float r, float g, float b);
	void SetName(const std::string& name);
	void SetSelected(bool selected);

	void Rotate(float angleRad, const glm::vec3& axis);
	void Move(const glm::vec3 offset);
	void Move(float x, float y, float z);

	glm::vec3 GetLocalAabbMin() const;
	glm::vec3 GetLocalAabbMax() const;
	glm::vec3 GetWorldAabbMin() const;
	glm::vec3 GetWorldAabbMax() const;
	void GetWorldAabb(glm::vec3& outMin, glm::vec3& outMax) const;
	const glm::mat4& GetModelMatrix() const;
	glm::vec3 GetWorldPosition() const;
	const glm::vec3& GetLocalPosition()	const;
	glm::vec3 GetLocalScale() const;
	SceneObject* GetParent();
	const SceneObject* GetParent() const;
	std::vector<SceneObject*>& GetChildren();
	const std::vector<SceneObject*>& GetChildren() const;
	const glm::vec3& GetColor() const;
	const std::string& GetName() const;
	std::uint64_t GetId() const;

	bool IsNothing() const;
	bool IsSelected() const;
	bool HasLocalAabb() const;

	void Draw(const glm::mat4& view, const glm::mat4& projection) const;
	void MarkDirty();

	template <typename T>
	void SetUniform(const std::string& name, const T& v)
	{
		for (auto& u : _material->Uniforms)
			if (u.first == name)
			{
				u.second = v;
				return;
			}
		_material->Uniforms[name] = v;
	}

	void ClearUniforms();

private:
	std::string _name;
	Mesh* _mesh = nullptr;
	std::unique_ptr<Material> _material = std::make_unique<Material>();
	SceneObject* _parent = nullptr;
	std::uint64_t _id = SceneHandle::INVALID;
	glm::vec3 _position{ 0.0f }, _scale{ 1.0f }, _color{ 1.0f };
	glm::mat4 _rotation{ 1.0f };
	mutable glm::mat4 _model{ 1.0f };
	mutable glm::vec3 _worldAabbMin{ 0.0f }, _worldAabbMax{ 0.0f };
	mutable bool _dirty = true, _worldAabbDirty = true;
	bool _isNothing = false, _selected = false;

	std::vector<SceneObject*> _children;

	void CalculateObject() const;
};