#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"

#include "Core.h"
#include "Sphere.h"
#include "Ray.h"


namespace RTW
{
	Sphere::Sphere(const Point& center, double radius)
		: m_Radius(glm::max(0.0, radius)), m_Center(center) {}

	bool Sphere::IsRayHit(const Ray& ray, double rayDistanceMin, double rayDistanceMax, HitData& hitData) const
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
		if (root <= rayDistanceMin || root >= rayDistanceMax)
		{
			root = (h + sqrtDiscriminant) / a;
			if (root <= rayDistanceMin || root >= rayDistanceMax)
				return false;
		}

		hitData.distance = root;
		hitData.point = ray.at(hitData.distance);
		Vec3 normal = (hitData.point - m_Center) / m_Radius;

		setFaceNormal(ray, normal, hitData);

		return true;
	}
}
