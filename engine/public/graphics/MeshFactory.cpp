#include "MeshFactory.h"
#include <glad.h>

MeshData MeshFactory::CreateCube(float size)
{
	size /= 2.0f;
	MeshData data;
	data.vertices =
	{
		//x      y      z
		// Задняя
		-size, -size, -size,  // 0
		 size, -size, -size,  // 1
		 size,  size, -size,  // 2
		-size,  size, -size,  // 3
		// Передняя
		-size, -size,  size,  // 4
		 size, -size,  size,  // 5
		 size,  size,  size,  // 6
		-size,  size,  size,  // 7
	};
	data.indices =
	{
		// Передняя
		4, 5, 6,
		4, 6, 7,
		// Задняя
		1, 0, 3,
		1, 3, 2,
		// Левая
		0, 4, 7,
		0, 7, 3,
		// Правая
		5, 1, 2,
		5, 2, 6,
		// Верхняя
		3, 7, 6,
		3, 6, 2,
		// Нижняя
		0, 1, 5,
		0, 5, 4,
	};

	constexpr std::size_t stride = 3 * sizeof(float);
	data.layout = { {0, 3, GL_FLOAT, false, stride, 0 } };

	return data;
}

MeshData MeshFactory::CreatePlane(float width, float length, int subdivX, int subdivY)
{
	MeshData data;
	if (subdivX < 1) subdivX = 1;
	if (subdivY < 1) subdivY = 1;

	for (int y = 0; y <= subdivY; y++)
		for (int x = 0; x <= subdivX; x++)
		{
			float fy = static_cast<float>(y) / subdivY;
			float fx = static_cast<float>(x) / subdivX;

			float px = -width * 0.5f + fx * width;
			float pz = -length * 0.5f + fy * length;

			data.vertices.insert(data.vertices.end(), { px, 0.0f, pz });
		}

	int strideDiv = subdivX + 1;
	for (int y = 0; y < subdivY; y++)
		for (int x = 0; x < subdivX; x++)
		{
			unsigned i0 = static_cast<unsigned>(y * strideDiv + x);
			unsigned i1 = i0 + 1;
			unsigned i2 = i0 + strideDiv;
			unsigned i3 = i2 + 1;

			data.indices.insert(data.indices.end(), { i0, i2, i1,
													  i1, i2, i3 });
		}

	constexpr std::size_t stride = 3 * sizeof(float);
	data.layout = { {0, 3, GL_FLOAT, false, stride, 0 } };

	return data;
}
// TODO
MeshData MeshFactory::CreateSphere(float radius, int segments, int rings)
{
	return {};
}

MeshData MeshFactory::CreateCylinder(float radius, float height, int segments)
{
	return {};
}