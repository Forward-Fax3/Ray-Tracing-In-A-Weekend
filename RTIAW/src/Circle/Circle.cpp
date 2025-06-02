#include "glm/glm.hpp"

#include "Core.h"
#include "Circle.h"
#include "Ray.h"


namespace RTW
{
	Circle::Circle()
		: m_Center(0.0), m_Radius(0.0) {}

	Circle::Circle(const Point& center, double radius)
		: m_Center(center), m_Radius(radius) {}

	bool Circle::IsHit(const Ray& ray)
	{
		Point oc = m_Center - ray.origin();
		double a = glm::dot(ray.direction(), ray.direction());
		double b = -2.0 * glm::dot(ray.direction(), oc);
		double c = glm::dot(oc, oc) - glm::pow(m_Radius, 2);

		double discriminant = glm::pow(b, 2) - 4 * a * c;

		m_RayHitDiastance = (-b - glm::sqrt(discriminant)) / (2.0 * a);

		return glm::round(discriminant) >= 0.0;
	}
}
