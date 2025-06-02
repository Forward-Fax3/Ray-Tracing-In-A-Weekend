#pragma once
#include "glm/glm.hpp"

#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"


namespace RTW
{
	class Sphere : public RayHittable
	{
	public:
		Sphere(const Point& center, double radius);

		virtual bool IsRayHit(const Ray& ray, double rayDistanceMin, double rayDistanceMax, HitData& hitData) const override;

	private:
		double m_Radius;
		Point m_Center;
	};
}
