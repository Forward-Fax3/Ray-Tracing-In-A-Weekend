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
	};

	void SceneSelect(Scenes scene, RayHittables& hitables);

	template <bool t_IsMoving, int t_RangeOfSmallBalls>
	void CollectionOfSpheres(RayHittables& hitables);

	void CheckeredShperes(RayHittables& hittables);

	void Earth(RayHittables& hitables);
}
