#pragma once

#include <memory>
#include <string>
#include <unordered_map>

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

private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>>			_meshes;
	std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> _shaders;
};