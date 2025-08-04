#include "Core.h"

#include "RayHittable.h"
#include "RayHittables.h"
#include "Sphere.h"

#include "BaseMaterial.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"

#include "BaseTexture.h"
#include "SolidColour.h"
#include "CheckeredTexture.h"
#include "ImageTexture.h"
#include "PerlinNoiseTexture.h"
#include "MarbleTexture.h"

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
		case RTW::Scenes::CollectionOfSpheres:
			CollectionOfSpheres<false, 22>(hitables);
			return;
		case RTW::Scenes::LargeCollectionOfSpheres:
			CollectionOfSpheres<false, 220>(hitables);
			return;
		case RTW::Scenes::MovingSpheres:
			CollectionOfSpheres<true, 22>(hitables);
			return;
		case RTW::Scenes::LargeMovingSpheres:
			CollectionOfSpheres<true, 220>(hitables);
			return;
		case Scenes::CheckeredShperes:
			CheckeredShperes(hitables);
			return;
		case Scenes::Earth:
			Earth(hitables);
			return;
		case Scenes::PerlinNoiseSpheres:
			PerlinNoiseSpheres(hitables);
			return;
		case Scenes::MarbleSpheres:
			MarbleSpheres(hitables);
			return;
		default:
			return;
		}
	}

	template <bool t_IsMoving, int t_RangeOfSmallBalls>
	void CollectionOfSpheres(RayHittables& hitables)
	{
		std::shared_ptr<BaseTexture> texture = std::make_shared<CheckeredTexture>(Colour(0.2, 0.3, 0.1), Colour(0.9), 0.32);
		std::shared_ptr<BaseMaterial> material = std::make_shared<Lambertian>(texture);
		hitables.add(std::make_shared<Sphere>(Point(0, -1000, 0), 1000, material));

		for (int a = -(t_RangeOfSmallBalls / 2); a < t_RangeOfSmallBalls / 2; a++)
			for (int b = -(t_RangeOfSmallBalls / 2); b < t_RangeOfSmallBalls / 2; b++)
			{
				double chooseMat = glm::linearRand(0.0, 1.0);
				double height = glm::linearRand(0.19, 0.21);
				Point center(Point(a, height, b) + glm::linearRand(Point(0.0), Point(0.9, 0.0, 0.9)));

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
		std::shared_ptr<BaseTexture> checker = std::make_shared<CheckeredTexture>(Colour(0.2, 0.3, 0.1), Colour(0.9), 0.32);

		hittables.add(std::make_shared<Sphere>(Point(0, -10, 0), 10, std::make_shared<Lambertian>(checker)));
		hittables.add(std::make_shared<Sphere>(Point(0, 10, 0), 10, std::make_shared<Lambertian>(checker)));
	}

	void Earth(RayHittables& hitables)
	{
		std::shared_ptr<BaseTexture> earthTexture = std::make_shared<ImageTexture>("earthmap.jpg");
		std::shared_ptr<BaseMaterial> earthMaterial = std::make_shared<Lambertian>(earthTexture);
		hitables.add(std::make_shared<Sphere>(Point(0.0), 2, earthMaterial));
	}

	void PerlinNoiseSpheres(RayHittables& hittables)
	{
		std::shared_ptr<BaseTexture> perlinNoiseTexture = std::make_shared<PerlinNoiseTexture>(4.0);
//		perlinNoiseTexture = std::make_shared<PerlinNoiseTexture>(perlinNoiseTexture, 32);
		hittables.add(std::make_shared<Sphere>(Point(0, -1000, 0), 1000, std::make_shared<Lambertian>(perlinNoiseTexture)));
		hittables.add(std::make_shared<Sphere>(Point(0, 2, 0), 2, std::make_shared<Lambertian>(perlinNoiseTexture)));
	}

	void MarbleSpheres(RayHittables& hittables)
	{
		std::shared_ptr<BaseTexture> marbleTexture = std::make_shared<MarbleTexture>(4.0);
		hittables.add(std::make_shared<Sphere>(Point(0, -1000, 0), 1000, std::make_shared<Lambertian>(marbleTexture)));
		hittables.add(std::make_shared<Sphere>(Point(0, 2, 0), 2, std::make_shared<Lambertian>(marbleTexture)));
	}
}
