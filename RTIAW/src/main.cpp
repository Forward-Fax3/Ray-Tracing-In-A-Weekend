#include <iostream>
#include <memory>
#include <chrono>

#include "glm/glm.hpp"

#include "Core.h"
#include "WriteColour.h"
#include "Interval.h"
#include "Ray.h"
#include "Sphere.h"
#include "Hittables.h"


static RTW::Colour rayColour(const RTW::Ray& ray, const RTW::RayHittable& object)
{
	RTW::HitData data;
	if (object.IsRayHit(ray, RTW::Interval(0, RTW::DoubleInf), data))
		return 0.5 * (data.normal + 1.0);

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

	RTW::RayHittables worldHitables;
	{
		RTW::Point tempPoint(0.0, 0.0, -1.0);
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 0.5));
		tempPoint = { 0.0, -100.5, -1 };
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 100.0));
	}

	double focalLength = 2.2;
	double viewportHeight = 1.2;
	double viewportWidth = viewportHeight * (static_cast<double>(imageWidth) / static_cast<double>(imageHeight));
	RTW::Point camaraPosition(0.0, 0.0, 3.0);

	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

	RTW::Point viewportU(viewportWidth, 0.0, 0.0);
	RTW::Point viewportV(0.0, -viewportHeight, 0.0);

	RTW::Point pixelDeltaU = viewportU / static_cast<double>(imageWidth);
	RTW::Point pixelDeltaV = viewportV / static_cast<double>(imageHeight);

	RTW::Point viewportUpperLeft = camaraPosition - RTW::Point(0.0, 0.0, focalLength) - 0.5 * (viewportU + viewportV);

	RTW::Point pixal100Location = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

	for (size_t j = 0; j < imageHeight; ++j)
	{
		std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;

		for (size_t i = 0; i < imageWidth; i++)
		{
			RTW::Point pixelCenter = pixal100Location + (static_cast<double>(i) * pixelDeltaU) + (static_cast<double>(j) * pixelDeltaV);
			RTW::Vec3 rayDirection = pixelCenter - camaraPosition;
			RTW::Ray ray(camaraPosition, rayDirection);

			RTW::Colour pixelColor = rayColour(ray, worldHitables);
			RTW::WriteColour(std::cout, pixelColor);
		}
	}

	auto finishTime = std::chrono::high_resolution_clock().now();

	std::clog << "\rDone.                 \nTime Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds";
	std::cin.get();
}
