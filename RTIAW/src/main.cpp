#include <iostream>
#include <chrono>

#include "glm/glm.hpp"

#include "Core.h"
#include "WriteColour.h"
#include "Ray.h"
#include "Circle.h"


RTW::Colour rayColour(const RTW::Ray& ray)
{
	static RTW::Point circleCentre(0.0, 0.0, -1.0);
	static RTW::Circle circle(circleCentre, 0.5);

	if (circle.IsHit(ray))
	{
		RTW::Vec3 normal = glm::normalize(ray.at(circle.getReyHitDiasatnace()) - RTW::Vec3(0.0, 0.0, -1));
		if (glm::dot(normal, normal) > 0.0)
			return 0.5 * (RTW::Colour(normal + 1.0));
	}

	RTW::Vec3 normalizedDirection = glm::normalize(ray.direction());
	double a = 0.5 * (normalizedDirection.y + 1.0);
	return RTW::Colour(1.0 - a) + a * RTW::Colour(0.5, 0.7, 1.0);
}

int main()
{
	auto startTime = std::chrono::high_resolution_clock().now();

	double aspectRatio = 16.0 / 9.0;
	int16_t imageWidth = 1920;

	int16_t imageHeight = static_cast<int16_t>(imageWidth / aspectRatio);
	imageHeight = (imageHeight < 1) ? 1 : imageHeight;

	double focalLength = 2.2;
	double viewportHeight = 1.2;
	double viewportWidth = viewportHeight * (double(imageWidth) / double(imageHeight));
	RTW::Point camaraPosition = RTW::Point(0.0, 0.0, 3.0);

	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

	RTW::Point viewportU(viewportWidth, 0.0, 0.0);
	RTW::Point viewportV(0.0, -viewportHeight, 0.0);

	RTW::Point pixelDeltaU = viewportU / double(imageWidth);
	RTW::Point pixelDeltaV = viewportV / double(imageHeight);

	RTW::Point viewportUpperLeft = camaraPosition - RTW::Point(0.0, 0.0, focalLength) - 0.5 * (viewportU + viewportV);

	RTW::Point pixal100Location = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

	for (int j = 0; j < imageHeight; ++j)
	{
		std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;

		for (int i = 0; i < imageWidth; i++)
		{
			RTW::Point pixelCenter = pixal100Location + (double(i) * pixelDeltaU) + (double(j) * pixelDeltaV);
			RTW::Vec3 rayDirection = pixelCenter - camaraPosition;
			RTW::Ray ray(camaraPosition, rayDirection);

			RTW::Colour pixelColor = rayColour(ray);
			RTW::WriteColour(std::cout, pixelColor);
		}
	}

	auto finishTime = std::chrono::high_resolution_clock().now();

	std::clog << "\rDone.                 \nTime Took: " << std::chrono::duration_cast<std::chrono::duration<float>>(finishTime - startTime).count() << " seconds";
	std::cin.get();
}
