#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "Metal.h"


namespace RTW
{
	Metal::Metal(const Colour& albedo, double fuzz)
		: m_Albido(albedo) ,m_Fuzz(fuzz < 1 ? fuzz : 1.0) {}

	bool Metal::Scatter(const Ray& ray, const HitData& data, Colour& colour, Ray& scatter) const
	{
		Vec3 refelcted = glm::reflect(ray.direction(), data.normal);
		refelcted = glm::normalize(refelcted) + (m_Fuzz * RandomUnitVector());
		scatter = Ray(data.point, refelcted);
		colour = m_Albido;
		return (glm::dot(scatter.direction(), data.normal) > 0.0);
	}
}
