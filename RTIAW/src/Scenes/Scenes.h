#pragma once
#include "Hittables.h"


namespace RTW
{
	enum class Scenes : uint8_t
	{
		CollectionOfSpheres,
		MovingSpheres,
		CheckeredShperes,
		Earth,
	};

	void SceneSelect(Scenes scene, RayHittables& hitables);

	template <bool t_IsMoving>
	void CollectionOfSpheres(RayHittables& hitables);

	void CheckeredShperes(RayHittables& hittables);

	void Earth(RayHittables& hitables);
}
