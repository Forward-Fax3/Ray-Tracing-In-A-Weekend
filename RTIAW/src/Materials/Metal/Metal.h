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
		Metal(const Colour& albedo);

		virtual bool Scatter(const Ray& ray, const HitData& data, Colour& colour, Ray& scatter) const override;

	private:
		Colour m_Albido;
	};
}
