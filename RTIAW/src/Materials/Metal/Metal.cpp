#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "Metal.h"


namespace RTW
{
	Metal::Metal(const Colour& albedo)
		: m_Albido(albedo) {}

	bool Metal::Scatter(const Ray& ray, const HitData& data, Colour& colour, Ray& scatter) const
	{
		Vec3 refelcted = glm::reflect(ray.direction(), data.normal);
		scatter = Ray(data.point, refelcted);
		colour = m_Albido;
		return true;
	}
}
