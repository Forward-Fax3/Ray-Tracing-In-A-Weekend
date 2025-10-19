#include <memory>
#include <utility>

#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "RayHittables.h"


namespace RTW
{
	bool RayHittables::IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& data) const
	{
		if (!this->GetBoundingBox().IsHit(ray, rayDistance))
			return false;

		bool hasHit = false;
		Interval tempRayDist(rayDistance);

		for (auto& object : m_Objects)
			if (object->IsRayHit(ray, tempRayDist, data))
			{
				hasHit = true;
				tempRayDist.SetMax(data.distance);
			}

		return hasHit;
	}

	void RayHittables::add(const std::shared_ptr<BaseRayHittable> object)
	{
		// checks if object is actually a RayHittables if so call the RayHittables specialization
		if (const auto rayHittables(std::dynamic_pointer_cast<RayHittables>(object)); rayHittables != nullptr)
		{
			this->add(rayHittables);
			return;
		}

		this->m_Objects.emplace_back(object);
		this->GetBoundingBox().Expand(object->GetBoundingBox());
	}

	void RayHittables::add(const std::shared_ptr<RayHittables> newHittables)
	{
		this->m_Objects.reserve(this->size() + newHittables->size());

		for (const auto& object : newHittables->GetObjects())
			this->m_Objects.emplace_back(object);

		this->GetBoundingBox().Expand(newHittables->GetBoundingBox());
	}

	void RayHittables::addBuffer()
	{
		if (m_Objects.empty())
			return;

		for (auto axis = AABB::Axis::x; axis <= AABB::Axis::z; axis++)
		{
			std::ranges::sort(m_Objects, [axis](auto boxA, auto boxB) -> bool {
				return BoxComparison(boxA, boxB, axis);
			});

			bool hasBeenAdjusted = true;
			
			while (hasBeenAdjusted)
			{
				hasBeenAdjusted = false;
				for (size_t i = 0; i < m_Objects.size() - 1; i++)
					hasBeenAdjusted |= BoxExpand(m_Objects[i], m_Objects[i + 1], axis);
			}
		}

		for (const auto& object : m_Objects)
			GetBoundingBox().Expand(object->GetBoundingBox());
	}

	bool RayHittables::BoxComparison(std::shared_ptr<BaseRayHittable> boxA, std::shared_ptr<BaseRayHittable> boxB, AABB::Axis axis)
	{
		Interval BoxAAxisInterval = boxA->GetBoundingBox().GetAxisInterval(axis);
		Interval BoxBAxisInterval = boxB->GetBoundingBox().GetAxisInterval(axis);

		double BoxAAxisIntervalMin = BoxAAxisInterval.GetMin();
		double BoxBAxisIntervalMin = BoxBAxisInterval.GetMin();

		if (!Interval(-1e-4, 1e-4).Contains(BoxAAxisIntervalMin - BoxBAxisIntervalMin))
			return BoxAAxisIntervalMin < BoxBAxisIntervalMin;

		double BoxAAxisIntervalMax = BoxAAxisInterval.GetMax();
		double BoxBAxisIntervalMax = BoxBAxisInterval.GetMax();

		return BoxAAxisIntervalMax < BoxBAxisIntervalMax;
	}

	bool RayHittables::BoxExpand(std::shared_ptr<BaseRayHittable> boxA, std::shared_ptr<BaseRayHittable> boxB, AABB::Axis axis)
	{
		Interval BoxAAxisInterval = boxA->GetBoundingBox().GetAxisInterval(axis);
		Interval BoxBAxisInterval = boxB->GetBoundingBox().GetAxisInterval(axis);

		double BoxAAxisIntervalMin = BoxAAxisInterval.GetMin();
		double BoxBAxisIntervalMin = BoxBAxisInterval.GetMin();

		if (!Interval(-1e-4, 1e-4).Contains(BoxAAxisIntervalMin - BoxBAxisIntervalMin))
			return false;

		double BoxAAxisIntervalMax = BoxAAxisInterval.GetMax();
		double BoxBAxisIntervalMax = BoxBAxisInterval.GetMax();

		if (!Interval(-1e-4, 1e-4).Contains(BoxAAxisIntervalMax - BoxBAxisIntervalMax))
			return false;

		Interval xInteval = boxA->GetBoundingBox().GetAxisInterval(AABB::Axis::x);
		Interval yInteval = boxA->GetBoundingBox().GetAxisInterval(AABB::Axis::y);
		Interval zInteval = boxA->GetBoundingBox().GetAxisInterval(AABB::Axis::z);
		switch (axis)
		{
			using enum AABB::Axis;
		case x:
			xInteval.Expand(1e-3, 0.0);
			break;
		case y:
			yInteval.Expand(1e-3, 0.0);
			break;
		case z:
			zInteval.Expand(1e-3, 0.0);
			break;
		case none:
		default:
			unreachable();
		}

		boxA->SetBoundingBox(AABB(xInteval, yInteval, zInteval));

		return true;
	}
}
