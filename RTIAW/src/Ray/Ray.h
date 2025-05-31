#pragma once
#include "glm/glm.hpp"

#include "Core.h"


namespace RTW
{
	class Ray
	{
		Ray();
		Ray(Point3 origin, Vec3 direction);

		inline const Point3& origin() const { return m_Origin; }
		inline const Vec3& direction() const { return m_Direction; }

		Point3 at(double distance) const;

	private:
		Point3 m_Origin;
		Vec3 m_Direction;
	};
}
