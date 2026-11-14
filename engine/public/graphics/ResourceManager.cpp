#include "ResourceManager.h"
#include <graphics/ShaderProgram.h>
#include <graphics/MeshFactory.h>
#include <graphics/Mesh.h>
#include <stdexcept>

Mesh* ResourceManager::GetMesh(const std::string& name) const
{
	auto mesh = _meshes.find(name);
	if (mesh == _meshes.end()) throw std::invalid_argument("[ResourceManager] Mesh not found: " + name);
	return mesh->second.get();
}

ShaderProgram* ResourceManager::GetShader(const std::string& name) const
{
	auto shader = _shaders.find(name);
	if (shader == _shaders.end()) throw std::invalid_argument("[ResourceManager] Shader not found: " + name);
	return shader->second.get();
}

Mesh* ResourceManager::CreateMesh(const std::string& name, const struct MeshData& data)
{
	auto temp = _meshes.find(name);
	if (temp != _meshes.end()) return temp->second.get();

	auto mesh = std::make_unique<Mesh>();
	mesh->Upload(data.vertices, data.layout, data.indices);
	_meshes[name] = std::move(mesh);
	return _meshes[name].get();
}

ShaderProgram* ResourceManager::CreateShader(const std::string& name, const char* vertexPath, const char* fragmentPath)
{
	auto temp = _shaders.find(name);
	if (temp != _shaders.end()) return temp->second.get();

	auto shader = std::make_unique<ShaderProgram>(vertexPath, fragmentPath);
	if (!shader->IsValid()) throw std::invalid_argument("[ResourceManager] Shader error: " + name);
	_shaders[name] = std::move(shader);
	return _shaders[name].get();
}

ShaderProgram* ResourceManager::CreateShaderFromSource(const std::string& name, const std::string& vsSource, const std::string& fsSource)
{
	if (_shaders.contains(name)) return _shaders[name].get();
	auto shader = std::make_unique<ShaderProgram>(vsSource, fsSource, true);
	if (!shader->IsValid()) throw std::invalid_argument("Shader error: " + name);
	_shaders[name] = std::move(shader);
	return _shaders[name].get();
}