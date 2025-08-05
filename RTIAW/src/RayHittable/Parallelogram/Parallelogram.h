#pragma once
#include "Core.h"
#include "RayHittable.h"
#include "BaseMaterial.h"

#include <memory>


namespace RTW
{
	class Parallelogram : public BaseRayHittable
	{
	public:
		Parallelogram(const Point& Q, const UVvec3& uv, const std::shared_ptr<BaseMaterial>& material);
		~Parallelogram() override = default;

		virtual bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

		virtual const AABB& GetBoundingBox() const override { return m_AABB; }

	private:
		void CreateAABB();
		UV CalculateUV(const Point& p) const;

	private:
		double m_D;
		std::shared_ptr<BaseMaterial> m_Material;
		AABB m_AABB;
		UVvec3 m_UV;
		Point m_Q;
		Vec3 m_Normal;
		Vec3 m_W;
	};
}
