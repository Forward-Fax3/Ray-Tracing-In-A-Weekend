#pragma once
#include "PDF.h"
#include "RayHittable.h"


namespace RTW
{
	class HittablesPDF : public PDF
	{
	public:
		HittablesPDF(const Vec3& origin, const std::shared_ptr<BaseRayHittable>& hittable);

		double Value(const Vec3& direction) const override;
		Vec3 Generate() const override;

	private:
		const std::shared_ptr<BaseRayHittable>& m_Objects;
		Vec3 m_Origin;
	};
}
