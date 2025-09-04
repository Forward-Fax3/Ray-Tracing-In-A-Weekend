#pragma once
#include "Core.h"
#include "RayHittable.h"
#include "BaseMaterial.h"

#include <memory>


namespace RTW
{
	class Parallelogram final : public BaseRayHittable
	{
	public:
		Parallelogram(const Point& Q, const UVvec3& uv, std::shared_ptr<BaseMaterial> material);
		~Parallelogram() override = default;

		bool IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const override;

		const AABB& GetBoundingBox() const override { return m_AABB; }
		inline void SetBoundingBox(const AABB& newAABB) override // newAABB must be bigger than or equal to current AABB in the x, y and z axises otherwise nothing will happen. 
		{
			if (newAABB.IsBigger(this->m_AABB))
				m_AABB = newAABB;
		}

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

	std::shared_ptr<BaseRayHittable> CreateBox(const Point& pointA, const Point& pointB, std::shared_ptr<BaseMaterial> material, const Vec3& rotation = Vec3(0.0));
}
	