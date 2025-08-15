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

	void SceneSelect(Scenes scene, std::shared_ptr<RayHittables> hittables, CameraData& cameraData);

	template <bool t_IsMoving, int t_RangeOfSmallBalls>
	void CollectionOfSpheres(std::shared_ptr<RayHittables> hittables, CameraData& cameraData);

	void CheckeredShperes(std::shared_ptr<RayHittables> hittables, CameraData& cameraData);

	void Earth(std::shared_ptr<RayHittables> hittables, CameraData& cameraData);

	void PerlinNoiseSpheres(std::shared_ptr<RayHittables> hittables, CameraData& cameraData);

	void MarbleSpheres(std::shared_ptr<RayHittables> hittables, CameraData& cameraData);

	void Parallelograms(std::shared_ptr<RayHittables> hittables, CameraData& cameraData);

	void LightTest(std::shared_ptr<RayHittables> hittables, CameraData& cameraData);

	void CornelBox(std::shared_ptr<RayHittables> hittables, CameraData& cameraData);
}
