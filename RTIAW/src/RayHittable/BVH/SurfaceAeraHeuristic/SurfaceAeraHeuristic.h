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
				: axis(AABB::Axis::none), Cost(doubleInf) {}

			inline ~BestSplit()
			{
				LeftAABB.~AxisAliagnedBoundingBoxes();
				RightAABB.~AxisAliagnedBoundingBoxes();
			}

			union
			{
				AABB LeftAABB;

				struct
				{
					Interval padding0[3];
					AABB::Axis padding1;
					
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

					size_t SplitPosition = 0;
				};
			};
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