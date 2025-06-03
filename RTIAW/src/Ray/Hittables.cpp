#include <memory>

#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "Hittables.h"


namespace RTW
{
	bool RayHittables::IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& data) const
	{
		bool hasHit = false;
		Interval tempRayDist(rayDistance);

		for (const std::shared_ptr<RayHittable>& object : m_Objects)
			if (object->IsRayHit(ray, tempRayDist, data))
			{
				hasHit = true;
				tempRayDist.SetMax(data.distance);
			}

		return hasHit;
	}
}