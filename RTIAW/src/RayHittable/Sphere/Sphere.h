#pragma once
#include <memory>
#include <type_traits>
#include <array>

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"
#include "glm//gtx/compatibility.hpp"

#include "Core.h"
#include "Ray.h"
#include "RayHittable.h" 


namespace RTW::Templates
{
	template <bool t_IsMoving>
	class Sphere : public BaseRayHittable
	{
	public:
		inline Sphere<t_IsMoving>(const Point& center, double radius, std::shared_ptr<BaseMaterial> material) requires (!t_IsMoving)
			: m_Radius(glm::max(0.0, radius)), m_Center(center), m_Material(material), m_AABB(center - radius, center + radius) {}

		inline Sphere<t_IsMoving>(const Point& center1, const Point& center2, double radius, std::shared_ptr<BaseMaterial> material) requires (t_IsMoving)
			: m_Radius(glm::max(0.0, radius)), m_Center(center1, center2 - center1), m_Material(material)
		{
			std::array<AABB, 2> box{
				AABB(m_Center.at(0.0) - radius, m_Center.at(0.0) + radius),
				AABB(m_Center.at(1.0) - radius, m_Center.at(1.0) + radius)
			};
			m_AABB = AABB(box[0], box[1]);
		}

		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override
		{
			if (!m_AABB.IsHit(ray, rayDistance))
				return false;

			Vec3 center;
			if constexpr (t_IsMoving)
				center = m_Center.at(ray.time());
			else
				center = m_Center;

			Vec3 oc = center - ray.origin();
			double a = glm::length2(ray.direction());
			double h = glm::dot(ray.direction(), oc);
			double c = glm::length2(oc) - m_Radius * m_Radius;

			double discriminant = h * h - a * c;
			if (discriminant <= 0.0)
				return false;

			double sqrtDiscriminant = glm::sqrt(discriminant);

			double root = (h - sqrtDiscriminant) / a;
			if (!rayDistance.Contains(root))
			{
				root = (h + sqrtDiscriminant) / a;
				if (!rayDistance.Contains(root))
					return false;
			}

			hitData.distance = root;
			hitData.point = ray.at(hitData.distance);
			hitData.material = m_Material;

			Vec3 normal = (hitData.point - center) / m_Radius;
			hitData.SetFaceNormal(ray, normal);
			hitData.uv = GetUV(hitData.normal);

			return true;
		}

		inline virtual const AABB& GetBoundingBox() const override { return m_AABB; }

	private:
		static inline UV GetUV(const Point& point)
		{
			/*
			point: a given point on the sphere of radius one, centered at the origin.
			u: returned value [0,1] of angle around the Y axis from X=-1.
			v: returned value [0,1] of angle from Y=-1 to Y=+1.
				<1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
				<0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
				<0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
			*/

			double theta = glm::acos(-point.y);
			double phi = glm::atan2(-point.z, point.x) + glm::pi<double>();

			return { phi / glm::two_pi<double>(), theta / glm::pi<double>() };
		}

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
