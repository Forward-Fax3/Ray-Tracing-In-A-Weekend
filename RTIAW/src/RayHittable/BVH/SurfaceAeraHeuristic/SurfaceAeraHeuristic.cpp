#include <memory>
#include <vector>
#include <iostream>

#include "Core.h"
#include "RayHittable.h"
#include "RayHittables.h"
#include "BaseBVH.h"
#include "SurfaceAeraHeuristic.h"


namespace RTW
{
	SurfaceAreaHeuristicNode::SurfaceAreaHeuristicNode(std::shared_ptr<RayHittables> hittables)
		: SurfaceAreaHeuristicNode(hittables->GetObjects(), 0, hittables->GetObjects().size()) {
		std::clog << "number of bounding boxes: " << AABB::GetNumberofBBs() << ", max depth: " << maxDepth << ". Created with SAH.\n" << std::flush;
		maxDepth = 0;
	}

	SurfaceAreaHeuristicNode::SurfaceAreaHeuristicNode(std::shared_ptr<RayHittables> hittables, size_t numberOfThreads)
		: SurfaceAreaHeuristicNode(hittables->GetObjects(), 0, hittables->GetObjects().size(), numberOfThreads) {
		// currently doesn't work with multi threading need to make a better system for it.
//		std::clog << "number of bounding boxes: " << AABB::GetNumberofBBs() << ", max depth: " << maxDepth << ". Created with SAH.\n" << std::flush;
		maxDepth = 0;
	}

