#include "Core.h"
#include "RayHittable.h"
#include "BaseBVH.h"


namespace RTW
{
	std::atomic<size_t> BVHBase::currentDepth = 0;
	std::atomic<size_t> BVHBase::maxDepth = 0;

	bool BVHBase::BoxComparison(std::shared_ptr<BaseRayHittable> boxA, std::shared_ptr<BaseRayHittable> boxB, AABB::Axis axis)
	{
		Interval BoxAAxisInterval = boxA->GetBoundingBox().GetAxisInterval(axis);
		Interval BoxBAxisInterval = boxB->GetBoundingBox().GetAxisInterval(axis);

		double BoxAAxisIntervalMin = BoxAAxisInterval.GetMin();
		double BoxBAxisIntervalMin = BoxBAxisInterval.GetMin();

		if (!Interval(-1e-4, 1e-4).Contains(BoxAAxisIntervalMin - BoxBAxisIntervalMin))
			return BoxAAxisIntervalMin < BoxBAxisIntervalMin;

		double BoxAAxisIntervalMax = BoxAAxisInterval.GetMax();
		double BoxBAxisIntervalMax = BoxBAxisInterval.GetMax();

		return BoxAAxisIntervalMax < BoxBAxisIntervalMax;
	}
}
