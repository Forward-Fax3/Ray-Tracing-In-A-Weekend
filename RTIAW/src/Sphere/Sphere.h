#pragma once
#include <memory>

#include "glm/glm.hpp"

#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"


namespace RTW
{
	class Sphere : public RayHittable
	{
	public:
		Sphere(const Point& center, double radius, std::shared_ptr<BaseMaterial> material);

		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

	private:
		double m_Radius;
		Point m_Center;
		std::shared_ptr<BaseMaterial> m_Material;
	};
}
