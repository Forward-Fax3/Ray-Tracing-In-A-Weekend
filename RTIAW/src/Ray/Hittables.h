#pragma once
#include <vector>
#include <memory>

#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"


namespace RTW
{
	class RayHittables : public RayHittable
	{
	public:
		inline RayHittables() = default;
		inline RayHittables(std::shared_ptr<RayHittable>& object) { m_Objects.emplace_back(object); }

		inline void add(const std::shared_ptr<RayHittable>& object) { m_Objects.emplace_back(object); m_AABB = { m_AABB, object->GetBoundingBox() }; }

		inline size_t size() const { return m_Objects.size(); }
		inline void clear() { m_Objects.clear(); m_AABB = AABB::empty; }

		inline std::vector<std::shared_ptr<RayHittable>>& GetObjects() { return m_Objects; }

		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

		virtual const AABB& GetBoundingBox() const override { return m_AABB; }

	private:
		std::vector<std::shared_ptr<RayHittable>> m_Objects;
		AABB m_AABB;
	};
}
