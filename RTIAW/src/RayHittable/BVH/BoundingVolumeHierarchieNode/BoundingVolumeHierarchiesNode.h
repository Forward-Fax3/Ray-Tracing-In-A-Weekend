#pragma once
#include <memory>
#include <vector>

#include "Core.h"
#include "RayHittable.h"
#include "BaseBVH.h"


namespace RTW
{
	// median based BVH (slow but fast to create)
	class BoundingVolumeHierarchiesNode final : public BVHBase
	{
	public:
		explicit BoundingVolumeHierarchiesNode(std::shared_ptr<RayHittables> hittables);
		BoundingVolumeHierarchiesNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end);

		bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

	private:
		std::shared_ptr<BaseRayHittable> m_Left;
		std::shared_ptr<BaseRayHittable> m_Right;
	};

	using BVHNode = BoundingVolumeHierarchiesNode;
}
