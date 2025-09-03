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
		inline HitData()
		{
			point.data = glm::dvec4(0.0).data; // will also set distance to 0
			normal.data = glm::dvec4(0.0).data; // will also set isFaceNormal to false
			uv.data = glm::dvec2(0.0).data;
		}

		union
		{
			// point must be written to before distance
			Point point{};

			struct
			{
				glm::highp_dvec3 padding1;
				// point must be written to before distance
				double distance;
			};
		};

		union
		{
			Vec3 normal{};

			struct
			{
				glm::highp_dvec3 padding2;
				
				union
				{
					bool isFrontFace;
					double isFrontFaceAsDouble;
				};
			};
		};

		UV uv{};
		std::shared_ptr<BaseMaterial> material;

		inline void SetFaceNormal(const Ray& ray, const Vec3& outwardNormal)
		{
			isFrontFace = glm::dot(ray.direction(), outwardNormal) <= 0.0;
			this->SetNormal(isFrontFace ? outwardNormal : -outwardNormal);
		}

		inline void SetPoint(const Point& newPoint)
		{
			point.data = glm::dvec4(newPoint, distance).data;
		}

		inline void SetNormal(const Vec3& newNormal)
		{
			normal.data = glm::dvec4(newNormal, isFrontFaceAsDouble).data;
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
	private:
		struct PRIVATE { consteval explicit PRIVATE() = default; };

	public:
		RayNoHit() = delete;
		explicit RayNoHit(PRIVATE) {};

		bool IsRayHit(const Ray&, const Interval&, HitData&) const override { return false; }

		const AABB& GetBoundingBox() const override { return AABB::empty; }
		inline void SetBoundingBox(const AABB&) override { /* not possible newAABB will be ignored */ }

		static inline std::shared_ptr<BaseRayHittable> GetNoHit()
		{
			static auto noHit(std::make_shared<RayNoHit>(PRIVATE()));
			return noHit;
		}
	};

	inline std::shared_ptr<BaseRayHittable> BaseRayHittable::GetNoHit() { return RayNoHit::GetNoHit(); }
}
