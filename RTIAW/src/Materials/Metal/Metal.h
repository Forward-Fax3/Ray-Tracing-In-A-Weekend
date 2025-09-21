#pragma once
#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "BaseMaterial.h"
#include "BaseTexture.h"

#include <memory>


namespace RTW
{
	class Metal : public BaseMaterial
	{
	public:
		Metal(double fuzz);
		Metal(const Colour& albedo, double fuzz);
		Metal(std::shared_ptr<BaseTexture> texture, double fuzz);

		ScatterReturn Scatter(Ray& ray, const HitData& data, int16_t& bouncesLeft) const override;

	private:
		std::shared_ptr<BaseTexture> m_Texture;
		double m_Fuzz;
	};
}
