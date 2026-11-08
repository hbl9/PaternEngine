#pragma once

#include <cstddef>

struct VertexAttribute
{
	unsigned	index = 0;
	int			size = 0;
	unsigned	type = 0;
	bool		normalized = false;
	std::size_t	stride = 0;
	std::size_t	offset = 0;
};