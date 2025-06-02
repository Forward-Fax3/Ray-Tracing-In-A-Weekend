#pragma once
#include "glm/glm.hpp"
#include "Ray.h"

#include "Core.h"


namespace RTW
{
	class Circle
	{
	public:
		Circle();
		Circle(const Point& center, double radius);

		bool IsHit(const Ray& ray);

		inline double getReyHitDiasatnace() { return m_RayHitDiastance; }

	private:
		Point m_Center;
		double m_Radius;
		double m_RayHitDiastance;
	};
}
