#include "Core.h"
#include "RayHittable.h"
#include "BaseBVH.h"


namespace RTW
{
	std::atomic<size_t> BVHBase::currentDepth = 0;
	std::atomic<size_t> BVHBase::maxDepth = 0;

	bool BVHBase::IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const
	{
		if (!m_AABB.IsHit(ray, rayDistance))
			return false;

		bool isHit = m_Left->IsRayHit(ray, rayDistance, hitData);

		if (isHit)
			isHit |= m_Right->IsRayHit(ray, { rayDistance.GetMin(), hitData.distance }, hitData);
		else
			isHit |= m_Right->IsRayHit(ray, rayDistance, hitData);

		return isHit;
	}

	bool BVHBase::BoxComparison(const std::shared_ptr<BaseRayHittable>& boxA, const std::shared_ptr<BaseRayHittable>& boxB, AABB::Axis axis)
	{
		Interval BoxAAxisInterval = boxA->GetBoundingBox().GetAxisInterval(axis);
		Interval BoxBAxisInterval = boxB->GetBoundingBox().GetAxisInterval(axis);
		return BoxAAxisInterval.GetMin() < BoxBAxisInterval.GetMin();
	}
}