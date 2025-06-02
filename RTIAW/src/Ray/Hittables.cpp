#include <memory>

#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "Hittables.h"


namespace RTW
{
	bool RayHittables::IsRayHit(const Ray& ray, double rayDistanceMin, double rayDistanceMax, HitData& data) const
	{
		bool hasHit = false;
		double closest = rayDistanceMax;

		for (const std::shared_ptr<RayHittable>& object : m_Objects)
			if (object->IsRayHit(ray, rayDistanceMin, closest, data))
			{
				hasHit = true;
				closest = data.distance;
			}

		return hasHit;
	}
}