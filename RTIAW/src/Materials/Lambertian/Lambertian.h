#pragma once
#include "Core.h"
#include "Ray.h"
#include "RayHittable.h"
#include "BaseMaterial.h"
#include "BaseTexture.h"

#include <memory>


namespace RTW
{
	class Lambertian : public BaseMaterial
	{
	public:
		Lambertian(const Colour& albedo);
		Lambertian(std::shared_ptr<BaseTexture> texture);

		virtual bool Scatter(const Ray& ray, const HitData& data, Colour& colour, Ray& scatter) const override;

	private:
		std::shared_ptr<BaseTexture> m_Texture;
	};
}
