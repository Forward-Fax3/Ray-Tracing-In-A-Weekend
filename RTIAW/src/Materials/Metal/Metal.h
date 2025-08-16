#pragma once
#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "BaseMaterial.h"


namespace RTW
{
	class Metal : public BaseMaterial
	{
	public:
		Metal(const Colour& albedo, double fuzz);

		std::pair<const bool, const Colour> Scatter(Ray& ray, const HitData& data) const override;

	private:
		Colour m_Albido;
		double m_Fuzz;
	};
}
