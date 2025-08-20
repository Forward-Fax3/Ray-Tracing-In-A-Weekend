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
			: _total(glm::dvec4(colour, *reinterpret_cast<double*>(&isScattered))) {}

		Colour attenuation;
		struct
		{
			glm::vec<3, double, glm::packed_highp> _;
			bool bounced;
		};

		glm::dvec4 _total;
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