	SurfaceAreaHeuristicNode::SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end)
	{
		m_AABB = AABB::empty;

		for (auto i = hittables.begin() + start; i != hittables.begin() + end; i++)
			m_AABB.Expand((*i)->GetBoundingBox());

		currentDepth++;
		maxDepth.store(std::max(maxDepth, currentDepth));

		SingleThreadedCalculateBVH(hittables, start, end);

		currentDepth--;
	}

	SurfaceAreaHeuristicNode::SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end, size_t numberOfThreads)
	{
		(void)numberOfThreads;

		for (auto i = hittables.begin() + start; i != hittables.begin() + end; i++)
			m_AABB.Expand((*i)->GetBoundingBox());

		currentDepth++;
		maxDepth.store(std::max(maxDepth, currentDepth));

		MultiThreadedCalculateBVH(hittables, start, end);

		while (g_Threads.n_idle() != static_cast<int>(std::thread::hardware_concurrency()))
			std::this_thread::sleep_for(std::chrono::microseconds(50));

		currentDepth--;
	}

	SurfaceAreaHeuristicNode::SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end, const AABB& thisAABB, bool isMultithreaded /*= false*/)
		: BVHBase(thisAABB)
	{
//		for (size_t i = start; i < end; i++)
//			m_AABB.Expand(hittables[i]->GetBoundingBox());

		//for (auto i = hittables.begin() + start; i != hittables.begin() + end; i++)
		//	leftAABB.Expand((*i)->GetBoundingBox());

		currentDepth++;
		maxDepth.store(std::max(maxDepth, currentDepth));

		if (isMultithreaded)
			MultiThreadedCalculateBVH(hittables, start, end);
		else
			SingleThreadedCalculateBVH(hittables, start, end);

		currentDepth--;
	}

	void SurfaceAreaHeuristicNode::SingleThreadedCalculateBVH(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end)
	{
		size_t hittablesRange = end - start;

		if (hittablesRange == 1)
		{
			m_Left = hittables[start];
			m_Right = BaseRayHittable::GetNoHit();
			return;
		}
		if (hittablesRange == 2)
		{
			m_Left = hittables[start];
			m_Right = hittables[start + 1];
			return;
		}

		BestSplit bestSplit;
		CalculateBestSplit(hittables, start, end, hittablesRange, bestSplit);

		if (bestSplit.axis != AABB::Axis::x)
			std::sort(hittables.begin() + start, hittables.begin() + end, [axis = bestSplit.axis](auto boxA, auto boxB) -> bool {
				return BoxComparison(boxA, boxB, axis);
			});

		m_Left = (bestSplit.SplitPosition == 1) ? hittables[start] :
			std::make_shared<SurfaceAreaHeuristicNode>(hittables, start, start + bestSplit.SplitPosition, bestSplit.LeftAABB);

		m_Right = (end - bestSplit.SplitPosition == 1) ? hittables[end - 1] :
			std::make_shared<SurfaceAreaHeuristicNode>(hittables, start + bestSplit.SplitPosition, end, bestSplit.RightAABB);
	}

	void SurfaceAreaHeuristicNode::MultiThreadedCalculateBVH(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end)
	{
		size_t hittablesRange = end - start;

		if (hittablesRange == 1)
		{
			m_Left = hittables[start];
			m_Right = BaseRayHittable::GetNoHit();
			return;
		}
		if (hittablesRange == 2)
		{
			m_Left = hittables[start];
			m_Right = hittables[start + 1];
			return;
		}
		
		BestSplit bestSplit{};
		CalculateBestSplit(hittables, start, end, hittablesRange, bestSplit);

		if (bestSplit.axis != AABB::Axis::x)
			std::sort(hittables.begin() + start, hittables.begin() + end, [axis = bestSplit.axis](auto boxA, auto boxB) -> bool {
				return BoxComparison(boxA, boxB, axis);
			});

		if (bestSplit.SplitPosition == 1)
			m_Left = hittables[start];
		else
			g_Threads.push([this, &hittables, start, lambdaEnd = start + bestSplit.SplitPosition, lambdaAABB = bestSplit.LeftAABB](int) {
					this->m_Left = std::make_shared<SAHNode>(hittables, start, lambdaEnd, lambdaAABB, true);
				});

		m_Right = (end - bestSplit.SplitPosition == 1) ? hittables[end - 1] :
			std::make_shared<SurfaceAreaHeuristicNode>(hittables, start + bestSplit.SplitPosition, end, bestSplit.RightAABB, true);
	}

	void SurfaceAreaHeuristicNode::CalculateBestSplit(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end, size_t hittablesRange, BestSplit& bestSplit) const
	{
		const size_t numberOfSplits = glm::min(glm::max(static_cast<size_t>(512), static_cast<size_t>(glm::sqrt(hittablesRange))), hittablesRange - 1);

		AABB leftAABB;
		AABB rightAABB;

		const double invertedIncermentedNumberOfSplits = 1.0 / static_cast<double>(numberOfSplits + 1);
		const double thisAABBInvertedSurfaceArea = 1.0 / m_AABB.GetSurfaceArea();

		for (AABB::Axis axis = AABB::Axis::x; axis <= AABB::Axis::z; ++axis)
		{
			std::sort(hittables.begin() + start, hittables.begin() + end, [axis](auto boxA, auto boxB) -> bool {
				return BoxComparison(boxA, boxB, axis);
			});

			for (size_t split = 1; split < numberOfSplits + 1; split++)
			{
				const auto splitPosition = static_cast<size_t>(static_cast<double>(split) * static_cast<double>(hittablesRange) * invertedIncermentedNumberOfSplits);

				for (auto i = hittables.begin() + start; i != hittables.begin() + start + splitPosition; i++)
					leftAABB.Expand((*i)->GetBoundingBox());
				for (auto i = hittables.begin() + start + splitPosition; i != hittables.begin() + end; i++)
					rightAABB.Expand((*i)->GetBoundingBox());

				const double leftSurfaceArea = leftAABB.GetSurfaceArea();
				const double rightSurfaceArea = rightAABB.GetSurfaceArea();

				const auto leftNumberOfItems = static_cast<double>(splitPosition);
				const auto rightNumberOfItems = static_cast<double>(hittablesRange - splitPosition);

				const double cost = 0.25 + (leftNumberOfItems * leftSurfaceArea + rightNumberOfItems * rightSurfaceArea) * thisAABBInvertedSurfaceArea;

				if (cost < bestSplit.Cost)
				{
					bestSplit.LeftAABB = leftAABB;
					bestSplit.RightAABB = rightAABB;
					bestSplit.Cost = cost;
					bestSplit.SplitPosition = splitPosition;
					bestSplit.axis = axis;
				}

				leftAABB = rightAABB = AABB::empty;
			}
		}
	}
}
