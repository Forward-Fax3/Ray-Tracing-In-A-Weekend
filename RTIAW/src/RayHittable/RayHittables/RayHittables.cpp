#include <memory>

#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "RayHittables.h"


namespace RTW
{
	bool RayHittables::IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& data) const
	{
		if (!m_AABB.IsHit(ray, rayDistance))
			return false;

		bool hasHit = false;
		Interval tempRayDist(rayDistance);

		for (auto& object : m_Objects)
			if (object->IsRayHit(ray, tempRayDist, data))
			{
				hasHit = true;
				tempRayDist.SetMax(data.distance);
			}

		return hasHit;
	}
}
