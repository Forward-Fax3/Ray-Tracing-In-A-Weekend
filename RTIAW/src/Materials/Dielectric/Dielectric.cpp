#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtx/norm.hpp"

#include "Core.h"
#include "Ray.h"
#include "Dielectric.h"


namespace RTW
{
	Dielectric::Dielectric(double refactionIndex)
		: m_RefractionIndex(refactionIndex) {}

	bool Dielectric::Scatter(const Ray& ray, const HitData& data, Colour& colour, Ray& scatter) const
	{
		colour = Colour(1.0);
		double ri = data.isFrontFace ? (1.0 / m_RefractionIndex) : m_RefractionIndex;

		Vec3 normalDirection = glm::normalize(ray.direction());
		double cosTheta = glm::min(dot(-normalDirection, data.normal), 1.0);
		double sinTheta = glm::sqrt(1.0 - cosTheta * cosTheta);

		Vec3 refracted = ri * sinTheta > 1.0 || reflectance(cosTheta, ri) > glm::linearRand(0.0, 1.0) ?
			reflect(normalDirection, data.normal) :
			refract(normalDirection, data.normal, ri);

		scatter = Ray(data.point, refracted);
		return true;
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