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

	RTW::CameraData cameraData{};

	cameraData.ImageWidth = 1920;
	cameraData.VUp = RTW::Vec3(0.0, 1.0, 0.0);

#ifdef _DEBUG // do not change these values for debug will take for ever otherwise
	cameraData.SamplesPerPixel = 4;
	cameraData.MaxBounces = 4;
#else
	cameraData.SamplesPerPixel = 64;
	cameraData.MaxBounces = 1024; // ridiculously high bounces doesn't seem to have much of an affect on performance
#endif

	cameraData.Gamma = RTW::Vec3(2.4);

	cameraData.DefocusAngle = 0.0;
	cameraData.FocusDistance = 0.0;

	[[maybe_unused]] uint32_t numberOfThreads = std::thread::hardware_concurrency();
//	[[maybe_unused]] uint32_t numberOfThreads = 1;
	RTW::g_Threads.resize(numberOfThreads);

	RTW::RayHittables worldHitables;

	// Scene Selection
	RTW::Scenes scene = RTW::Scenes::CollectionOfSpheres;
	RTW::SceneSelect(scene, worldHitables, cameraData);

	RTW::Camera camera(cameraData);

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
