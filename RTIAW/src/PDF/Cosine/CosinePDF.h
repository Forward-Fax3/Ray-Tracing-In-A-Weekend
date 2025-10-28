#pragma once
#include "PDF.h"
#include "ONB.h"


namespace RTW
{
	class CosinePDF : public PDF
	{
	public:
		explicit CosinePDF(const Vec3& w);

		double Value(const Vec3& direction) const override;

		Vec3 Generate() const override;

	private:
		ONB m_ONB;
	};
}
