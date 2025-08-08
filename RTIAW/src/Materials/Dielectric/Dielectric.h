#pragma once
#include "Core.h"
#include "Ray.h"
#include "BaseMaterial.h"


namespace RTW
{
	class Dielectric : public BaseMaterial
	{
	public:
		explicit Dielectric(double refactionIndex);

		bool Scatter(const Ray& ray, const HitData& data, Colour& colour, Ray& scatter) const override;

	private:
		static Vec3 refract(const Vec3& uv, const Vec3& normal, double etaiOverEtat);

		static double reflectance(double cos, double ri);

	private:
		double m_RefractionIndex;
	};
}
