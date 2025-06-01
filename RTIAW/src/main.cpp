#include <iostream>

#include "glm/glm.hpp"

#include "Core.h"
#include "WriteColour.h"
#include "Ray.h"


RTW::Colour rayColour(const RTW::Ray& ray)
{
	RTW::Vec3 normalizedDirection = glm::normalize(ray.direction());
	double a = 0.5 * (normalizedDirection.y + 1.0);
	return RTW::Colour(1.0 - a) + a * RTW::Colour(0.5, 0.7, 1.0);
}

bool isCircleHit(const RTW::Point3& center, double radius, const RTW::Ray& ray)
{
	return false;
}

int main()
{
	double aspectRatio = 16.0 / 9.0;
	int16_t imageWidth = 1920;

	int16_t imageHeight = static_cast<int16_t>(imageWidth / aspectRatio);
	imageHeight = (imageHeight < 1) ? 1 : imageHeight;

	double focalLength = 1.0;
	double viewportHeight = 2.0;
	double viewportWidth = viewportHeight * (double(imageWidth) / double(imageHeight));
	RTW::Point3 camaraPosition = RTW::Point3(0.0);

	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

	RTW::Point3 viewportU(viewportWidth, 0.0, 0.0);
	RTW::Point3 viewportV(0.0, -viewportHeight, 0.0);

	RTW::Point3 pixelDeltaU = viewportU / double(imageWidth);
	RTW::Point3 pixelDeltaV = viewportV / double(imageHeight);

	RTW::Point3 viewportUpperLeft = camaraPosition - RTW::Point3(0.0, 0.0, focalLength) - 0.5 * (viewportU + viewportV);

	RTW::Point3 pixal100Location = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

	for (int j = 0; j < imageHeight; ++j)
	{
		std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;

		for (int i = 0; i < imageWidth; i++)
		{
			RTW::Point3 pixelCenter = pixal100Location + (double(i) * pixelDeltaU) + (double(j) * pixelDeltaV);
			RTW::Vec3 rayDirection = pixelCenter - camaraPosition;
			RTW::Ray ray(camaraPosition, rayDirection);

			RTW::Colour pixelColor = rayColour(ray);
			RTW::WriteColour(std::cout, pixelColor);
		}
	}

	std::clog << "\rDone.                 \n";
}
