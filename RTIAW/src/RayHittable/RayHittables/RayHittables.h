#pragma once
#include <vector>
#include <memory>

#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"


namespace RTW
{
	class RayHittables : public BaseRayHittable
	{
	public:
		inline RayHittables() = default;
		explicit inline RayHittables(std::shared_ptr<BaseRayHittable>& object) { m_Objects.emplace_back(object); }

		constexpr void reserve(size_t size) { m_Objects.reserve(size); }
		inline void add(const std::shared_ptr<BaseRayHittable>& object) { m_Objects.emplace_back(object); m_AABB.Expand(object->GetBoundingBox()); }

		inline size_t size() const { return m_Objects.size(); }
		inline void clear() { m_Objects.clear(); m_AABB = AABB::empty; }

		inline std::vector<std::shared_ptr<BaseRayHittable>>& GetObjects() { return m_Objects; }

		bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

		const AABB& GetBoundingBox() const override { return m_AABB; }
		inline void SetBoundingBox(const AABB& newAABB) override // newAABB must be bigger than or equal to current AABB in the x, y and z axises otherwise nothing will happen. 
		{
			if (newAABB.IsBigger(this->m_AABB))
				m_AABB = newAABB;
		}

	private:
		std::vector<std::shared_ptr<BaseRayHittable>> m_Objects;
		AABB m_AABB;
	};
}
