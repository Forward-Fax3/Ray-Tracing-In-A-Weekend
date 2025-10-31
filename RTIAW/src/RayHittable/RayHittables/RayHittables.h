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
		RayHittables() = default;
		explicit RTW_FORCE_INLINE RayHittables(const std::shared_ptr<BaseRayHittable> object) { this->add(object); }

		constexpr void reserve(size_t size) { m_Objects.reserve(size); }
		void add(const std::shared_ptr<BaseRayHittable> object);
		void add(const std::shared_ptr<RayHittables> object);

		double PDFValue(const Point& origin, const Vec3& direction) const override;

		Vec3 Random(const Point& origin) const override;

		// add buffers to AABBs so that comparisons that need to be axis aligned don't end up having the same value
		void addBuffer();

		RTW_FORCE_INLINE size_t size() const { return m_Objects.size(); }
		RTW_FORCE_INLINE void clear() { m_Objects.clear(); SetBoundingBox(AABB::empty); }

		RTW_FORCE_INLINE std::vector<std::shared_ptr<BaseRayHittable>>& GetObjects() { return m_Objects; }

		bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

	private: // Functions
		static bool BoxComparison(std::shared_ptr<BaseRayHittable> boxA, std::shared_ptr<BaseRayHittable> boxB, AABB::Axis axis);
		static bool BoxExpand(std::shared_ptr<BaseRayHittable> boxA, std::shared_ptr<BaseRayHittable> boxB, AABB::Axis axis);

	private: // Data
		std::vector<std::shared_ptr<BaseRayHittable>> m_Objects;
	};
}
