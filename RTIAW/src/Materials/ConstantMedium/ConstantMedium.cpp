#include "ConstantMedium.h"

#include "SolidColour.h"

#include "Camera.h"

#include "glm/gtc/random.hpp"

#include "HittablesPDF.h"
#include "SpherePDF.h"
#include "MixturePDF.h"


namespace RTW
{
	ConstantMedium::ConstantMedium(double density, const Colour& albedo)
		: m_NegitiveInvertedDensity(-1.0 / density), m_Texture(std::make_shared<SolidColour>(albedo)) { }

	ConstantMedium::ConstantMedium(double density, std::weak_ptr<BaseTexture> texture)
		: m_NegitiveInvertedDensity(-1.0 / density), m_Texture(texture) {}

	ScatterReturn ConstantMedium::Scatter(Ray& ray, const HitData& data, int16_t& bouncesLeft) const
	{
		Colour tempColour(1.0);
		bool bounced = true;
		const auto& objects = Camera::GetObjects();
		const auto& lights = Camera::GetLights();
		Ray tempRay(data.point + 0.001 * ray.direction(), ray.direction(), ray.time());
		HitData tempData{};
		bool hasBounced = false;

		while (bouncesLeft != 0 && bounced)
		{
			if (!objects->IsRayHit(tempRay, Interval(0.001, doubleInf), tempData))
			{
				if (hasBounced)
				{
					bounced = true;
					ray = tempRay;
					return { { 1.0, 1.0, 1.0 }, std::make_unique<SpherePDF>(), bounced, false };
				}

				auto rayLength = glm::length(ray.direction());
				auto distanceToHit = data.distance * rayLength;
				auto hitDistance = m_NegitiveInvertedDensity * glm::log(glm::linearRand(0.0, 1.0));

				if (hitDistance <= distanceToHit)
				{
					Ray originalRay = ray;
					tempRay = Ray(ray.at(glm::linearRand(0.0, data.distance)), RandomUnitVector(), tempRay.time());

					HittablesPDF hittablesPDF(data.point, lights);
					SpherePDF spherePDF;
					MixturePDF mixedPDF(hittablesPDF, spherePDF);

					double PDFValue = mixedPDF.Value(ray.direction());
					double scatteringPDF = this->ScatteringPDF(originalRay, tempData, tempRay);

					tempColour *= (scatteringPDF / PDFValue);
					tempColour *= m_Texture->GetColour(data.uv, tempRay.origin());

					bouncesLeft--;
				}

				ray = tempRay;

				return { tempColour, std::make_unique<SpherePDF>(), true, false };
			}

			hasBounced = true;

			Colour emittedColour = tempData.material->EmittedColour(tempData);

			auto rayLength = glm::length(tempRay.direction());
			auto distanceToHit = tempData.distance * rayLength;
			auto hitDistance = m_NegitiveInvertedDensity * glm::log(glm::linearRand(0.0, 1.0));

			if (hitDistance <= distanceToHit)
			{
				Ray originalTempRay = tempRay;
				tempRay = Ray(tempRay.at(glm::linearRand(0.0, tempData.distance)), RandomUnitVector(), tempRay.time());

				HittablesPDF hittablesPDF(tempData.point, lights);
				SpherePDF spherePDF;
				MixturePDF mixedPDF(hittablesPDF, spherePDF);

				double PDFValue = mixedPDF.Value(tempRay.direction());
				double scatteringPDF = this->ScatteringPDF(originalTempRay, tempData, tempRay);

				tempColour *= (scatteringPDF / PDFValue);
				tempColour *= m_Texture->GetColour(data.uv, tempRay.origin());
				tempColour += emittedColour;

				bouncesLeft--;

				if (tempData.material == this)
					break;
			}
			else if (tempData.material != this)
			{
				ScatterReturn scatteredData;
				double scatteringPDF;
				{
					Ray originalTempRay = tempRay;
					scatteredData = tempData.material->Scatter(tempRay, tempData, bouncesLeft);
					if (!scatteredData.bounced)
					{
						bounced = false;
						break;
					}

					if (!scatteredData.skipPDF)
					{
						HittablesPDF hittablesPDF(data.point, lights);
						MixturePDF mixedPDF(hittablesPDF, *scatteredData.pdf);

						ray = Ray(data.point, mixedPDF.Generate(), ray.time());
						double PDFValue = mixedPDF.Value(ray.direction());

						scatteringPDF = tempData.material->ScatteringPDF(originalTempRay, tempData, tempRay);

						tempColour *= scatteringPDF / PDFValue;
					}
				}

				tempColour *= scatteredData.attenuation;
				tempColour += emittedColour;

				if (!scatteredData.bounced)
					bounced = false;
				else
					tempData = HitData();
			}
			else
				break;
		}

		ray = Ray(tempRay.origin()+ 0.001 * tempRay.direction(), tempRay.direction(), ray.time());

		return { tempColour, std::make_unique<SpherePDF>(), bounced, false };
	}
}
 