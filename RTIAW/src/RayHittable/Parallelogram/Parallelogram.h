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

		double PDFValue(const Point& origin, const Vec3& direction) const override;

		Vec3 Random(const Point& origin, const Ray&) const override;

	private:
		void CreateAABB();
		UV CalculateUV(const Point& p) const;

	private:
		double m_D;
		std::shared_ptr<BaseMaterial> m_Material;
		double m_Area;
		UVvec3 m_UV;
		Point m_Q;
		Vec3 m_Normal;
		Vec3 m_W;
	};

	std::shared_ptr<BaseRayHittable> CreateBox(const Point& pointA, const Point& pointB, std::shared_ptr<BaseMaterial> material);
	std::shared_ptr<BaseRayHittable> CreateBox(const Point& pointA, const Point& pointB, std::shared_ptr<BaseMaterial> material, const Vec3& degrees);
}
	