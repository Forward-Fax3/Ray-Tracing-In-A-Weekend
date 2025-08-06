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
	};

	void SceneSelect(Scenes scene, RayHittables& hitables, CameraData& cameraData);

	template <bool t_IsMoving, int t_RangeOfSmallBalls>
	void CollectionOfSpheres(RayHittables& hittables, CameraData& cameraData);

	void CheckeredShperes(RayHittables& hittables, CameraData& cameraData);

	void Earth(RayHittables& hittables, CameraData& cameraData);

	void PerlinNoiseSpheres(RayHittables& hittables, CameraData& cameraData);

	void MarbleSpheres(RayHittables& hittables, CameraData& cameraData);

	void Parallelograms(RayHittables& hittables, CameraData& cameraData);

	void LightTest(RayHittables& hittables, CameraData& cameraData);

	void CornelBox(RayHittables& hittables, CameraData& cameraData);
}
