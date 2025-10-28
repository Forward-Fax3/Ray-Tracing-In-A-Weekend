#include "MixturePDF.h"


namespace RTW
{
	MixturePDF::MixturePDF(const PDF& p0, const PDF& p1)
		: m_PDF0(p0), m_PDF1(p1) {}

	double MixturePDF::Value(const Vec3& direction) const
	{
		return 0.25 * m_PDF0.Value(direction) + 0.75 * m_PDF1.Value(direction);
	}

	RTW::Vec3 MixturePDF::Generate() const
	{
		return (glm::linearRand(0.0, 1.0) < 0.25) ? m_PDF0.Generate() : m_PDF1.Generate();
	}
}