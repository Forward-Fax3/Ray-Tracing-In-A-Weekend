#pragma once
#include <memory>
#include <type_traits>
#include <array>

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"
#include "glm/gtx/compatibility.hpp"

#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"

#include "ONB.h"


namespace RTW::Templates
{
	template <bool t_IsMoving>
	class Sphere final : public BaseRayHittable
	{
	public:
		inline Sphere<t_IsMoving>(const Point& center, double radius, std::shared_ptr<BaseMaterial> material) requires (!t_IsMoving)
			: BaseRayHittable(AABB(center - radius, center + radius)), m_Radius(glm::max(0.0, radius)), m_Center(center), m_Material(material) {}

		inline Sphere<t_IsMoving>(const Point& center1, const Point& center2, double radius, std::shared_ptr<BaseMaterial> material) requires t_IsMoving
			: m_Radius(glm::max(0.0, radius)), m_Center(center1, center2 - center1), m_Material(material)
		{
			AABB aabb = AABB(m_Center.at(0.0) - radius, m_Center.at(0.0) + radius);
			aabb.Expand(AABB(m_Center.at(1.0) - radius, m_Center.at(1.0) + radius));
			SetBoundingBox(aabb);
		}

		bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override
		{
//			if (!this->GetBoundingBox().IsHit(ray, rayDistance))
//				return false;

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
			hitData.SetPoint(ray.at(root));
			hitData.material = m_Material.get();

			Vec3 normal = (hitData.point - center) / m_Radius;
			hitData.SetFaceNormal(ray, normal);
			hitData.uv = GetUV(hitData.normal);

			return true;
		}

		double PDFValue(const Point& origin, const Vec3& direction) const override
		{
			if (HitData data; !IsRayHit(Ray(origin, direction), Interval(0.001, doubleInf), data))
				return 0.0;

			double rDistance2;
			if constexpr (t_IsMoving)
				rDistance2 = 1.0 / glm::length2(m_Center.at(0.0) - origin);
			else
				rDistance2 = 1.0 / glm::length2(m_Center - origin);
			
			double cosThetaMax = glm::sqrt(1.0 - m_Radius * m_Radius * rDistance2);
			double solidAngle = glm::two_pi<double>() * (1.0 - cosThetaMax);

			return 1.0 / solidAngle;
		}

		Vec3 Random(const Point& origin) const override
		{
			Vec3 direction;
			if constexpr (t_IsMoving)
				direction = m_Center.at(0.0) - origin;
			else
				direction = m_Center - origin;

			double distance2 = glm::length2(direction);
			ONB onb(direction);
			return onb.Transform(RandomToSphere(distance2));
		}

	private:
		static RTW_FORCE_INLINE UV GetUV(const Point& point)
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

			return { phi * glm::one_over_two_pi<double>(), theta * glm::one_over_pi<double>() };
		}

		RTW_FORCE_INLINE Vec3 RandomToSphere(double distance2) const
		{
			auto r = glm::linearRand(Vec2(0.0), Vec2(1.0));

			auto z = 1.0 + r.y * (glm::sqrt(1.0 - m_Radius * m_Radius / distance2) - 1.0);
			auto phi = glm::two_pi<double>() * r.x;
			auto sqrt1MinusZ2 = glm::sqrt(1.0 - z * z);
			auto x = glm::cos(phi) * sqrt1MinusZ2;
			auto y = glm::sin(phi) * sqrt1MinusZ2;

			return { x, y, z };
		}

	private:
		using m_CenterType = std::conditional_t<t_IsMoving, Ray, Point>;

		double m_Radius;
		m_CenterType m_Center;
		std::shared_ptr<BaseMaterial> m_Material;
	};
}

namespace RTW
{
	using Sphere = Templates::Sphere<false>;
	using MovingSphere = Templates::Sphere<true>;
}
