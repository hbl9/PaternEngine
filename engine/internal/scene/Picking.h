#pragma once

#include <glm/vec3.hpp>

namespace Picking
{
	float RayAabbLocal(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& boundingMin, const glm::vec3& boundingMax);
	bool PointInsideAabb(const glm::vec3& pos, const glm::vec3& boundingMin, const glm::vec3& boundingMax);
}