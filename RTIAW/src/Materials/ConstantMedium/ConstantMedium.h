#pragma once
#include "Core.h"
#include "BaseMaterial.h"
#include "BaseTexture.h"

#include <memory>


namespace RTW
{
	class ConstantMedium : public BaseMaterial
	{
	public:
		ConstantMedium(double density, const Colour& albedo);
		ConstantMedium(double density, std::weak_ptr<BaseTexture> texture);

		ScatterReturn Scatter(Ray& ray, const HitData& data, int16_t& bouncesLeft) const override;

		double ScatteringPDF(const Ray& inRay, const HitData& data, const Ray& scatteredRay) const override
		{
			return 1.0 / (4.0 * glm::pi<double>());
		}

	private:
		double m_NegitiveInvertedDensity;
		std::shared_ptr<BaseTexture> m_Texture;
	};
}
