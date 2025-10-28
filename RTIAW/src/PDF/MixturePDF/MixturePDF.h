#pragma once
#include "PDF.h"


namespace RTW
{
	class MixturePDF : public PDF
	{
	public:
		MixturePDF(const PDF& p0, const PDF& p1);

		double Value(const Vec3& direction) const override;

		Vec3 Generate() const override;

	private:
		const PDF& m_PDF0;
		const PDF& m_PDF1;
	};
}
