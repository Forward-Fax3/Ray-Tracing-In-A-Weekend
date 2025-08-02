#include <memory>

#include "glm/glm.hpp"

#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "RayHittable.h"


namespace RTW
{
	std::shared_ptr<RayHittable> RayNoHit::s_NoHit = std::make_shared<RayNoHit>();
}
