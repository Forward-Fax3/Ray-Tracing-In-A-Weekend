#include <memory>
#include <vector>

#include "Core.h"
#include "RayHittable.h"
#include "RayHittables.h"
#include "BaseBVH.h"


namespace RTW
{
	// Surface Area Heuristic based BVH (fast but slow to create)
	class SurfaceAreaHeuristicNode final : public BVHBase
	{
	public:
		explicit SurfaceAreaHeuristicNode(std::shared_ptr<RayHittables> hittables);
		SurfaceAreaHeuristicNode(std::shared_ptr<RayHittables> hittables, size_t numberOfThreads);
		SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end);
		SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end, size_t numberOfThreads);

		bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

		SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end, const AABB& thisAABB, bool isMultithreaded = false);

		const AABB& GetBoundingBox() const override { return m_AABB; }
		inline void SetBoundingBox(const AABB& newAABB) override // newAABB must be bigger than or equal to current AABB in the x, y and z axises otherwise nothing will happen. 
		{
			if (newAABB.IsBigger(this->m_AABB))
				m_AABB = newAABB;
		}

	private:
		struct BestSplit
		{
			inline BestSplit()
				: axis(AABB::Axis::none), Cost(doubleInf), SplitPosition(0) {}

			~BestSplit() {}; // need to use "{}" instead of "= default" due to the function being deleted by the compiler

#ifndef _DEBUG
			union
			{
				AABB LeftAABB;

				struct
				{
					Interval padding1[3];
					AABB::Axis padding2;
					
					AABB::Axis axis;
					double Cost;
				};
			};

			union
			{
				AABB RightAABB;

				struct
				{
					Interval padding3[3];
					AABB::Axis padding4;

					size_t SplitPosition;
				};
			};

#else // ifdef _DEBUG
			AABB LeftAABB;
			AABB RightAABB;
			double Cost;
			size_t SplitPosition;
			AABB::Axis axis;
#endif
		};

	private:
		void SingleThreadedCalculateBVH(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end);
		void MultiThreadedCalculateBVH(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end);
		void CalculateBestSplit(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end, size_t hittablesRange, std::shared_ptr<BestSplit> bestSplit) const;

	private:
		std::shared_ptr<BaseRayHittable> m_Left;
		std::shared_ptr<BaseRayHittable> m_Right;
		AABB m_AABB;
	};

	using SAHNode = SurfaceAreaHeuristicNode;
}
