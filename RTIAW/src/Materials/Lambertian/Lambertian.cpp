#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "RayHittable.h"
#include "BaseMaterial.h"
#include "BaseTexture.h"
#include "Lambertian.h"
#include "SolidColour.h"
#include "ONB.h"


namespace RTW
{
//	inline static bool operator<(const Vec3& lhs, const double rhs)
//	{
//		Vec3 abslhs(glm::abs(lhs));
//		return (abslhs.x < rhs) && (abslhs.y < rhs) && (abslhs.z < rhs);
//	}

	Lambertian::Lambertian(const Colour& albedo)
		: m_Texture(std::make_shared<SolidColour>(albedo)) {}

	Lambertian::Lambertian(std::shared_ptr<BaseTexture> texture)
		: m_Texture(texture) {}

	ScatterReturn Lambertian::Scatter(Ray& ray, const HitData& data, int16_t& bouncesLeft) const
	{
		bouncesLeft--;

		ONB onb(data.normal);
		Vec3 scatterDirection = onb.Transform(RandomCosineDirection());

		ray = Ray(data.point, glm::normalize(scatterDirection), ray.time());
		return {
			m_Texture->GetColour(data.uv, data.point),
			glm::dot(onb.W(), ray.direction() * glm::one_over_pi<double>()),
			true
			};
	}

	double Lambertian::ScatteringPDF(const Ray&, const HitData&, const Ray&) const
	{
		return 1.0 / glm::tau<double>();
	}
}
