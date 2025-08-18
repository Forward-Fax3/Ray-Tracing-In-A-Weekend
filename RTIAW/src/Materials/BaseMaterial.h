#pragma once
#define _BaseMaterial
#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"

#include <utility>

#ifndef _HITDATA
#include "RayHittable.h"
#endif


namespace RTW
{
	struct HitData;

	class BaseMaterial
	{
	public:
		BaseMaterial() = default;
		virtual ~BaseMaterial() = default;

		virtual std::pair<const bool, const Colour> Scatter([[maybe_unused]] Ray& ray, [[maybe_unused]] const HitData& data) const
			{ return { false, { 0.0, 0.0, 0.0 } }; }

		virtual Colour EmittedColour([[maybe_unused]] const UV& uv, [[maybe_unused]] const Point& point) const { return Colour(0.0); }
	};
}
