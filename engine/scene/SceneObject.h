#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <unordered_map>
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
	SceneObject(const char* name, Mesh* mesh, ShaderProgram* shader);
	SceneObject(const char* name);
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

	void Rotate(float angleRad, const glm::vec3& axis);
	void Move(const glm::vec3 offset);
	void Move(float x, float y, float z);

	const glm::mat4& GetModelMatrix() const;
	glm::vec3 GetWorldPosition() const;
	const glm::vec3& GetLocalPosition()	const;
	SceneObject* GetParent();
	std::vector<SceneObject*>& GetChildren();
	const glm::vec3& GetColor() const;
	const std::string& GetName() const;

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

	glm::vec3 _position{ 0.0f }, _scale{ 1.0f }, _color{ 1.0f };
	glm::mat4 _rotation{ 1.0f };
	mutable glm::mat4 _model{ 1.0f };
	mutable bool _dirty = true;
	bool _isNothing = false;

	std::vector<SceneObject*> _children;

	void CalculateObject() const;
};