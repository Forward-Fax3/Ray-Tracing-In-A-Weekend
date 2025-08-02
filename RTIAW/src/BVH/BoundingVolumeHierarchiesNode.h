#pragma once
#include <memory>
#include <vector>

#include "Core.h"
#include "AxisAliagnedBoundingBoxes.h"
#include "RayHittable.h"
#include "Hittables.h"

#include "ctpl_stl.h"


namespace RTW
{
	class BVHBase : public RayHittable
	{
	public:
		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

		virtual const AABB& GetBoundingBox() const override { return m_AABB; }

	protected:
		static bool BoxComparison(const std::shared_ptr<RayHittable>& boxA, const std::shared_ptr<RayHittable>& boxB, AABB::Axis axis);
		static inline bool CompareBoxXAxis(const std::shared_ptr<RayHittable>& boxA, const std::shared_ptr<RayHittable>& boxB)
		{
			return BoxComparison(boxA, boxB, AABB::Axis::x);
		}
		static inline bool CompareBoxYAxis(const std::shared_ptr<RayHittable>& boxA, const std::shared_ptr<RayHittable>& boxB)
		{
			return BoxComparison(boxA, boxB, AABB::Axis::y);
		}
		static inline bool CompareBoxZAxis(const std::shared_ptr<RayHittable>& boxA, const std::shared_ptr<RayHittable>& boxB)
		{
			return BoxComparison(boxA, boxB, AABB::Axis::z);
		}

	protected:
		std::shared_ptr<RayHittable> m_Left, m_Right;
		AABB m_AABB;

		static std::atomic<size_t> currentDepth, maxDepth;
	};

	// median based BVH (slow but fast to create)
	class BoundingVolumeHierarchiesNode : public BVHBase
	{
	public:
		BoundingVolumeHierarchiesNode(RayHittables& hittables);
		BoundingVolumeHierarchiesNode(std::vector<std::shared_ptr<RayHittable>>& hittables, size_t start, size_t end);
	};

	// Surface Area Heuristic based BVH (fast but slow to create)
	class SurfaceAreaHeuristicNode : public BVHBase
	{
	public:
		SurfaceAreaHeuristicNode(RayHittables& hittables);
		SurfaceAreaHeuristicNode(RayHittables& hittables, size_t numberOfThreads);
		SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<RayHittable>>& hittables, size_t start, size_t end);
		SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<RayHittable>>& hittables, size_t start, size_t end, size_t numberOfThreads);
		
		SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<RayHittable>>& hittables, size_t start, size_t end, AABB& thisAABB, bool isMultithreaded = false);

	private:
		struct BestSplit
		{
			AABB LeftAABB{};
			AABB RightAABB{};
			size_t SplitPosition = 0;
			double Cost = doubleInf;
			AABB::Axis axis = AABB::Axis::none;
		};

	private:
		void SingleThreadedCalculateBVH(std::vector<std::shared_ptr<RayHittable>>& hittables, size_t start, size_t end);
		void MultiThreadedCalculateBVH(std::vector<std::shared_ptr<RayHittable>>& hittables, size_t start, size_t end);
		void CalculateBestSplit(std::vector<std::shared_ptr<RayHittable>>& hittables, size_t start, size_t end, size_t hittablesRange, BestSplit& bestSplit);

		static void multiThreadedCreateNextNodeNoReturn(int id, std::shared_ptr<RayHittable>& childNode, std::vector<std::shared_ptr<RayHittable>>& hittables, size_t start, size_t end, AABB& nextAABB);

		static ctpl::thread_pool s_Threads;
		static std::thread::id s_MainThreadID;
	};

	using BVHNode = BoundingVolumeHierarchiesNode;
	using SAHNode = SurfaceAreaHeuristicNode;
}
