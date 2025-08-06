#pragma once
#define _BaseMaterial
#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"


namespace RTW
{
	class BaseMaterial
	{
	public:
		BaseMaterial() = default;
		virtual ~BaseMaterial() = default;

		virtual bool Scatter([[maybe_unused]] const Ray& ray, [[maybe_unused]] const HitData& data, [[maybe_unused]] Colour& colour, [[maybe_unused]] Ray& scatter) const { return false; }

		virtual Colour EmittedColour([[maybe_unused]] const UV& uv, [[maybe_unused]] const Point& point) const { return Colour(0.0); }
	};
}
