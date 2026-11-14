#include "Mesh.h"
#include <graphics/VertexAttribute.h>
#include <glm/common.hpp>
#include <stdexcept>
#include <glad.h>

Mesh::Mesh() = default;
Mesh::~Mesh() { Release(); }

Mesh::Mesh(Mesh&& other) noexcept
	: _vao(other._vao), _vbo(other._vbo), _ebo(other._ebo), _indexCount(other._indexCount)
{
	other._vao = other._vbo = other._ebo = 0;
	other._indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (this != &other)
	{
		Release();
		_vao = other._vao; _vbo = other._vbo; _ebo = other._ebo;
		_indexCount = other._indexCount;
		other._vao = other._vbo = other._ebo = 0;
		other._indexCount = 0;
	}
	return *this;
}

const glm::vec3& Mesh::GetAabbMax() const { return _aabbMax; }
const glm::vec3& Mesh::GetAabbMin() const { return _aabbMin; }

bool Mesh::HasAabb() const { return _hasAabb; }

void Mesh::Upload(std::span<const float> vertices, std::span<const VertexAttribute> layout, std::span<const std::uint32_t> indices)
{
	Release();

	if (vertices.empty()) throw std::invalid_argument("[Mesh] Vertex array is empty");
	if (layout.empty())   throw std::invalid_argument("[Mesh] Vertex layout is empty");
	if (indices.empty())  throw std::invalid_argument("[Mesh] Index array is empty");

	ComputeAabb(vertices, layout);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);

	for (const VertexAttribute& attr : layout)
	{
		glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, static_cast<GLsizei>(attr.stride),
			reinterpret_cast<const void*>(attr.offset));
		glEnableVertexAttribArray(attr.index);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_indexCount = indices.size();
}

void Mesh::Draw() const
{
	Bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indexCount), GL_UNSIGNED_INT, nullptr);
}

void Mesh::Bind()   const { glBindVertexArray(_vao); }
void Mesh::Unbind() const { glBindVertexArray(0); }

void Mesh::Release()
{
	if (_vao == 0 && _vbo == 0 && _ebo == 0) return;

	Unbind();
	if (_ebo) glDeleteBuffers(1, &_ebo);
	if (_vbo) glDeleteBuffers(1, &_vbo);
	if (_vao) glDeleteVertexArrays(1, &_vao);

	_vao = _vbo = _ebo = 0;
	_indexCount = 0;
}

void Mesh::ComputeAabb(std::span<const float> vertices, std::span<const VertexAttribute> layout)
{
	_hasAabb = false;

	const VertexAttribute* posAttr = nullptr;
	for (const VertexAttribute& va : layout)
		if (!va.index)
		{
			posAttr = &va;
			break;
		}
	if (!posAttr || posAttr->size < 3) return;

	const std::size_t strideFloats = posAttr->stride / sizeof(float);
	if (!strideFloats) return;

	const std::size_t offsetFloats = posAttr->offset / sizeof(float);
	const std::size_t vertexCount = vertices.size() / strideFloats;

	glm::vec3 min(std::numeric_limits<float>::max());
	glm::vec3 max(std::numeric_limits<float>::lowest());

	for (std::size_t i = 0; i < vertexCount; i++)
	{
		const float* pos = vertices.data() + i * strideFloats + offsetFloats;
		const glm::vec3 vertex(pos[0], pos[1], pos[2]);
		min = glm::min(min, vertex);
		max = glm::max(max, vertex);
	}

	_aabbMin = min;
	_aabbMax = max;
	_hasAabb = true;
}