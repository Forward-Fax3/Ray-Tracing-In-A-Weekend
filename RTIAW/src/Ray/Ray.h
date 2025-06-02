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

		inline const Point& origin() const { return m_Origin; }
		inline const Vec3& direction() const { return m_Direction; }

		Point at(double distance) const;

	private:
		Point m_Origin;
		Vec3 m_Direction;
	};
}
