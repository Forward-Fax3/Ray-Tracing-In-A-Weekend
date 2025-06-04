#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtx/norm.hpp"

#include "Core.h"
#include "Interval.h"
#include "Sphere.h"
#include "Ray.h"


namespace RTW
{
	Sphere::Sphere(const Point& center, double radius)
		: m_Radius(glm::max(0.0, radius)), m_Center(center) {}

	bool Sphere::IsRayHit(const Ray& ray, const Interval& rayDistance, HitData& hitData) const
	{
		Vec3 oc = m_Center - ray.origin();
		double a = glm::length2(ray.direction());
		double h = glm::dot(ray.direction(), oc);
		double c = glm::length2(oc) - m_Radius * m_Radius;

		double discriminant = h * h - a * c;
		if (discriminant <= 0.0)
			return false;

		double sqrtDiscriminant = glm::sqrt(discriminant);

		double root = (h - sqrtDiscriminant) / a;
		if (!rayDistance.Contains(root))
		{
			root = (h + sqrtDiscriminant) / a;
			if (!rayDistance.Contains(root))
				return false;
		}

		hitData.distance = root;
		hitData.point = ray.at(hitData.distance);
		Vec3 normal = (hitData.point - m_Center) / m_Radius;

		setFaceNormal(ray, normal, hitData);

		return true;
	}

	Vec3 Sphere::RandomOnHemisphere(const Vec3& normal)
	{
		Vec3 onUnitSphere = RandomUnitVector();
		return glm::dot(onUnitSphere, normal) > 0.0 ? onUnitSphere : -onUnitSphere;
	}

	Vec3 Sphere::RandomUnitVector()
	{
		Vec3 direction(0.0);
		double lengthSqared = 0.0;
		const Interval MaxMin(1e-160, 1);

		do {
			direction = glm::linearRand(Vec3(-1), Vec3(1));
			lengthSqared = glm::length2(direction);
		} while (!MaxMin.Surrounds(lengthSqared));

		return direction / lengthSqared;
	}
}
