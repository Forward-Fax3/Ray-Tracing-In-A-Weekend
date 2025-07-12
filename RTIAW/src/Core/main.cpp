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

#include "BaseTexture.h"
#include "SolidColour.h"
#include "CheckeredTexture.h"


int main()
{
	auto startTime = std::chrono::high_resolution_clock().now();

	double aspectRatio = 16.0 / 9.0;
	int16_t imageWidth = 1920;

	double FOV = 50.0;
//	RTW::Vec3 lookFrom(0.0, 0.0, 1.0);
//	RTW::Vec3 LookAt(0.0, 0.0, 0.0);
//	RTW::Vec3 VUp(0.0, 1.0, 0.0);

	RTW::Vec3 lookFrom(13.0, 2.0, 3.0);
	RTW::Vec3 LookAt(0.0);
	RTW::Vec3 VUp(0.0, 1.0, 0.0);

#ifdef _DEBUG // do not change these values for debug will take for ever otherwise
	int16_t samplesPerPixel = 4;
	int16_t maxBounceDepth = 4;
#else
	int16_t samplesPerPixel = 256;
	int16_t maxBounceDepth = 256; // ridiculously high bounces doesn't seem to have much of an affect on performance
#endif

	RTW::Vec3 gamma(2.4);

	double defocusAngle = 0.0;
	double focusDistance = 0.0;

	[[maybe_unused]] int16_t numberOfThreads = std::thread::hardware_concurrency();

	RTW::RayHittables worldHitables;
	{
		std::shared_ptr<RTW::BaseTexture> Texture = std::make_shared<RTW::CheckeredTexture>(RTW::Colour(0.2, 0.3, 0.1), RTW::Colour(0.9), 0.32);
		std::shared_ptr<RTW::BaseMaterial> material = std::make_shared<RTW::Lambertian>(Texture);
		worldHitables.add(std::make_shared<RTW::Sphere>(RTW::Point(0, -1000, 0), 1000, material));

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
						material = std::make_shared<RTW::Lambertian>(albedo);
//						RTW::Point center2 = center + RTW::Vec3(0, glm::linearRand(0.0, 0.5), 0);
//						worldHitables.add(std::make_shared<RTW::MovingSphere>(center, center2, 0.2, material));
						worldHitables.add(std::make_shared<RTW::Sphere>(center, 0.2, material));
					}
					else if (chooseMat < 0.95) // metal
					{
						RTW::Colour albedo = glm::linearRand(RTW::Vec3(0.5), RTW::Vec3(1.0));
						double fuzz = glm::linearRand(0.0, 0.5);
						material = std::make_shared<RTW::Metal>(albedo, fuzz);
						worldHitables.add(std::make_shared<RTW::Sphere>(center, 0.2, material));
					}
					else // glass
					{
						double refactionIndex = glm::linearRand(0.0, 2.0);
						material = std::make_shared<RTW::Dielectric>(refactionIndex);
						worldHitables.add(std::make_shared<RTW::Sphere>(center, 0.2, material));
					}
				}
			}

		material = std::make_shared<RTW::Dielectric>(1.5);
		worldHitables.add(std::make_shared<RTW::Sphere>(RTW::Point(0, 1, 0), 1.0, material));

		material = std::make_shared<RTW::Lambertian>(RTW::Colour(0.4, 0.2, 0.1));
		worldHitables.add(std::make_shared<RTW::Sphere>(RTW::Point(-4, 1, 0), 1.0, material));

		material = std::make_shared<RTW::Metal>(RTW::Colour(0.7, 0.6, 0.5), 0.0);
		worldHitables.add(std::make_shared<RTW::Sphere>(RTW::Point(4, 1, 0), 1.0, material));
	}

	RTW::Camera camera(aspectRatio, imageWidth, FOV, defocusAngle, focusDistance, lookFrom, LookAt, VUp, gamma, samplesPerPixel, maxBounceDepth);

	std::shared_ptr<RTW::RayHittable> BVHWorldHitables = std::make_shared<RTW::BVHNode>(worldHitables);
	worldHitables.clear();
	worldHitables.add(BVHWorldHitables);

//	camera.Render(worldHitables);
	camera.RenderMultiThreaded(numberOfThreads, worldHitables);

	auto finishTime = std::chrono::high_resolution_clock().now();

	std::clog << "\rTime Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds";
	std::cin.get();
}
