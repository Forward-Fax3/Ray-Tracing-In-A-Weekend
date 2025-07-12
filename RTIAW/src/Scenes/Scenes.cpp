#include "Core.h"

#include "RayHittable.h"
#include "Hittables.h"
#include "Sphere.h"

#include "BaseMaterial.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"

#include "BaseTexture.h"
#include "SolidColour.h"
#include "CheckeredTexture.h"

#include "Scenes.h"

#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"


namespace RTW
{
	void SceneSelect(Scenes scene, RayHittables& hitables)
	{
		switch (scene)
		{
		case RTW::Scenes::CollectionOfShperes:
			CollectionOfShperes<false>(hitables);
			return;
		case RTW::Scenes::MovingSpheres:
			CollectionOfShperes<true>(hitables);
			return;
		case Scenes::CheckeredShperes:
			CheckeredShperes(hitables);
		default:
			return;
		}
	}

	template <bool t_IsMoving>
	void CollectionOfShperes(RayHittables& hitables)
	{

		std::shared_ptr<BaseTexture> Texture = std::make_shared<CheckeredTexture>(Colour(0.2, 0.3, 0.1), Colour(0.9), 0.32);
		std::shared_ptr<BaseMaterial> material = std::make_shared<Lambertian>(Texture);
		hitables.add(std::make_shared<Sphere>(Point(0, -1000, 0), 1000, material));

		for (int a = -11; a < 11; a++)
			for (int b = -11; b < 11; b++)
			{
				double chooseMat = glm::linearRand(0.0, 1.0);
				Point center(Point(a, 0.2, b) + glm::linearRand(Point(0.0), Point(0.9, 0.0, 0.9)));

				if ((center - Point(4, 0.2, 0)).length() > 0.9)
				{
					if (chooseMat < 0.8) // diffuse
					{
						Colour albedo = glm::linearRand(Vec3(0.0), Vec3(1.0)) * glm::linearRand(Vec3(0.0), Vec3(1.0));
						material = std::make_shared<Lambertian>(albedo);
						if constexpr (t_IsMoving)
						{
							Point center2 = center + Vec3(0, glm::linearRand(0.0, 0.5), 0);
							hitables.add(std::make_shared<MovingSphere>(center, center2, 0.2, material));
						}
						else
							hitables.add(std::make_shared<Sphere>(center, 0.2, material));
					}
					else if (chooseMat < 0.95) // metal
					{
						Colour albedo = glm::linearRand(Vec3(0.5), Vec3(1.0));
						double fuzz = glm::linearRand(0.0, 0.5);
						material = std::make_shared<Metal>(albedo, fuzz);
						hitables.add(std::make_shared<Sphere>(center, 0.2, material));
					}
					else // glass
					{
						double refactionIndex = glm::linearRand(0.0, 2.0);
						material = std::make_shared<Dielectric>(refactionIndex);
						hitables.add(std::make_shared<Sphere>(center, 0.2, material));
					}
				}
			}

		material = std::make_shared<Dielectric>(1.5);
		hitables.add(std::make_shared<Sphere>(Point(0, 1, 0), 1.0, material));

		material = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1));
		hitables.add(std::make_shared<Sphere>(Point(-4, 1, 0), 1.0, material));

		material = std::make_shared<Metal>(Colour(0.7, 0.6, 0.5), 0.0);
		hitables.add(std::make_shared<Sphere>(Point(4, 1, 0), 1.0, material));
	}

	void CheckeredShperes(RayHittables& hittables)
	{
		auto checker = std::make_shared<CheckeredTexture>(Colour(0.2, 0.3, 0.1), Colour(0.9), 0.32);

		hittables.add(std::make_shared<Sphere>(Point(0, -10, 0), 10, std::make_shared<Lambertian>(checker)));
		hittables.add(std::make_shared<Sphere>(Point(0, 10, 0), 10, std::make_shared<Lambertian>(checker)));
	}
}
