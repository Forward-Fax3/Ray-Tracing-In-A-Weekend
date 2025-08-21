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

	// point must be written to before distance
	struct HitData
	{
		union
		{
			// point must be written to before distance
			Point point;

			struct
			{
				glm::highp_dvec3 padding1;
				// point must be written to before distance
				double distance = 0.0;
			};
		};

		union
		{
			Vec3 normal;

			struct
			{
				glm::highp_dvec3 padding2;
				bool isFrontFace = false;
			};
		};

		UV uv;
		std::shared_ptr<BaseMaterial> material;

		inline void SetFaceNormal(const Ray& ray, const Vec3& outwardNormal)
		{
			bool tempIsFrontFace = glm::dot(ray.direction(), outwardNormal) <= 0.0;
			normal = tempIsFrontFace ? outwardNormal : -outwardNormal;
			isFrontFace = tempIsFrontFace;
		}
	};

	class BaseRayHittable
	{
	public:
		virtual ~BaseRayHittable() = default;

		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const = 0;

		virtual const AABB& GetBoundingBox() const = 0;
		virtual void SetBoundingBox(const AABB& newBox) = 0;

		static std::shared_ptr<BaseRayHittable> GetNoHit();
	};

	class RayNoHit final : public BaseRayHittable
	{
	public:
		RayNoHit() = default;

		bool IsRayHit(const Ray&, const Interval&, HitData&) const override { return false; }

		const AABB& GetBoundingBox() const override { return AABB::empty; }
		inline void SetBoundingBox(const AABB&) override { /* not possible newAABB will be ignored */ }

		static inline std::shared_ptr<BaseRayHittable> GetNoHit()
		{
			static std::shared_ptr<BaseRayHittable> noHit = std::make_shared<RayNoHit>();
			return noHit;
		}
	};

	inline std::shared_ptr<BaseRayHittable> BaseRayHittable::GetNoHit() { return RayNoHit::GetNoHit(); }
}
