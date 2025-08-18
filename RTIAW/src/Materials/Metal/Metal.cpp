#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "Metal.h"
#include "SolidColour.h"


namespace RTW
{
	Metal::Metal(double fuzz)
		: m_Texture(std::make_shared<SolidColour>(Colour(1.0))), m_Fuzz((fuzz < 1.0) ? fuzz : 1.0) {}

	Metal::Metal(const Colour& albedo, double fuzz)
		: m_Texture(std::make_shared<SolidColour>(albedo)), m_Fuzz((fuzz < 1.0) ? fuzz : 1.0) {}

	Metal::Metal(std::shared_ptr<BaseTexture> texture, double fuzz)
		: m_Texture(texture), m_Fuzz((fuzz < 1.0) ? fuzz : 1.0) {}

	std::pair<const bool, const Colour> Metal::Scatter(Ray& ray, const HitData& data) const
	{
		Vec3 refelcted = glm::reflect(ray.direction(), data.normal);
		refelcted = glm::normalize(refelcted) + (m_Fuzz * RandomUnitVector());

		ray = Ray(data.point, refelcted, ray.time());
		return { (glm::dot(ray.direction(), data.normal) > 0.0), m_Texture->GetColour(data.uv, data.point) };
	}
}
