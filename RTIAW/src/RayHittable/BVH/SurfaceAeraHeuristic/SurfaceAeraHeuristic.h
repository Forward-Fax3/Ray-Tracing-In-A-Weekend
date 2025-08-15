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

		SurfaceAreaHeuristicNode(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end, const AABB& thisAABB, bool isMultithreaded = false);

	private:
		struct BestSplit
		{
			BestSplit() = default;

			AABB LeftAABB;
			AABB RightAABB;
			size_t SplitPosition = 0;
			double Cost = doubleInf;
			AABB::Axis axis = AABB::Axis::none;
		};

	private:
		void SingleThreadedCalculateBVH(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end);
		void MultiThreadedCalculateBVH(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end);
		void CalculateBestSplit(std::vector<std::shared_ptr<BaseRayHittable>>& hittables, size_t start, size_t end, size_t hittablesRange, BestSplit& bestSplit)const ;
	};

	using SAHNode = SurfaceAreaHeuristicNode;
}