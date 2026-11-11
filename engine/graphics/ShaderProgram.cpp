#include "ShaderProgram.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <iostream>

ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
{ LoadFromFiles(vertexPath, fragmentPath); }

ShaderProgram::~ShaderProgram() { Destroy(); }

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
	: _id(other._id), _uniformCache(std::move(other._uniformCache)), _vertexPath(std::move(other._vertexPath)), _fragmentPath(std::move(other._fragmentPath))
{ other._id = 0; }

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
{
	if (this != &other)
	{
		Destroy();
		_uniformCache = std::move(other._uniformCache);
		_vertexPath = std::move(other._vertexPath);
		_fragmentPath = std::move(other._fragmentPath);
		_id = other._id;
		other._id = 0;
	}
	return *this;
}

void ShaderProgram::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vsSrc = ReadFile(vertexPath);
	std::string fsSrc = ReadFile(fragmentPath);

	if (vsSrc.empty()) throw std::invalid_argument("[ShaderProgram] Failed to read file: " + vertexPath);
	if (fsSrc.empty()) throw std::invalid_argument("[ShaderProgram] Failed to read file: " + fragmentPath);

	_vertexPath = vertexPath;
	_fragmentPath = fragmentPath;

	GLuint vs = CompileShader(GL_VERTEX_SHADER, vsSrc.c_str(), vertexPath);
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSrc.c_str(), fragmentPath);

	GLuint program = LinkProgram(vs, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	if (!program) throw std::runtime_error("[ShaderProgram] Shader linking failed");

	Destroy();
	_id = program;
	BuildUniformReflection();
}

void ShaderProgram::Bind() const { glUseProgram(_id); }
void ShaderProgram::Unbind() const { glUseProgram(0); }

GLuint ShaderProgram::GetID() const { return _id; }

bool ShaderProgram::IsValid() const { return _id; }

GLuint ShaderProgram::CompileShader(GLenum type, const std::string& src, const std::string& path)
{
	GLuint shader = glCreateShader(type);

	auto srcC = src.c_str();
	glShaderSource(shader, 1, &srcC, nullptr);
	glCompileShader(shader);

	int success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infoLog[512] = {};
		glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
		glDeleteShader(shader);
		std::cerr << "[ShaderProgram] Compile failed: " << path << "\n" << infoLog << "\n";
		throw std::runtime_error("[ShaderProgram] Shader compilation failed: " + path + '\n' + infoLog);
	}

	return shader;
}

void ShaderProgram::BuildUniformReflection()
{
	_uniformCache.clear();

	GLint count = 0;
	glGetProgramiv(_id, GL_ACTIVE_UNIFORMS, &count);

	for (GLint i = 0; i < count; i++)
	{
		UniformInfo info;
		char name[128];
		GLsizei len = 0;
		GLint size = 0;
		GLenum type = 0;

		glGetActiveUniform(_id, i, sizeof(name), &len, &size, &type, name);

		std::string fullName(name, len);
		std::string baseName = fullName;
		if (auto bracket = baseName.find('['); bracket != std::string::npos)
			baseName.erase(bracket);

		info.location = glGetUniformLocation(_id, fullName.c_str());
		info.type = type;
		info.size = size;

		_uniformCache[baseName] = info;
	}
}

GLuint ShaderProgram::LinkProgram(GLuint vertex, GLuint fragment)
{
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	GLint success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512] = {};
		glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
		glDeleteProgram(program);
		throw std::runtime_error("[ShaderProgram] Program linking failed:\n" + std::string(infoLog));
	}
	return program;
}

std::string ShaderProgram::ReadFile(const std::string& path)
{
	std::ifstream file(path);
	std::stringstream ss;

	if (!file.is_open()) throw std::runtime_error("[ShaderProgram] Failed to open file: " + path);

	ss << file.rdbuf();

	return std::string(ss.str());
}

void ShaderProgram::SendMat4(const std::string& name, const glm::mat4& v) const
{
	GLint id = 0;
	FindUniform(name, id);
	glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(v));
}

void ShaderProgram::SendMat3(const std::string& name, const glm::mat3& v) const
{
	GLint id = 0;
	FindUniform(name, id);
	glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(v));
}

void ShaderProgram::SendVec4(const std::string& name, const glm::vec4& v) const
{
	GLint id = 0;
	FindUniform(name, id);
	glUniform4fv(id, 1, glm::value_ptr(v));
}

void ShaderProgram::SendVec3(const std::string& name, const glm::vec3& v) const
{
	GLint id = 0;
	FindUniform(name, id);
	glUniform3fv(id, 1, glm::value_ptr(v));
}

void ShaderProgram::SendVec2(const std::string& name, const glm::vec2& v) const
{
	GLint id = 0;
	FindUniform(name, id);
	glUniform2fv(id, 1, glm::value_ptr(v));
}

void ShaderProgram::SendFloat(const std::string& name, const float v) const
{
	GLint id = 0;
	FindUniform(name, id);
	glUniform1f(id, v);
}

void ShaderProgram::SendInt(const std::string& name, const int v) const
{
	GLint id = 0;
	FindUniform(name, id);
	glUniform1i(id, v);
}

void ShaderProgram::SendBool(const std::string& name, const bool v) const { SendInt(name, static_cast<int>(v)); }

void ShaderProgram::FindUniform(const std::string& name, GLint& id) const
{
	auto u = _uniformCache.find(name);
	if (u == _uniformCache.end())
		throw std::runtime_error("[ShaderProgram] Uniform not found: " + name + " for shaders - " + _vertexPath + ' ' + _fragmentPath);
	id = u->second.location;
}

void ShaderProgram::Destroy()
{
	if (_id)
	{
		glDeleteProgram(_id);
		_uniformCache.clear();
		_vertexPath.clear();
		_fragmentPath.clear();
		_id = 0;
	}
}