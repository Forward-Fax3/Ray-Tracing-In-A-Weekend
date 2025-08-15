#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"

#include "Core.h"
#include "AxisAliagnedBoundingBoxes.h"
#include "RayHittable.h"
#include "RayHittables.h"
#include "BoundingVolumeHierarchiesNode.h"


namespace RTW
{
	BoundingVolumeHierarchiesNode::BoundingVolumeHierarchiesNode(RayHittables& hittables)
		: BoundingVolumeHierarchiesNode(hittables.GetObjects(), 0, hittables.GetObjects().size()) {
		std::clog << "number of bounding boxes: " << AABB::GetNumberofBBs() << ", max depth: " << maxDepth << ". Created With midpoints.\n" << std::flush;
		maxDepth = 0;
	}

	BoundingVolumeHierarchiesNode::BoundingVolumeHierarchiesNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end) 
	{
		for (auto i = hittables.begin() + start; i != hittables.begin() + end; i++)
			m_AABB.Expand((*i)->GetBoundingBox());

		currentDepth++;
		maxDepth.store(std::max(maxDepth, currentDepth));

		size_t hittablesRange = end - start;

		if (hittablesRange == 1)
		{
			m_Left = hittables[start];
			m_Right = BaseRayHittable::GetNoHit();
			currentDepth--;
			return;
		}
		if (hittablesRange == 2)
		{
			m_Left = hittables[start];
			m_Right = hittables[start + 1];
			currentDepth--;
			return;
		}

		AABB::Axis axis = m_AABB.LongestAxis();

		std::sort(std::begin(hittables) + start, std::begin(hittables) + end, [axis](auto boxA, auto boxB) -> bool {
			return BoxComparison(boxA, boxB, axis);
		});

		size_t midPoint = start + hittablesRange / 2;
		m_Left = std::make_shared<BoundingVolumeHierarchiesNode>(hittables, start, midPoint);
		m_Right = std::make_shared<BoundingVolumeHierarchiesNode>(hittables, midPoint, end);

		currentDepth--;
	}
}
