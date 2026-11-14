#include "Picking.h"
#include <glm/mat4x4.hpp>

float Picking::RayAabbLocal(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& boundingMin, const glm::vec3& boundingMax)
{
	float tmin = -FLT_MAX, tmax = FLT_MAX;
	for (int i = 0; i < 3; i++)
	{
		if (std::abs(rayDirection[i]) < 1e-8f)
		{
			if (rayOrigin[i] < boundingMin[i] || rayOrigin[i] > boundingMax[i])
				return -1.0f;
		}
		else
		{
			float t1 = (boundingMin[i] - rayOrigin[i]) / rayDirection[i];
			float t2 = (boundingMax[i] - rayOrigin[i]) / rayDirection[i];
			if (t1 > t2) std::swap(t1, t2);
			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);
			if (tmin > tmax) return -1.0f;
		}
	}
	return tmax >= 0.0f ? (tmin >= 0.0f ? tmin : tmax) : -1.0f;
}

bool Picking::PointInsideAabb(const glm::vec3& pos, const glm::vec3& bmin, const glm::vec3& bmax)
{
	return pos.x >= bmin.x && pos.x <= bmax.x && pos.y >= bmin.y && pos.y <= bmax.y && pos.z >= bmin.z && pos.z <= bmax.z;
}