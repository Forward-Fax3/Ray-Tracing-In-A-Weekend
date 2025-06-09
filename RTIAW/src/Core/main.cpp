#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"

#include "Core.h"
#include "Sphere.h"
#include "Hittables.h"
#include "BoundingVolumeHierarchiesNode.h"
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
//	RTW::Vec3 lookFrom(-2.0, 2.0, 1.0);
//	RTW::Vec3 LookAt(0.0, 0.0, -1.0);
//	RTW::Vec3 VUp(0.0, 1.0, 0.0);

	RTW::Vec3 lookFrom(13.0, 2.0, 3.0);
	RTW::Vec3 LookAt(0.0);
	RTW::Vec3 VUp(0.0, 1.0, 0.0);

#ifdef _DEBUG // do not change these values for debug will take for ever otherwise
	int16_t samplesPerPixel = 4;
	int16_t maxBounceDepth = 4;
#else
	int16_t samplesPerPixel = 32;
	int16_t maxBounceDepth = 8;
#endif

	RTW::Vec3 gamma(2.4);

	double defocusAngle = 0.6;
	double focusDistance = 10.0;

	[[maybe_unused]] int16_t numberOfThreads = std::thread::hardware_concurrency();

	RTW::RayHittables worldHitables;
//	{
//		RTW::Point tempPoint(0.0, -100.5, -1.0);
//		std::shared_ptr<RTW::BaseMaterial> material(std::make_shared<RTW::Lambertian>(RTW::Colour(0.8, 0.8, 0.0)));
//		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 100.0, material));
//
//		tempPoint = { 0.0, 0.0, -1.2 };
//		material = std::make_shared<RTW::Lambertian>(RTW::Colour(0.1, 0.2, 0.5));
//		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 0.5, material));
//
//		tempPoint = { -1.0, 0.0, -1.0 };
//		material = std::make_shared<RTW::Metal>(RTW::Colour(0.8), 0.0);
//		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 0.5, material));
//
//		tempPoint = { 1.0, 0.0, -1.0 };
//		material = std::make_shared<RTW::Dielectric>(1.5);
//		worldHitables.add(std::make_shared<RTW::Sphere>(tempPoint, 0.5, material)); 
//	}

	{
		std::shared_ptr<RTW::BaseMaterial> groundMaterial = std::make_shared<RTW::Lambertian>(RTW::Colour(0.5));
		worldHitables.add(std::make_shared<RTW::Sphere>(RTW::Point(0, -1000, 0), 1000, groundMaterial));

		for (int a = -11; a < 11; a++)
			for (int b = -11; b < 11; b++)
			{
				double chooseMat = glm::linearRand(0.0, 1.0);
				RTW::Point center(RTW::Point(a, 0.2, b) + glm::linearRand(RTW::Point(0.0), RTW::Point(0.9, 0.0, 0.9)));

				if ((center - RTW::Point(4, 0.2, 0)).length() > 0.9)
				{
					if (chooseMat < 0.8) // diffuse
					{
						RTW::Colour albedo = glm::linearRand(RTW::Vec3(0.0), RTW::Vec3(1.0)) * glm::linearRand(RTW::Vec3(0.0), RTW::Vec3(1.0));
						std::shared_ptr<RTW::BaseMaterial> sphereMaterial = std::make_shared<RTW::Lambertian>(albedo);
						RTW::Point center2 = center + RTW::Vec3(0, glm::linearRand(0.0, 0.5), 0);
						worldHitables.add(std::make_shared<RTW::MovingSphere>(center, center2, 0.2, sphereMaterial));
					}
					else if (chooseMat < 0.95) // metal
					{
						RTW::Colour albedo = glm::linearRand(RTW::Vec3(0.5), RTW::Vec3(1.0));
						double fuzz = glm::linearRand(0.0, 0.5);
						std::shared_ptr<RTW::BaseMaterial> sphereMaterial = std::make_shared<RTW::Metal>(albedo, fuzz);
						worldHitables.add(std::make_shared<RTW::Sphere>(center, 0.2, sphereMaterial));
					}
					else // glass
					{
						double refactionIndex = glm::linearRand(0.0, 2.0);
						std::shared_ptr<RTW::BaseMaterial> sphereMaterial = std::make_shared<RTW::Dielectric>(refactionIndex);
						worldHitables.add(std::make_shared<RTW::Sphere>(center, 0.2, sphereMaterial));
					}
				}
			}

		std::shared_ptr<RTW::BaseMaterial> material1 = std::make_shared<RTW::Dielectric>(1.5);
		worldHitables.add(std::make_shared<RTW::Sphere>(RTW::Point(0, 1, 0), 1.0, material1));

		std::shared_ptr<RTW::BaseMaterial> material2 = std::make_shared<RTW::Lambertian>(RTW::Colour(0.4, 0.2, 0.1));
		worldHitables.add(std::make_shared<RTW::Sphere>(RTW::Point(-4, 1, 0), 1.0, material2));

		std::shared_ptr<RTW::BaseMaterial> material3 = std::make_shared<RTW::Metal>(RTW::Colour(0.7, 0.6, 0.5), 0.0);
		worldHitables.add(std::make_shared<RTW::Sphere>(RTW::Point(4, 1, 0), 1.0, material3));
	}

	RTW::Camera camera(aspectRatio, imageWidth, FOV, defocusAngle, focusDistance, lookFrom, LookAt, VUp, gamma, samplesPerPixel, maxBounceDepth);

	RTW::BVHNode BVHWorldHitables(worldHitables);

//	camera.Render(BVHWorldHitables);
	camera.RenderMultiThreaded(numberOfThreads, BVHWorldHitables);

	auto finishTime = std::chrono::high_resolution_clock().now();

	std::clog << "\rTime Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds";
	std::cin.get();
}
