#include "CosinePDF.h"


namespace RTW
{
	CosinePDF::CosinePDF(const Vec3& w)
		: m_ONB(w) {}

	double CosinePDF::Value(const Vec3& direction) const
	{
		double cosine = glm::dot(glm::normalize(direction), m_ONB.W());
		return (cosine <= 0.0) ? 0.0 : cosine * glm::one_over_pi<double>();
	}

	Vec3 CosinePDF::Generate() const
	{
		return m_ONB.Transform(RandomCosineDirection());
	}
}