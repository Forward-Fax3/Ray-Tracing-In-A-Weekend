#include <iostream>
#include <chrono>
#include <thread>

#include "Core.h"
#include "RayHittables.h"
#include "BoundingVolumeHierarchiesNode.h"
#include "SurfaceAeraHeuristic.h"
#include "Camera.h"
#include "Scenes.h"
#include "WriteColour.h"


int main()
{
	std::srand(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));

	auto startTime = std::chrono::high_resolution_clock::now();

	RTW::WriteFile writeFile;

	if (!writeFile.IsOpen())
	{
		std::cout << "Failed to open File." << '\n';
		std::cin.get();
		return 1;
	}

	RTW::CameraData cameraData{};

	cameraData.ImageWidth = 1920;
//	cameraData.ImageWidth = 480;
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

	uint32_t numberOfThreads = std::thread::hardware_concurrency();
	RTW::g_Threads.resize(numberOfThreads - 1);

	auto sceneHitables(std::make_shared<RTW::RayHittables>());

	// Scene Selection
	RTW::Scenes scene = RTW::Scenes::CornelBox;
	RTW::SceneSelect(scene, sceneHitables, cameraData);
	sceneHitables->addBuffer();

	RTW::Camera camera(cameraData);

	auto finishTime = std::chrono::high_resolution_clock::now();
	std::clog << "Scene set up took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() <<
		" seconds\nNumberOfBoundingBoxes: " << RTW::AABB::GetNumberofBBs() << ".\n";
	startTime = std::chrono::high_resolution_clock::now();

	std::shared_ptr<RTW::BaseRayHittable> worldHittables;

	if (sceneHitables->size() > 1)
	{
//		worldHittables = std::make_shared<RTW::BVHNode>(sceneHitables);
//		worldHittables = std::make_shared<RTW::SAHNode>(sceneHitables);
		worldHittables = std::make_shared<RTW::SAHNode>(sceneHitables, numberOfThreads);
		sceneHitables->clear();

		finishTime = std::chrono::high_resolution_clock::now();
		std::clog << "BVH set up took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds\n";
		startTime = std::chrono::high_resolution_clock::now();
	}
	else
		worldHittables = sceneHitables->GetObjects().front();

//	camera.Render(worldHittables);
	camera.RenderMultiThreaded(numberOfThreads, worldHittables);

	finishTime = std::chrono::high_resolution_clock::now();

	std::clog << "\rRay tracing time Took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds";

	startTime = std::chrono::high_resolution_clock::now();

	const auto& pixels = camera.GetColours();
	const int16_t imageWidth = camera.GetImageWidth();
	const int16_t imageHeight = camera.GetImageHeight();

	writeFile.WriteToFile(pixels, imageWidth, imageHeight);

	finishTime = std::chrono::high_resolution_clock::now();

	std::clog << "\nFile writing took: " << std::chrono::duration_cast<std::chrono::duration<double>>(finishTime - startTime).count() << " seconds";

	sceneHitables->clear();
	std::cin.get();
}
