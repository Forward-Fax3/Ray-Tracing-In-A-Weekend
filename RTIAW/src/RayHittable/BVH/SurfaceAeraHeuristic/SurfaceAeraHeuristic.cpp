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
	SurfaceAreaHeuristicNode::SurfaceAreaHeuristicNode(RayHittables& hittables)
		: SurfaceAreaHeuristicNode(hittables.GetObjects(), 0, hittables.GetObjects().size()) {
		std::clog << "number of bounding boxes: " << AABB::GetNumberofBBs() << ", max depth: " << maxDepth << ". Created with SAH.\n" << std::flush;
		maxDepth = 0;
	}

	SurfaceAreaHeuristicNode::SurfaceAreaHeuristicNode(RayHittables& hittables, size_t numberOfThreads)
		: SurfaceAreaHeuristicNode(hittables.GetObjects(), 0, hittables.GetObjects().size(), numberOfThreads) {
		// currently doesn't work with multi threading need to make a better system for it.
//		std::clog << "number of bounding boxes: " << AABB::GetNumberofBBs() << ", max depth: " << maxDepth << ". Created with SAH.\n" << std::flush;
		maxDepth = 0;
	}

	SurfaceAreaHeuristicNode::SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end)
	{
		m_AABB = AABB::empty;

		for (size_t i = start; i < end; i++)
			m_AABB = { m_AABB, hittables[i]->GetBoundingBox() };

		currentDepth++;
		maxDepth.store(std::max(maxDepth, currentDepth));

		SingleThreadedCalculateBVH(hittables, start, end);

		currentDepth--;
	}

	SurfaceAreaHeuristicNode::SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end, size_t numberOfThreads)
	{
		(void)numberOfThreads;
		m_AABB = AABB::empty;

		for (size_t i = start; i < end; i++)
			m_AABB = { m_AABB, hittables[i]->GetBoundingBox() };

		currentDepth++;
		maxDepth.store(std::max(maxDepth, currentDepth));

		//		s_MainThreadID = std::this_thread::get_id();
		//		s_Threads.resize(numberOfThreads - 1);

		MultiThreadedCalculateBVH(hittables, start, end);

		while (g_Threads.n_idle() != static_cast<int>(std::thread::hardware_concurrency()))
			std::this_thread::sleep_for(std::chrono::microseconds(50));

		currentDepth--;
	}

	SurfaceAreaHeuristicNode::SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end, AABB& thisAABB, bool isMultithreaded /*= false*/)
	{
		m_AABB = thisAABB;
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

		BestSplit bestSplit{};
		CalculateBestSplit(hittables, start, end, hittablesRange, bestSplit);

		if (bestSplit.axis != AABB::Axis::z)
		{
			auto compartionFunction = (bestSplit.axis == AABB::Axis::x) ? CompareBoxXAxis : CompareBoxYAxis;
			std::sort(hittables.begin() + start, hittables.begin() + end, compartionFunction);
		}

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

		if (bestSplit.axis != AABB::Axis::z)
		{
			auto compartionFunction = (bestSplit.axis == AABB::Axis::x) ? CompareBoxXAxis : CompareBoxYAxis;
			std::sort(hittables.begin() + start, hittables.begin() + end, compartionFunction);
		}

		if (bestSplit.SplitPosition == 1)
			m_Left = hittables[start];
		else
			g_Threads.push([&, start](int, size_t lambdaEnd, AABB lambdaAABB) {
					m_Left = std::make_shared<SAHNode>(hittables, start, lambdaEnd, lambdaAABB, true);
				}, start + bestSplit.SplitPosition, bestSplit.LeftAABB);

		m_Right = (end - bestSplit.SplitPosition == 1) ? hittables[end - 1] :
			std::make_shared<SurfaceAreaHeuristicNode>(hittables, start + bestSplit.SplitPosition, end, bestSplit.RightAABB, true);
	}

	void SurfaceAreaHeuristicNode::CalculateBestSplit(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end, size_t hittablesRange, BestSplit& bestSplit)
	{
		const size_t numberOfSplits = glm::min(glm::max(static_cast<size_t>(512), static_cast<size_t>(glm::sqrt(hittablesRange))), hittablesRange - 1);
		//		const size_t numberOfSplits = hittablesRange - 1;

		AABB leftAABB{};
		AABB rightAABB{};

		const double invertedIncermentedNumberOsSplits = 1.0 / static_cast<double>(numberOfSplits + 1);
		const double thisAABBInvertedSurfaceArea = 1.0 / m_AABB.GetSurfaceArea();

		for (AABB::Axis axis = AABB::Axis::x; axis <= AABB::Axis::z; axis++)
		{
			auto compartionFunction = (axis == AABB::Axis::x) ? CompareBoxXAxis :
				(axis == AABB::Axis::y) ? CompareBoxYAxis : CompareBoxZAxis;

			std::sort(hittables.begin() + start, hittables.begin() + end, compartionFunction);

			for (size_t split = 1; split < numberOfSplits + 1; split++)
			{
				leftAABB = AABB::empty;
				rightAABB = AABB::empty;

				size_t splitPosition = static_cast<size_t>((static_cast<double>(split) * static_cast<double>(hittablesRange)) * invertedIncermentedNumberOsSplits);

				for (auto i = hittables.begin() + start; i != hittables.begin() + start + splitPosition; i++)
					leftAABB.Expand((*i)->GetBoundingBox());
				for (auto i = hittables.begin() + start + splitPosition; i != hittables.begin() + end; i++)
					rightAABB.Expand((*i)->GetBoundingBox());

				double leftSufaceArea = leftAABB.GetSurfaceArea();
				double rightSurfaceArea = rightAABB.GetSurfaceArea();

				double leftNumberOfItems = static_cast<double>(splitPosition);
				double rightNumbeOfItems = static_cast<double>(hittablesRange - splitPosition);

				double cost = 0.25 + (leftNumberOfItems * leftSufaceArea + rightNumbeOfItems * rightSurfaceArea) * thisAABBInvertedSurfaceArea;

				if (cost < bestSplit.Cost)
				{
					bestSplit.LeftAABB = leftAABB;
					bestSplit.RightAABB = rightAABB;
					bestSplit.Cost = cost;
					bestSplit.SplitPosition = splitPosition;
					bestSplit.axis = axis;
				}
			}
		}
	}
}
