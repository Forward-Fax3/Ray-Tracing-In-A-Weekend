#pragma once
#include <memory>
#include <vector>

#include "Core.h"
#include "RayHittable.h"
#include "BaseBVH.h"


namespace RTW
{
	// median based BVH (slow but fast to create)
	class BoundingVolumeHierarchiesNode : public BVHBase
	{
	public:
		explicit BoundingVolumeHierarchiesNode(RayHittables& hittables);
		BoundingVolumeHierarchiesNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end);
	};

	using BVHNode = BoundingVolumeHierarchiesNode;
}
