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
		m_AABB = AABB::empty;

		for (size_t i = start; i < end; i++)
			m_AABB = { m_AABB, hittables[i]->GetBoundingBox() };

		currentDepth++;
		maxDepth.store(std::max(maxDepth, currentDepth));

		size_t hittablesRange = end - start;

		if (hittablesRange == 1)
		{
			m_Left = hittables[start];
			m_Right = BaseRayHittable::GetNoHit();
		}
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
			m_Left = std::make_shared<BoundingVolumeHierarchiesNode>(hittables, start, midPoint);
			m_Right = std::make_shared<BoundingVolumeHierarchiesNode>(hittables, midPoint, end);
		}
		currentDepth--;
	}
}
