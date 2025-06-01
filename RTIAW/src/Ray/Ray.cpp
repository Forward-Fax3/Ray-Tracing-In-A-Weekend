#include "glm/glm.hpp"

#include "Core.h"
#include "Ray.h"

namespace RTW
{
	Ray::Ray()
		: m_Origin(0.0), m_Direction(0.0) {}

	Ray::Ray(Point origin, Vec3 direction)
		: m_Origin(origin), m_Direction(direction) {}

	RTW::Point Ray::at(double distance) const
	{
		return m_Origin + distance * m_Direction;
	}
}