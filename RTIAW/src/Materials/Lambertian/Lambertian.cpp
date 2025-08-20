#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "RayHittable.h"
#include "BaseMaterial.h"
#include "BaseTexture.h"
#include "Lambertian.h"
#include "SolidColour.h"


namespace RTW
{
	inline static bool operator<(const Vec3& lhs, const double rhs)
	{
		Vec3 abslhs(glm::abs(lhs));
		return (abslhs.x < rhs) && (abslhs.y < rhs) && (abslhs.z < rhs);
	}

	Lambertian::Lambertian(const Colour& albedo)
		: m_Texture(std::make_shared<SolidColour>(albedo)) {}

	Lambertian::Lambertian(std::shared_ptr<BaseTexture> texture)
		: m_Texture(texture) {}

	ScatterReturn Lambertian::Scatter(Ray& ray, const HitData& data) const
	{
		Vec3 scatterDirection = data.normal + RandomUnitVector();
		double minValue = 1e-8;

		scatterDirection = scatterDirection < minValue ? data.normal : scatterDirection;

		ray = Ray(data.point, scatterDirection, ray.time());
		return { m_Texture->GetColour(data.uv, data.point), true };
	}
}
