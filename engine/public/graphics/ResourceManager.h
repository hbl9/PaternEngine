#pragma once

#include <unordered_map>
#include <memory>
#include <string>

struct MeshData;

class Mesh;
class ShaderProgram;

class ResourceManager
{
public:
	ResourceManager() = default;

	Mesh* GetMesh(const std::string& name) const;
	ShaderProgram* GetShader(const std::string& name) const;

	Mesh* CreateMesh(const std::string& name, const MeshData& data);
	ShaderProgram* CreateShader(const std::string& name, const char* vertexPath, const char* fragmentPath);
	ShaderProgram* CreateShaderFromSource(const std::string& name, const std::string& vsSource, const std::string& fsSource);

private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>>			_meshes;
	std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> _shaders;
};