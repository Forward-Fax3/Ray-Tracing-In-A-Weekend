#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"

#include "Core.h"
#include "AxisAliagnedBoundingBoxes.h"
#include "RayHittable.h"
#include "Hittables.h"
#include "BoundingVolumeHierarchiesNode.h"


namespace RTW
{
	size_t BoundingVolumeHierarchiesNode::currentDepth = 0;
	size_t BoundingVolumeHierarchiesNode::maxDepth = 0;

	BVHNode::BoundingVolumeHierarchiesNode(RayHittables& hittables)
		: BVHNode(hittables.GetObjects(), 0, hittables.GetObjects().size()) {
		std::clog << "number of bounding boxes: " << AABB::GetNumberofBBs() << ", max depth: " << maxDepth << '\n' << std::flush;
	}

	BVHNode::BoundingVolumeHierarchiesNode(std::vector<std::shared_ptr<RayHittable>>& hittables, size_t start, size_t end)
	{
		m_AABB = AABB::empty;

		for (size_t i = start; i < end; i++)
			m_AABB = { m_AABB, hittables[i]->GetBoundingBox() };

		currentDepth++;
		maxDepth = glm::max(maxDepth, currentDepth);

		size_t hittablesRange = end - start;

		if (hittablesRange == 1)
			m_Left = m_Right = hittables[start];
		else if (hittablesRange == 2)
		{
			m_Left = hittables[start];
			m_Right = hittables[start + 1];
		}
		else
		{
			AABB::Axis axis = m_AABB.LongestAxis();

			auto compartionFunction = (axis == AABB::Axis::x) ? CompareBoxXAxis
									: (axis == AABB::Axis::y) ? CompareBoxYAxis
									: CompareBoxZAxis;

			std::sort(std::begin(hittables) + start, std::begin(hittables) + end, compartionFunction);

			size_t midPoint = start + hittablesRange / 2;
			m_Left = (midPoint - start == 1) ? hittables[start] : std::make_shared<BVHNode>(hittables, start, midPoint);
			m_Right = std::make_shared<BVHNode>(hittables, midPoint, end);
		}
		currentDepth--;
	}

	bool BVHNode::IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const
	{
		if (!m_AABB.IsHit(ray, rayDistance))
			return false;

		bool isHit = m_Left->IsRayHit(ray, rayDistance, hitData);
		isHit |= m_Right->IsRayHit(ray, { rayDistance.GetMin(), isHit ? hitData.distance : rayDistance.GetMax() }, hitData);

		return isHit;
	}

	bool BVHNode::BoxComparison(const std::shared_ptr<RayHittable> boxA, const std::shared_ptr<RayHittable> boxB, AABB::Axis axis)
	{
		Interval BoxAAxisInterval = boxA->GetBoundingBox().GetAxisInterval(axis);
		Interval BoxBAxisInterval = boxB->GetBoundingBox().GetAxisInterval(axis);
		return BoxAAxisInterval.GetMin() < BoxBAxisInterval.GetMin();
	}
}
