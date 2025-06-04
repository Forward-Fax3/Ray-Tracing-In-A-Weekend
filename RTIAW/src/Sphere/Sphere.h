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

		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

		static Vec3 RandomOnHemisphere(const Vec3& normal);

	private:
		static Vec3 RandomUnitVector();

	private:
		double m_Radius;
		Point m_Center;
	};
}
