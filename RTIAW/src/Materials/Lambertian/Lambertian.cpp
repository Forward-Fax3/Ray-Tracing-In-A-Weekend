#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "RayHittable.h"
#include "BaseMaterial.h"
#include "Lambertian.h"


namespace RTW
{
	inline static bool operator<(const double lhs, const Vec3& rhs)
	{
		return abs(lhs) < rhs.x && abs(lhs) < rhs.y && abs(lhs) < rhs.z;
	}

	Lambertian::Lambertian(const Colour& albedo)
		: m_Albedo(albedo) {}

	bool Lambertian::Scatter([[maybe_unused]] const Ray& ray, const HitData& data, Colour& colour, Ray& scatter) const
	{
		Vec3 scatterDirection = data.normal + RandomUnitVector();
		double minValue = 1e-8;

		scatterDirection = minValue < scatterDirection ? data.normal : scatterDirection;

		scatter = Ray(data.point, scatterDirection);
		colour = m_Albedo;
		return true;
	}
}
