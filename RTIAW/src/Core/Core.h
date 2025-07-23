#pragma once
#include <limits>

#include "glm/glm.hpp"

#pragma warning (disable: 4201)


namespace RTW
{
	using Vec3 = glm::dvec3;
//	using Vec3 = glm::vec3;
	using Colour = Vec3;
	using Point = Vec3;
	using UV = glm::dvec2;
//	using UV = glm::vec2;
	static constexpr double doubleInf = std::numeric_limits<double>::infinity();

	Vec3 RandomOnHemisphere(const Vec3& normal);

	Vec3 RandomUnitVector();

	Vec3 RandomOnUnitDisk();
}
