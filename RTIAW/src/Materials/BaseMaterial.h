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

	union ScatterReturn
	{
		explicit(false) ScatterReturn(const Colour& colour, bool isScattered)
			: _(colour), bounced(isScattered) {}

		Colour attenuation;
		struct
		{
			glm::highp_dvec3 _;
			bool bounced;
		};
	};

	class BaseMaterial
	{
	public:
		BaseMaterial() = default;
		virtual ~BaseMaterial() = default;

		virtual ScatterReturn Scatter(Ray&, const HitData&) const { return { {0.0, 0.0, 0.0 }, false }; }

		virtual Colour EmittedColour(const UV&, const Point&) const { return Colour(0.0); }
	};
}
