#pragma once
#include "Core.h"
#include "RayHittable.h"


namespace RTW
{
	class BVHBase : public BaseRayHittable
	{
	public:
		BVHBase() = default;

		bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const final;

		const AABB& GetBoundingBox() const final { return m_AABB; }
		inline void SetBoundingBox(const AABB& newAABB) final // newAABB must be bigger than or equal to current AABB in the x, y and z axises otherwise nothing will happen. 
		{
			if (newAABB.IsBigger(this->m_AABB))
				m_AABB = newAABB;
		}

	protected:
		inline explicit BVHBase(const AABB& thisAABB)
			: m_AABB(thisAABB) {}

		static bool BoxComparison(std::shared_ptr<BaseRayHittable> boxA, std::shared_ptr<BaseRayHittable> boxB, AABB::Axis axis);

	protected:
		std::shared_ptr<BaseRayHittable> m_Left;
		std::shared_ptr<BaseRayHittable> m_Right;
		AABB m_AABB;

		static std::atomic<size_t> currentDepth;
		static std::atomic<size_t> maxDepth;
	};
}