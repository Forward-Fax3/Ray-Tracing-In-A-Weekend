#pragma once
#include <memory>

#include "glm/glm.hpp"

#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"


namespace RTW::Templates
{
	template <bool t_IsMoving>
	class Sphere : public RayHittable
	{
	public:
		Sphere<false>(const Point& center, double radius, std::shared_ptr<BaseMaterial> material);
		Sphere<true>(const Point& center1, const Point& center2, double radius, std::shared_ptr<BaseMaterial> material);

		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

		inline virtual const AABB& GetBoundingBox() const override { return m_AABB; }

	private:
		static inline UV GetUV(const Point& point);

	private:
		using m_CenterType = std::conditional_t<t_IsMoving, Ray, Point>;

		double m_Radius;
		m_CenterType m_Center;
		std::shared_ptr<BaseMaterial> m_Material;
		AABB m_AABB;
	};
}

namespace RTW
{
	using Sphere = Templates::Sphere<false>;
	using MovingSphere = Templates::Sphere<true>;
}
