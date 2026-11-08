#pragma once

#include "VertexAttribute.h"
#include <cstdint>
#include <vector>

struct MeshData
{
	std::vector<float> vertices;
	std::vector<std::uint32_t> indices;
	std::vector<VertexAttribute> layout;
};

namespace MeshFactory
{
	MeshData CreateCube(float size = 1.0f);
	MeshData CreatePlane(float width, float length, int subdivX = 1, int subdivY = 1);
	MeshData CreateSphere(float radius, int segments, int rings);
	MeshData CreateCylinder(float radius, float height, int segments);
}