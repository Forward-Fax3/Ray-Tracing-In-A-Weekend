#pragma once
#include "Core.h"
#include "RayHittable.h"


namespace RTW
{
	class BVHBase : public BaseRayHittable
	{
	protected:
		static bool BoxComparison(std::shared_ptr<BaseRayHittable> boxA, std::shared_ptr<BaseRayHittable> boxB, AABB::Axis axis);

	protected:
		static std::atomic<size_t> currentDepth;
		static std::atomic<size_t> maxDepth;
	};
}