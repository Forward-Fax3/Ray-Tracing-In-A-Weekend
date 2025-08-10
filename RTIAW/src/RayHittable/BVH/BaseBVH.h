#pragma once
#include "Core.h"
#include "RayHittable.h"


namespace RTW
{
	class BVHBase : public BaseRayHittable
	{
	public:
		bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

		const AABB& GetBoundingBox() const override { return m_AABB; }

	protected:
		static bool BoxComparison(std::shared_ptr<BaseRayHittable> boxA, std::shared_ptr<BaseRayHittable> boxB, AABB::Axis axis);

	protected:
		std::shared_ptr<BaseRayHittable> m_Left;
		std::shared_ptr<BaseRayHittable> m_Right;
		AABB m_AABB;

		static std::atomic<size_t> currentDepth;
		static std::atomic<size_t> maxDepth;
	};
}