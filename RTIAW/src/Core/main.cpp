#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

#include "glm/glm.hpp"

#include "Core.h"
#include "Sphere.h"
#include "Hittables.h"
#include "Camera.h"

#include "BaseMaterial.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"


int main()
{
	auto startTime = std::chrono::high_resolution_clock().now();

	double aspectRatio = 16.0 / 9.0;
	int16_t imageWidth = 1920;

	double FOV = 20.0;
	RTW::Vec3 lookFrom(-2.0, 2.0, 1.0);
	RTW::Vec3 LookAt(0.0, 0.0, -1.0);
	RTW::Vec3 VUp(0.0, 1.0, 0.0);

#ifdef _DEBUG // do not change these values for debug will take for ever otherwise
	int16_t samplesPerPixel = 4;
	int16_t maxBounceDepth = 4;
#else
	int16_t samplesPerPixel = 64;
	int16_t maxBounceDepth = 8;
#endif
	[[maybe_unused]] int16_t numberOfThreads = std::thread::hardware_concurrency();

	RTW::RayHittables worldHitables;
	{
		RTW::Point tempPoint(0.0, -100.5, -1.0);
		std::shared_ptr<RTW::BaseMaterial> material(std::make_shared<RTW::Lambertian>(RTW::Colour(0.8, 0.8, 0.0)));
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 100.0, material));

		tempPoint = { 0.0, 0.0, -1.2 };
		material = std::make_shared<RTW::Lambertian>(RTW::Colour(0.1, 0.2, 0.5));
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 0.5, material));

		tempPoint = { -1.0, 0.0, -1.0 };
		material = std::make_shared<RTW::Metal>(RTW::Colour(0.8), 0.0);
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 0.5, material));

		tempPoint = { 1.0, 0.0, -1.0 };
		material = std::make_shared<RTW::Dielectric>(1.5);
		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 0.5, material)); 
	}

	RTW::Camera camera(aspectRatio, imageWidth, FOV, lookFrom, LookAt, VUp, samplesPerPixel, maxBounceDepth);

//	camera.Render(worldHitables);
	camera.RenderMultiThreaded(numberOfThreads, worldHitables);

	auto finishTime = std::chrono::high_resolution_clock().now();

	std::clog << "\rTime Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds";
	std::cin.get();
}
