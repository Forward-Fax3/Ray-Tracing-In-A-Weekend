#pragma once
#define _HITDATA
#include <memory>

#include "glm/glm.hpp"

#include "Core.h"
#include "Interval.h"
#include "AxisAliagnedBoundingBoxes.h"
#include "Ray.h"

#ifndef _BaseMaterial
#include "BaseMaterial.h"
#endif


namespace RTW
{
	class BaseMaterial;

	struct HitData
	{
		Point point = Point(0.0);
		Vec3 normal = Vec3(0.0);
		UV uv = UV(0.0);
		std::shared_ptr<BaseMaterial> material = nullptr;
		double distance = 0.0;
		bool isFrontFace = false;

		inline void SetFaceNormal(const Ray& ray, const Vec3& outwardNormal)
		{
			isFrontFace = glm::dot(ray.direction(), outwardNormal) <= 0.0;
			normal = isFrontFace ? outwardNormal : -outwardNormal;
		}
	};

	class RayNoHit;

	class BaseRayHittable
	{
	public:
		virtual ~BaseRayHittable() = default;

		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const = 0;

		virtual const AABB& GetBoundingBox() const = 0;
		virtual void SetBoundingBox(const AABB& newBox) = 0;

		static std::shared_ptr<BaseRayHittable>& GetNoHit();
	};

	class RayNoHit final : public BaseRayHittable
	{
	public:
		RayNoHit() = default;

		bool IsRayHit([[maybe_unused]] const Ray& ray, [[maybe_unused]] const Interval& rayDistance, [[maybe_unused]] HitData& hitData) const override { return false; }

		const AABB& GetBoundingBox() const override { return AABB::empty; }
		inline void SetBoundingBox(const AABB& newAABB) override // not possible newAABB will be ignored
		{(void)newAABB;}

		static inline std::shared_ptr<BaseRayHittable>& GetNoHit() { return s_NoHit; }

	private:
		static std::shared_ptr<BaseRayHittable> s_NoHit;
		friend class BaseRayHittable;
	};

	inline std::shared_ptr<BaseRayHittable>& BaseRayHittable::GetNoHit() { return RayNoHit::s_NoHit; }
}
