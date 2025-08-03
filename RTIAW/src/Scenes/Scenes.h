#pragma once
#include "Hittables.h"


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
	};

	void SceneSelect(Scenes scene, RayHittables& hitables);

	template <bool t_IsMoving, int t_RangeOfSmallBalls>
	void CollectionOfSpheres(RayHittables& hitables);

	void CheckeredShperes(RayHittables& hittables);

	void Earth(RayHittables& hitables);

	void PerlinNoiseSpheres(RayHittables& hittables);

	void MarbleSpheres(RayHittables& hittables);
}
