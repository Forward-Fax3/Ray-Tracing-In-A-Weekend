#include <limits>

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtx/norm.hpp"

#include "Core.h"
#include "Interval.h"


namespace RTW
{
	Vec3 RandomOnHemisphere(const Vec3& normal)
	{
		Vec3 onUnitSphere = RandomUnitVector();
		return (glm::dot(onUnitSphere, normal) > 0.0) ? onUnitSphere : -onUnitSphere;
	}

	Vec3 RandomUnitVector()
	{
		Vec3 direction(0.0);
		double lengthSqared = 0.0;
		const Interval minMax(1e-160, 1);

		do {
			direction = glm::linearRand(Vec3(-1), Vec3(1));
			lengthSqared = glm::length2(direction);
		} while (!minMax.Surrounds(lengthSqared));

		return direction / lengthSqared;
	}

	RTW::Vec3 RandomOnUnitDisk()
	{
		Point point(0.0);
		do {
			point = glm::linearRand(Vec3(-1.0, -1.0, 0.0), Vec3(1.0, 1.0, 0.0));
		} while (glm::length2(point) >= 1);

		return point;
	}

	::ctpl::thread_pool g_Threads;
}
