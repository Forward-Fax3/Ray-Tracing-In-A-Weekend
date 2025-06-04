#include <iostream>
#include <memory>
#include <chrono>

#include "glm/glm.hpp"

#include "Core.h"
#include "Sphere.h"
#include "Hittables.h"
#include "Camera.h"

#include "BaseMaterial.h"
#include "Lambertian.h"
#include "Metal.h"


int main()
{
	auto startTime = std::chrono::high_resolution_clock().now();

	double aspectRatio = 16.0 / 9.0;
	int16_t imageWidth = 1920;
	int16_t samplesPerPixel = 512;
	int16_t maxBounceDepth = 256;

	RTW::RayHittables worldHitables;
	{
		RTW::Point tempPoint(0.0, -100.5, -1.0);
		std::shared_ptr<RTW::BaseMaterial> material(std::make_shared<RTW::Lambertian>(RTW::Colour(0.8, 0.8, 0.0)));
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 100.0, material));

		tempPoint = { 0.0, 0.0, -1.0 };
		material = std::make_shared<RTW::Lambertian>(RTW::Colour(0.1, 0.2, 0.5));
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 0.5, material));

		tempPoint = { -1.0, 0.0, -1.0 };
		material = std::make_shared<RTW::Metal>(RTW::Colour(0.8));
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 0.5, material));

		tempPoint = { 1.0, 0.0, -1.0 };
		material = std::make_shared<RTW::Metal>(RTW::Colour(0.8, 0.6, 0.2));
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 0.5, material));
	}

	RTW::Camera camera(aspectRatio, imageWidth, samplesPerPixel, maxBounceDepth);

	camera.Render(worldHitables);

	auto finishTime = std::chrono::high_resolution_clock().now();

	std::clog << "\rDone.                 \nTime Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds";
	std::cin.get();
}
