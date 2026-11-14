#pragma once

#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <unordered_map>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glad.h>
#include <string>

struct UniformInfo
{
	GLint  location = -1;
	GLenum type = 0;
	GLint  size = 1;
};

class ShaderProgram
{
public:
	ShaderProgram() = default;
	ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
	ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource, bool isSource);
	~ShaderProgram();

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	ShaderProgram(ShaderProgram&& other)			noexcept;
	ShaderProgram& operator=(ShaderProgram&& other) noexcept;

	void LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);

	void Bind() const;
	void Unbind() const;

	GLuint GetID() const;

	bool IsValid() const;

	void SendMat4(const std::string& name, const glm::mat4& value) const;
	void SendMat3(const std::string& name, const glm::mat3& value) const;
	void SendVec4(const std::string& name, const glm::vec4& value) const;
	void SendVec3(const std::string& name, const glm::vec3& value) const;
	void SendVec2(const std::string& name, const glm::vec2& value) const;
	void SendFloat(const std::string& name, float value) const;
	void SendInt(const std::string& name, int value) const;
	void SendBool(const std::string& name, bool value) const;

private:
	std::unordered_map<std::string, UniformInfo> _uniformCache;
	GLuint _id = 0;

	// Пути для отладки
	std::string _vertexPath;
	std::string _fragmentPath;

	void FindUniform(const std::string& name, GLint& id) const;

	static GLuint CompileShader(GLenum type, const std::string& src, const std::string& path);

	void BuildUniformReflection();

	static GLuint LinkProgram(GLuint vertex, GLuint fragment);

	static std::string ReadFile(const std::string& path);

	void Destroy();
};