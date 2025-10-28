#pragma once
#include "PDF.h"


namespace RTW
{
	class SpherePDF : public PDF
	{
	public:
		SpherePDF() = default;

		double Value(const Vec3&) const override;

		Vec3 Generate() const override;
	};
}
