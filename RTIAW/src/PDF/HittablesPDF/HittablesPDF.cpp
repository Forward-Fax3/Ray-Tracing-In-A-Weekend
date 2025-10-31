#include "HittablesPDF.h"


namespace RTW
{
	HittablesPDF::HittablesPDF(const Vec3& origin, const std::shared_ptr<BaseRayHittable>& objects)
		: m_Objects(objects), m_Origin(origin) {}

	double HittablesPDF::Value(const Vec3& direction) const
	{
		return m_Objects->PDFValue(m_Origin, direction);
	}

	Vec3 HittablesPDF::Generate() const
	{
		return m_Objects->Random(m_Origin);
	}
}