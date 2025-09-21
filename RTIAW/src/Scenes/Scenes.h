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
	};

	void SceneSelect(Scenes scene, std::shared_ptr<RayHittables> hittables, CameraData& cameraData);
}
