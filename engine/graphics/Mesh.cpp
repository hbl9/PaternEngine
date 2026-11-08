#include "Mesh.h"
#include <glad/glad.h>
#include <stdexcept>

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

void Mesh::Upload(std::span<const float> vertices,
	std::span<const VertexAttribute> layout, std::span<const std::uint32_t> indices)
{
	Release();

	if (vertices.empty()) throw std::invalid_argument("[Mesh] Массив вершин пуст");
	if (layout.empty()) throw std::invalid_argument("[Mesh] Описание вершины пустое");
	if (indices.empty()) throw std::invalid_argument("[Mesh] Массив индексов пуст");

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