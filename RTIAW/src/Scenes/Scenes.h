#pragma once
#include "RayHittables.h"
#include "Camera.h"


namespace RTW
{
	enum class Scenes : uint8_t
	{
		CollectionOfSpheres,
		LargeCollectionOfSpheres,
		MovingSpheres,
		LargeMovingSpheres,
		CheckeredShperes,
		Earth,
		PerlinNoiseSpheres,
		MarbleSpheres,
		Parallelograms,
		LightTest,
		CornelBox,
		CornelSmoke,
		FinalSceneBook2
	};

	void SceneSelect(Scenes scene, std::shared_ptr<RayHittables> hittables, std::shared_ptr<RayHittables> lights, CameraData& cameraData);
}
