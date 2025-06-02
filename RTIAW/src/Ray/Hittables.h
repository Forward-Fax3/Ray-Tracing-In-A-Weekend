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

		inline void add(const std::shared_ptr<RayHittable> object) { m_Objects.emplace_back(object); }

		inline void clear() { m_Objects.clear(); }

		virtual bool IsRayHit(const Ray& ray, double rayDistanceMin, double rayDistanceMax, HitData& hitData) const override;

	private:
		std::vector<std::shared_ptr<RayHittable>> m_Objects;
	};
}
