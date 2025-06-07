#pragma once
#include "glm/glm.hpp"

#include "Core.h"


namespace RTW
{
	class Ray
	{
	public:
		Ray();
		Ray(Point origin, Vec3 direction);
		Ray(Point origin, Vec3 direction, double time);

		inline const Point& origin() const { return m_Origin; }
		inline const Vec3& direction() const { return m_Direction; }

		Point at(double distance) const;

		double time() const { return m_Time; }

	private:
		Point m_Origin;
		Vec3 m_Direction;
		double m_Time;
	};
}
