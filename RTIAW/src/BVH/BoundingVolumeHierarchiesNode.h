#pragma once
#include <memory>
#include <vector>

#include "Core.h"
#include "AxisAliagnedBoundingBoxes.h"
#include "RayHittable.h"
#include "Hittables.h"


namespace RTW
{
	class BoundingVolumeHierarchiesNode : public RayHittable
	{
	public:
		BoundingVolumeHierarchiesNode(RayHittables& hittables);
		BoundingVolumeHierarchiesNode(std::vector<std::shared_ptr<RayHittable>>& hittables, size_t start, size_t end);

		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

		virtual const AABB& GetBoundingBox() const override { return m_AABB; }

	private:
		static bool BoxComparison(const std::shared_ptr<RayHittable> boxA, const std::shared_ptr<RayHittable> boxB, AABB::Axis axis);
		static inline bool CompareBoxXAxis(const std::shared_ptr<RayHittable> boxA, const std::shared_ptr<RayHittable> boxB)
			{ return BoxComparison(boxA, boxB, AABB::Axis::x); }
		static inline bool CompareBoxYAxis(const std::shared_ptr<RayHittable> boxA, const std::shared_ptr<RayHittable> boxB)
			{ return BoxComparison(boxA, boxB, AABB::Axis::y); }
		static inline bool CompareBoxZAxis(const std::shared_ptr<RayHittable> boxA, const std::shared_ptr<RayHittable> boxB)
			{ return BoxComparison(boxA, boxB, AABB::Axis::z); }

	private:
		std::shared_ptr<RayHittable> m_Left, m_Right;
		AABB m_AABB;

		static size_t numberOfBBs;
	};

	using BVHNode = BoundingVolumeHierarchiesNode;
}
