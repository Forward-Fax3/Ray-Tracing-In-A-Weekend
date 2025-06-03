#pragma once
#include "glm/glm.hpp"

#include "Core.h"
#include "Interval.h"
#include "Ray.h"


namespace RTW
{
	struct HitData
	{
		Point point = Point(0.0);
		Vec3 normal = Vec3(0.0);
		double distance = 0.0;
		bool isFrontFace = false;
	};

	class RayHittable
	{
	public:
		virtual ~RayHittable() = default;

		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const = 0;

		inline void setFaceNormal(const Ray& ray, const Vec3& outwardNormal, HitData& hitData) const
		{
			hitData.isFrontFace = glm::dot(ray.direction(), outwardNormal);
			hitData.normal = hitData.isFrontFace ? outwardNormal : -outwardNormal;
		}
	};
}
