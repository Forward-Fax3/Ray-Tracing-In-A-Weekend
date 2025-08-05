#include <iostream>
#include <chrono>
#include <thread>

#include "Core.h"
#include "RayHittables.h"
#include "BoundingVolumeHierarchiesNode.h"
#include "SurfaceAeraHeuristic.h"
#include "Camera.h"
#include "Scenes.h"


int main()
{
	auto startTime = std::chrono::high_resolution_clock().now();

	double aspectRatio = 16.0 / 9.0;
	int16_t imageWidth = 1920;

//	double FOV = 50.0;
	double FOV = 80.0;
//	RTW::Vec3 lookFrom(0.0, 0.0, 1.0);
//	RTW::Vec3 LookAt(0.0, 0.0, 0.0);
//	RTW::Vec3 VUp(0.0, 1.0, 0.0);

//	RTW::Vec3 lookFrom(13.0, 2.0, 3.0);
	RTW::Vec3 lookFrom(0.0, 0.0, 9.0);
//	RTW::Vec3 lookFrom(1.0, 200.0, 1.0);
	RTW::Vec3 LookAt(0.0);
	RTW::Vec3 VUp(0.0, 1.0, 0.0);

#ifdef _DEBUG // do not change these values for debug will take for ever otherwise
	int16_t samplesPerPixel = 4;
	int16_t maxBounceDepth = 4;
#else
	int16_t samplesPerPixel = 64;
	int16_t maxBounceDepth = 64; // ridiculously high bounces doesn't seem to have much of an affect on performance
#endif

	RTW::Vec3 gamma(2.4);

	double defocusAngle = 0.0;
	double focusDistance = 0.0;

	[[maybe_unused]] uint32_t numberOfThreads = std::thread::hardware_concurrency();
//	[[maybe_unused]] uint32_t numberOfThreads = 1;
	RTW::g_Threads.resize(numberOfThreads);

	RTW::RayHittables worldHitables;

	// Scene Selection
	RTW::Scenes scene = RTW::Scenes::Parallelograms;
	RTW::SceneSelect(scene, worldHitables);

	RTW::Camera camera(aspectRatio, imageWidth, FOV, defocusAngle, focusDistance, lookFrom, LookAt, VUp, gamma, samplesPerPixel, maxBounceDepth);

	auto finishTime = std::chrono::high_resolution_clock().now();
	std::clog << "Scene set up took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() <<
		" seconds\nNumberOfBoundingBoxes: " << RTW::AABB::GetNumberofBBs() << ".\n";
	startTime = std::chrono::high_resolution_clock().now();

	if (worldHitables.size() > 1)
	{
//		std::shared_ptr<RTW::BaseRayHittable> BVHWorldHitables = std::make_shared<RTW::BVHNode>(worldHitables);
//		std::shared_ptr<RTW::BaseRayHittable> BVHWorldHitables = std::make_shared<RTW::SAHNode>(worldHitables);
		std::shared_ptr<RTW::BaseRayHittable> BVHWorldHitables = std::make_shared<RTW::SAHNode>(worldHitables, numberOfThreads);
		worldHitables.clear();
		worldHitables.add(BVHWorldHitables);

		finishTime = std::chrono::high_resolution_clock().now();
		std::clog << "BVH set up took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds\n";
		startTime = std::chrono::high_resolution_clock().now();
	}

//	camera.Render(worldHitables);
	camera.RenderMultiThreaded(numberOfThreads, worldHitables);

	finishTime = std::chrono::high_resolution_clock().now();

	std::clog << "\rRay tracing time Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds";
	worldHitables.clear();
	std::cin.get();
}
