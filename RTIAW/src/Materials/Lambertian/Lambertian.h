#pragma once
#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "BaseMaterial.h"


namespace RTW
{
	class Lambertian : public BaseMaterial
	{
	public:
		Lambertian(const Colour& albedo);

		virtual bool Scatter(const Ray& ray, const HitData& data, Colour& colour, Ray& scatter) const override;

	private:
		Colour m_Albedo;
	};
}
