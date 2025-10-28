#include "SpherePDF.h"


namespace RTW
{
	double SpherePDF::Value(const Vec3&) const
	{
		return 1.0 / (4.0 * glm::pi<double>());
	}
	
	Vec3 SpherePDF::Generate() const
	{
		return RandomUnitVector();
	}
}