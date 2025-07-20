#include <iostream>
#include <chrono>
#include <thread>

#include "Core.h"
#include "Hittables.h"
#include "BoundingVolumeHierarchiesNode.h"
#include "Camera.h"
#include "Scenes.h"


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
	int16_t samplesPerPixel = 2048;
	int16_t maxBounceDepth = 2048; // ridiculously high bounces doesn't seem to have much of an affect on performance
#endif

	RTW::Vec3 gamma(2.4);

	double defocusAngle = 0.0;
	double focusDistance = 0.0;

	[[maybe_unused]] uint32_t numberOfThreads = std::thread::hardware_concurrency();

	RTW::RayHittables worldHitables;

	// Scene Selection
	RTW::Scenes scene = RTW::Scenes::Earth;
	RTW::SceneSelect(scene, worldHitables);

	RTW::Camera camera(aspectRatio, imageWidth, FOV, defocusAngle, focusDistance, lookFrom, LookAt, VUp, gamma, samplesPerPixel, maxBounceDepth);

	if (worldHitables.size() > 1)
	{
		std::shared_ptr<RTW::RayHittable> BVHWorldHitables = std::make_shared<RTW::BVHNode>(worldHitables);
		worldHitables.clear();
		worldHitables.add(BVHWorldHitables);
	}

//	camera.Render(worldHitables);
	camera.RenderMultiThreaded(numberOfThreads, worldHitables);

	auto finishTime = std::chrono::high_resolution_clock().now();

	std::clog << "\rTime Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds";
	std::cin.get();
}
