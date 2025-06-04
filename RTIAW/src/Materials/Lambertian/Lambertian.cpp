#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "RayHittable.h"
#include "BaseMaterial.h"
#include "Lambertian.h"


namespace RTW
{
	inline static bool operator<(const Vec3& lhs, const double rhs)
	{
		Vec3 abslhs = glm::abs(lhs);
		return abslhs.x < rhs && abslhs.y < rhs && abslhs.z < rhs;
	}

	Lambertian::Lambertian(const Colour& albedo)
		: m_Albedo(albedo) {}

	bool Lambertian::Scatter([[maybe_unused]] const Ray& ray, const HitData& data, Colour& colour, Ray& scatter) const
	{
		Vec3 scatterDirection = data.normal + RandomUnitVector();
		double minValue = 1e-8;

		scatterDirection = scatterDirection < minValue ? data.normal : scatterDirection;

		scatter = Ray(data.point, scatterDirection);
		colour = m_Albedo;
		return true;
	}
}
