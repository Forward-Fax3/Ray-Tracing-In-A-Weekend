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

	struct ScatterReturn
	{
		explicit(false) ScatterReturn()
			: attenuation(0.0), pdfValue(0.0), bounced(false) { }

		explicit(false) ScatterReturn(const Colour& colour, double pdf, bool isScattered)
			: attenuation(colour), pdfValue(pdf), bounced(isScattered) { }

		Colour attenuation;
		double pdfValue;
		bool bounced;
	};

	class BaseMaterial
	{
	public:
		BaseMaterial() = default;
		virtual ~BaseMaterial() = default;

		virtual ScatterReturn Scatter(Ray&, const HitData&, int16_t&) const { return { {0.0, 0.0, 0.0 }, 1.0, false }; }

		virtual Colour EmittedColour(const UV&, const Point&) const { return Colour(0.0); }

		virtual double ScatteringPDF(const Ray&, const HitData&, const Ray&) const { return 0.0; }
	};
}
