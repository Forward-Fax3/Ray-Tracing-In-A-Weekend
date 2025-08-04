#pragma once
#include "Core.h"
#include "RayHittable.h"


namespace RTW
{
	class BVHBase : public BaseRayHittable
	{
	public:
		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

		virtual const AABB& GetBoundingBox() const override { return m_AABB; }

	protected:
		static bool BoxComparison(const std::shared_ptr<BaseRayHittable>& boxA, const std::shared_ptr<BaseRayHittable>& boxB, AABB::Axis axis);
		static inline bool CompareBoxXAxis(const std::shared_ptr<BaseRayHittable>& boxA, const std::shared_ptr<BaseRayHittable>& boxB)
		{
			return BoxComparison(boxA, boxB, AABB::Axis::x);
		}
		static inline bool CompareBoxYAxis(const std::shared_ptr<BaseRayHittable>& boxA, const std::shared_ptr<BaseRayHittable>& boxB)
		{
			return BoxComparison(boxA, boxB, AABB::Axis::y);
		}
		static inline bool CompareBoxZAxis(const std::shared_ptr<BaseRayHittable>& boxA, const std::shared_ptr<BaseRayHittable>& boxB)
		{
			return BoxComparison(boxA, boxB, AABB::Axis::z);
		}

	protected:
		std::shared_ptr<BaseRayHittable> m_Left, m_Right;
		AABB m_AABB;

		static std::atomic<size_t> currentDepth, maxDepth;
	};
}