#include <iostream>
#include <memory>
#include <chrono>

#include "glm/glm.hpp"

#include "Core.h"
#include "Sphere.h"
#include "Hittables.h"
#include "Camera.h"


int main()
{
	auto startTime = std::chrono::high_resolution_clock().now();

	double aspectRatio = 16.0 / 9.0;
	int16_t imageWidth = 1920;
	int16_t samplesPerPixel = 16;
	int16_t maxBounceDepth = 16;

	RTW::RayHittables worldHitables;
	{
		RTW::Point tempPoint(0.0, 0.0, -1.0);
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 0.5));
		tempPoint = { 0.0, -100.5, -1.0 };
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 100.0));
	}

	RTW::Camera camera(aspectRatio, imageWidth, samplesPerPixel, maxBounceDepth);

	camera.Render(worldHitables);

	auto finishTime = std::chrono::high_resolution_clock().now();

	std::clog << "\rDone.                 \nTime Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds";
	std::cin.get();
}
