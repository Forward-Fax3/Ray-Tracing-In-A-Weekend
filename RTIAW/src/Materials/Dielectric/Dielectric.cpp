#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtx/norm.hpp"

#include "Core.h"
#include "Ray.h"
#include "Dielectric.h"
#include "SolidColour.h"


namespace RTW
{
	Dielectric::Dielectric(double refactionIndex)
		: m_RefractionIndex(refactionIndex), m_Texture(std::make_shared<SolidColour>(Colour(1.0))) { }

	Dielectric::Dielectric(double refactionIndex, const Colour& colour)
		: m_RefractionIndex(refactionIndex), m_Texture(std::make_shared<SolidColour>(colour)) {}

	Dielectric::Dielectric(double refactionIndex, std::shared_ptr<BaseTexture> texture)
		: m_RefractionIndex(refactionIndex), m_Texture(texture)	{}

	ScatterReturn Dielectric::Scatter(Ray& ray, const HitData& data) const
	{
		double ri = data.isFrontFace ? (1.0 / m_RefractionIndex) : m_RefractionIndex;

		Vec3 normalDirection = glm::normalize(ray.direction());
		double cosTheta = glm::min(glm::dot(-normalDirection, data.normal), 1.0);
		double sinTheta = glm::sqrt(1.0 - cosTheta * cosTheta);

		Vec3 newDirection = (ri * sinTheta > 1.0 || reflectance(cosTheta, ri) > glm::linearRand(0.0, 1.0)) ?
			glm::reflect(normalDirection, data.normal) :
			glm::refract(normalDirection, data.normal, ri);

		ray = Ray(data.point, newDirection, ray.time());
		return { m_Texture->GetColour(data.uv, data.point), true };
	}

	Vec3 Dielectric::refract(const Vec3& uv, const Vec3& normal, double etaOverEtaPrime)
	{
		double cosTheta = glm::min(glm::dot(-uv, normal), 1.0);
		Vec3 rOutPerp = etaOverEtaPrime * (uv + cosTheta * normal);
		Vec3 rOutParrallel = -glm::sqrt(glm::abs(1.0 - glm::length2(rOutPerp))) * normal;
		return rOutPerp + rOutParrallel;
	}

	double Dielectric::reflectance(double cos, double ri)
	{
		double r0 = (1.0 - ri) / (1 + ri);
		r0 *= r0;
		return r0 + (1 - r0) * glm::pow(1 - cos, 5);
	}
}