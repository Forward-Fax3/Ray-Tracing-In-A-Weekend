#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "Metal.h"


namespace RTW
{
	Metal::Metal(const Colour& albedo, double fuzz)
		: m_Albido(albedo) ,m_Fuzz(fuzz < 1 ? fuzz : 1.0) {}

	std::pair<const bool, const Colour> Metal::Scatter(Ray& ray, const HitData& data) const
	{
		Vec3 refelcted = glm::reflect(ray.direction(), data.normal);
		refelcted = glm::normalize(refelcted) + (m_Fuzz * RandomUnitVector());

		ray = Ray(data.point, refelcted, ray.time());
		return { (glm::dot(ray.direction(), data.normal) > 0.0), m_Albido };
	}
}
