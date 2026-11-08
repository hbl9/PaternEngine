#include "SceneObject.h"
#include "../graphics/Mesh.h"
#include "../graphics/ShaderProgram.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <type_traits>
#include <stdexcept>

SceneObject::SceneObject(const char* name, Mesh* mesh, ShaderProgram* shader) : _mesh(mesh), _name(name) { _material->Shader = shader; }
SceneObject::SceneObject(const char* name) : _isNothing(true), _name(name) {}
SceneObject::~SceneObject()
{
	if (_parent) std::erase(_parent->_children, this);
	for (SceneObject* obj : _children)
		obj->_parent = nullptr;
	_children.clear();
}

void SceneObject::SetWorldPosition(const glm::vec3& pos)
{
	if (!_parent)
	{
		SetLocalPosition(pos);
		return;
	}

	SetLocalPosition(glm::vec3(glm::inverse(_parent->GetModelMatrix()) * glm::vec4(pos, 1.0f)));
	MarkDirty();
}

void SceneObject::SetParent(SceneObject* parent)
{
	if (_parent == parent) return;

	if (_parent) std::erase(_parent->_children, this);

	_parent = parent;

	if (_parent) _parent->_children.push_back(this);

	MarkDirty();
}

void SceneObject::SetLocalRotation(float angleRad, const glm::vec3& axis) { _rotation = glm::rotate(glm::mat4(1.0f), angleRad, glm::normalize(axis)); MarkDirty(); }
void SceneObject::SetLocalRotation(const glm::mat4& rot) { _rotation = rot; MarkDirty(); }
void SceneObject::SetLocalRotation(const glm::quat& q) { _rotation = glm::mat4_cast(q); MarkDirty(); }
void SceneObject::SetRotationFromYawPitch(float yawDeg, float pitchDeg) { SetLocalRotation(glm::quat(glm::vec3(glm::radians(pitchDeg), glm::radians(-yawDeg - 90.0f), 0.0f))); }

void SceneObject::SetLocalPosition(const glm::vec3& pos) { _position = pos; MarkDirty(); }
void SceneObject::SetLocalScale(const glm::vec3& scale) { _scale = scale; MarkDirty(); }
void SceneObject::SetColor(const glm::vec3& color) { _color = color; }
void SceneObject::SetColor(float r, float g, float b) { SetColor(glm::vec3(r, g, b)); }

const glm::mat4& SceneObject::GetModelMatrix() const
{
	if (_dirty) CalculateObject();
	return _model;
}

glm::vec3 SceneObject::GetWorldPosition() const { return glm::vec3(GetModelMatrix()[3]); }
const glm::vec3& SceneObject::GetLocalPosition() const { return _position; }
SceneObject* SceneObject::GetParent() { return _parent; }
std::vector<SceneObject*> SceneObject::GetChildren() { return _children; }
const glm::vec3& SceneObject::GetColor() const { return _color; }
const std::string& SceneObject::GetName() const { return _name; }

void SceneObject::Rotate(float angleRad, const glm::vec3& axis) { _rotation = glm::rotate(_rotation, angleRad, glm::normalize(axis)); MarkDirty(); }

void SceneObject::Move(const glm::vec3 offset) { _position += offset; MarkDirty(); }
void SceneObject::Move(float x, float y, float z) { Move(glm::vec3(x, y, z)); }

void SceneObject::Draw(const glm::mat4& view, const glm::mat4& projection) const
{
	if (_isNothing) return;
	if (!_material->Shader || !_material->Shader->IsValid()) throw std::runtime_error("Невалидный Shader");
	if (!_mesh) throw std::runtime_error("Невалидный Mesh");

	if (_dirty) CalculateObject();

	_material->Shader->Bind();

	_material->Shader->SendMat4("uModel", GetModelMatrix());
	_material->Shader->SendMat4("uView", view);
	_material->Shader->SendMat4("uProjection", projection);

	for (const auto& u : _material->Uniforms)
	{
		std::visit([&](auto&& v)
			{
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, float>)          _material->Shader->SendFloat(u.first.c_str(), v);
				else if constexpr (std::is_same_v<T, int>)       _material->Shader->SendInt(u.first.c_str(), v);
				else if constexpr (std::is_same_v<T, bool>)      _material->Shader->SendBool(u.first.c_str(), v);
				else if constexpr (std::is_same_v<T, glm::vec2>) _material->Shader->SendVec2(u.first.c_str(), v);
				else if constexpr (std::is_same_v<T, glm::vec3>) _material->Shader->SendVec3(u.first.c_str(), v);
				else if constexpr (std::is_same_v<T, glm::vec4>) _material->Shader->SendVec4(u.first.c_str(), v);
				else if constexpr (std::is_same_v<T, glm::mat3>) _material->Shader->SendMat3(u.first.c_str(), v);
				else if constexpr (std::is_same_v<T, glm::mat4>) _material->Shader->SendMat4(u.first.c_str(), v);
			}, u.second);
	}

	_mesh->Draw();
}

void SceneObject::ClearUniforms() { _material->Uniforms.clear(); }

void SceneObject::MarkDirty()
{
	_dirty = true;
	for (SceneObject* obj : _children)
		obj->MarkDirty();
}

void SceneObject::CalculateObject() const
{
	glm::mat4 m = glm::translate(glm::mat4(1.0f), _position) * _rotation * glm::scale(glm::mat4(1.0f), _scale);  // T * R * S
	_model = _parent ? _parent->GetModelMatrix() * m : m;
	_dirty = false;
}