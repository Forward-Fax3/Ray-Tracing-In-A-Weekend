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

	bool BVHBase::BoxComparison(std::shared_ptr<BaseRayHittable> boxA, std::shared_ptr<BaseRayHittable> boxB, AABB::Axis axis)
	{
		Interval BoxAAxisInterval = boxA->GetBoundingBox().GetAxisInterval(axis);
		Interval BoxBAxisInterval = boxB->GetBoundingBox().GetAxisInterval(axis);

		double BoxAAxisIntervalMin = BoxAAxisInterval.GetMin();
		double BoxBAxisIntervalMin = BoxBAxisInterval.GetMin();

		if (!Interval(-1e-3, 1e-3).Contains(BoxAAxisIntervalMin - BoxBAxisIntervalMin))
			return BoxAAxisIntervalMin < BoxBAxisIntervalMin;

		double BoxAAxisIntervalMax = BoxAAxisInterval.GetMax();
		double BoxBAxisIntervalMax = BoxBAxisInterval.GetMax();

		if (!Interval(-1e-3, 1e-3).Contains(BoxAAxisIntervalMax - BoxBAxisIntervalMax))
			return BoxAAxisIntervalMax < BoxBAxisIntervalMax;

		AABB tempBoxA(boxA->GetBoundingBox());
		switch (axis)
		{
		case AABB::Axis::x:
			tempBoxA.Expand(AABB(Interval(tempBoxA.GetAxisInterval(AABB::Axis::x).GetMin() - 1e-4, 0.0), Interval(0.0, 0.0), Interval(0.0, 0.0)));
			break;
		case AABB::Axis::y:
			tempBoxA.Expand(AABB(Interval(0.0, 0.0), Interval(tempBoxA.GetAxisInterval(AABB::Axis::y).GetMin() - 1e-4, 0.0), Interval(0.0, 0.0)));
			break;
		case AABB::Axis::z:
			tempBoxA.Expand(AABB(Interval(0.0, 0.0), Interval(0.0, 0.0), Interval(tempBoxA.GetAxisInterval(AABB::Axis::z).GetMin() - 1e-4, 0.0)));
			break;
		case AABB::Axis::none:
		default:
			__debugbreak(); // should be impossible
		}

		boxA->SetBoundingBox(tempBoxA);
		BoxAAxisIntervalMin = boxA->GetBoundingBox().GetAxisInterval(axis).GetMin();

		return BoxAAxisIntervalMin < BoxBAxisIntervalMin;
	}
}
