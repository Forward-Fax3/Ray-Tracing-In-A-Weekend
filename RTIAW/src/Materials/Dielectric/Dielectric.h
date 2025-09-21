#pragma once
#include "Core.h"
#include "Ray.h"
#include "BaseMaterial.h"
#include "BaseTexture.h"

#include <memory>


namespace RTW
{
	class Dielectric : public BaseMaterial
	{
	public:
		explicit Dielectric(double refactionIndex);
		explicit Dielectric(double refactionIndex, const Colour& colour);
		explicit Dielectric(double refactionIndex, std::shared_ptr<BaseTexture> texture);

		ScatterReturn Scatter(Ray& ray, const HitData& data, int16_t& bouncesLeft) const override;

	private:
		static Vec3 refract(const Vec3& uv, const Vec3& normal, double etaiOverEtat);

		static double reflectance(double cos, double ri);

	private:
		double m_RefractionIndex;
		std::shared_ptr<BaseTexture> m_Texture;
	};
}
