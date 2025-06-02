#pragma once
#include <limits>

#include "glm/glm.hpp"


namespace RTW
{
	using Vec3 = glm::dvec3;
	using Colour = Vec3;
	using Point = Vec3;
	static double DoubleInf = std::numeric_limits<double>::infinity();
}
