#include "ConstantMedium.h"

#include "SolidColour.h"

#include "Camera.h"

#include "glm/gtc/random.hpp"


namespace RTW
{
	ConstantMedium::ConstantMedium(double density, const Colour& albedo)
		: m_NegitiveInvertedDensity(-1.0 / density), m_Texture(std::make_shared<SolidColour>(albedo)) { }

	ConstantMedium::ConstantMedium(double density, std::weak_ptr<BaseTexture> texture)
		: m_NegitiveInvertedDensity(-1.0 / density), m_Texture(texture) {}

	RTW::ScatterReturn ConstantMedium::Scatter(Ray& ray, const HitData& data, int16_t& bouncesLeft) const
	{
		const auto& objects = Camera::GetObjects();
		Colour tempColour(1.0);
		bool bounced = true;
		bool missed = false;
		Ray tempRay = ray;
		HitData tempData{};
		double distance = 0.0;

		for (; bouncesLeft != 0; bouncesLeft--)
		{
			if (!objects->IsRayHit(tempRay, Interval(0.001, doubleInf), tempData))
			{
				missed = true;
				bouncesLeft = 0;
				break;
			}

			distance += tempData.distance;
			
			if (tempData.material == this)
				break;

			Colour emittedColour = tempData.material->EmittedColour(tempData.uv, tempData.point);

			ScatterReturn scatteredData = tempData.material->Scatter(tempRay, tempData, bouncesLeft);

			if (!scatteredData.bounced)
			{
				bounced = false;
				tempColour *= emittedColour;
				break;
			}

			tempColour *= scatteredData.attenuation;
			tempColour += emittedColour;
			tempData = HitData();
		}

		if (missed)
			return { { 0.0, 0.0, 0.0 }, false };

		double rayLength = glm::length(tempData.point - ray.origin());
		double distanceInsideMedium = distance * rayLength;
		double hitDistance = m_NegitiveInvertedDensity * glm::log(glm::linearRand(-1.0, 1.0));

		if (hitDistance <= distanceInsideMedium)
		{
			ray = Ray(tempRay.at(tempData.distance), RandomUnitVector(), tempRay.time());
			tempColour *= m_Texture->GetColour(data.uv, ray.origin());
		}
		else
			ray = Ray(tempRay.at(tempData.distance), tempRay.direction(), tempRay.time());

		return { tempColour, bounced };
	}
}
