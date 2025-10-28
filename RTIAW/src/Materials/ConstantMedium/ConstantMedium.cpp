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
		Colour tempColour(1.0);
		bool bounced = true;
		const auto& objects = Camera::GetObjects();
		Ray tempRay(data.point, ray.direction(), ray.time());
		HitData tempData{};
		double distance = 0.0;
		int16_t bouncesLeftTemp = bouncesLeft;

		while (bouncesLeftTemp != 0 && bounced)
		{
			if (!objects->IsRayHit(tempRay, Interval(0.001, doubleInf), tempData))
			{
				if (distance == 0.0)
					distance = data.distance;

				if (
					double hitDistance = m_NegitiveInvertedDensity * glm::log(glm::linearRand(-1.0, 1.0));
					hitDistance <= distance
					)
				{
//					ray = Ray(tempRay.at(tempData.distance), RandomUnitVector(), tempRay.time());
//					ray = Ray(ray.at(tempData.distance), RandomUnitVector(), tempRay.time());
					ray = Ray(tempRay.origin(), RandomUnitVector(), tempRay.time());
					tempColour *= m_Texture->GetColour(data.uv, ray.origin());
					bouncesLeft = bouncesLeftTemp;
				}
				else
					ray = tempRay;

				return { tempColour, 1.0 / (4.0 * glm::pi<double>()), true};
			}

			distance += tempData.distance;
			
			if (tempData.material == this)
				break;

			Colour emittedColour = tempData.material->EmittedColour(tempData, tempData.point);

			ScatterReturn scatteredData;
			double scatteringPDF;
			{
				Ray originalTempRay = tempRay;
				scatteredData = tempData.material->Scatter(tempRay, tempData, bouncesLeftTemp);
				scatteringPDF = tempData.material->ScatteringPDF(originalTempRay, tempData, tempRay);
			}
			double PDFValue = scatteringPDF;

			tempColour *= scatteredData.attenuation;
			tempColour *= scatteringPDF;
			tempColour /= PDFValue;
			tempColour += emittedColour;

			if (!scatteredData.bounced)
				bounced = false;
			else
				tempData = HitData();
		}

		bouncesLeft = bouncesLeftTemp;

//		double rayLength = glm::length(tempData.point - ray.origin());
//		double rayLength = glm::length(ray.direction());
//		double rayLength = glm::length(tempRay.direction());
		double distanceInsideMedium = distance;
		double hitDistance = m_NegitiveInvertedDensity * glm::log(glm::linearRand(-1.0, 1.0));

		if (hitDistance <= distanceInsideMedium)
		{
			ray = Ray(tempRay.at(tempData.distance), RandomUnitVector(), tempRay.time());
//			ray = Ray(ray.at(tempData.distance), RandomUnitVector(), tempRay.time());
//			ray = Ray(tempData.point, RandomUnitVector(), tempRay.time());
			tempColour *= m_Texture->GetColour(data.uv, ray.origin());
		}
		else
			ray = Ray(tempData.point, tempRay.direction(), tempRay.time());

		return { tempColour, 1.0 / (4.0 * glm::pi<double>()), bounced };
	}
}
