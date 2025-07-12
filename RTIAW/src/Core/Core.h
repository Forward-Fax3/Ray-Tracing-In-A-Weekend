#pragma once
#include <limits>

#include "glm/glm.hpp"


namespace RTW
{
	using Vec3 = glm::dvec3;
	using Colour = Vec3;
	using Point = Vec3;
	using UV = glm::dvec2;
	static constexpr double DoubleInf = std::numeric_limits<double>::infinity();

	Vec3 RandomOnHemisphere(const Vec3& normal);

	Vec3 RandomUnitVector();

	Vec3 RandomOnUnitDisk();
}
