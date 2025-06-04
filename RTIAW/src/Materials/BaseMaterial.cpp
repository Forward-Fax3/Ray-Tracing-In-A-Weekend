#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "BaseMaterial.h"


namespace RTW
{
	bool BaseMaterial::Scatter([[maybe_unused]] const Ray& ray, [[maybe_unused]] const HitData& data, [[maybe_unused]] Colour& colour, [[maybe_unused]] Ray& scatter) const
	{
		return false;
	}
}
