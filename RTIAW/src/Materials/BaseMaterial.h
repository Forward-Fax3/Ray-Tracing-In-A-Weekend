#pragma once
#define _BaseMaterial
#include "Core.h"
#include "Ray.h"
#include "PDF.h"

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
			: attenuation(0.0), pdf(nullptr), bounced(false), skipPDF(true) { }

		explicit(false) ScatterReturn(const Colour& colour, std::unique_ptr<PDF> Newpdf, bool isScattered, bool skipPDF)
			: attenuation(colour), pdf(std::move(Newpdf)), bounced(isScattered), skipPDF(skipPDF) {}

		Colour attenuation;
		std::unique_ptr<PDF> pdf;
		bool bounced;
		bool skipPDF;
	};

	class BaseMaterial
	{
	public:
		BaseMaterial() = default;
		virtual ~BaseMaterial() = default;

		virtual ScatterReturn Scatter(Ray&, const HitData&, int16_t&) const { return {}; }

		virtual Colour EmittedColour(const HitData&) const { return Colour(0.0); }

		virtual double ScatteringPDF(const Ray&, const HitData&, const Ray&) const { return 0.0; }
	};
}
