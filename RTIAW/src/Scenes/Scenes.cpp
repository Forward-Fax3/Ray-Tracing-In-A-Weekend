#include "Core.h"

#include "RayHittable.h"
#include "RayHittables.h"
#include "Sphere.h"
#include "Parallelogram.h"

#include "BaseMaterial.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"
#include "DiffusedLight.h"
#include "ConstantMedium.h"

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
	template <bool t_IsMoving, int t_RangeOfSmallBalls>
	static void CollectionOfSpheres(std::shared_ptr<RayHittables> hittables, CameraData& cameraData)
	{
		std::shared_ptr<BaseTexture> texture = std::make_shared<CheckeredTexture>(Colour(0.2, 0.3, 0.1), Colour(0.9), 0.32);
		std::shared_ptr<BaseMaterial> material = std::make_shared<Lambertian>(texture);
		hittables->add(std::make_shared<Sphere>(Point(0, -1000, 0), 1000, material));

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
							hittables->add(std::make_shared<MovingSphere>(center, center2, 0.2, material));
						}
						else
							hittables->add(std::make_shared<Sphere>(center, 0.2, material));
					}
					else if (chooseMat < 0.95) // metal
					{
						Colour albedo = glm::linearRand(Vec3(0.5), Vec3(1.0));
						double fuzz = glm::linearRand(0.0, 0.5);
						material = std::make_shared<Metal>(albedo, fuzz);
						hittables->add(std::make_shared<Sphere>(center, 0.2, material));
					}
					else // glass
					{
						double refactionIndex = glm::linearRand(0.0, 2.0);
						material = std::make_shared<Dielectric>(refactionIndex);
						hittables->add(std::make_shared<Sphere>(center, 0.2, material));
					}
				}
			}

		material = std::make_shared<Dielectric>(1.5);
		hittables->add(std::make_shared<Sphere>(Point(0, 1, 0), 1.0, material));

		material = std::make_shared<Lambertian>(Colour(0.4, 0.2, 0.1));
		hittables->add(std::make_shared<Sphere>(Point(-4, 1, 0), 1.0, material));

		material = std::make_shared<Metal>(Colour(0.7, 0.6, 0.5), 0.0);
		hittables->add(std::make_shared<Sphere>(Point(4, 1, 0), 1.0, material));

		cameraData.AspectRatio = 16.0 / 9.0;
		cameraData.FOV = 50.0;
		cameraData.LookFrom = RTW::Point(13.0, 2.0, 3.0);
		cameraData.LookAt = RTW::Point(0.0);
		cameraData.BackgroundColour = RTW::Colour(0.70, 0.80, 1.00);
	}

	static void CheckeredShperes(std::shared_ptr<RayHittables> hittables, CameraData& cameraData)
	{
		std::shared_ptr<BaseTexture> checker = std::make_shared<CheckeredTexture>(Colour(0.2, 0.3, 0.1), Colour(0.9), 0.32);

		hittables->add(std::make_shared<Sphere>(Point(0, -10, 0), 10, std::make_shared<Lambertian>(checker)));
		hittables->add(std::make_shared<Sphere>(Point(0, 10, 0), 10, std::make_shared<Lambertian>(checker)));

		cameraData.AspectRatio = 16.0 / 9.0;
		cameraData.FOV = 20.0;
		cameraData.LookFrom = RTW::Point(13.0, 2.0, 3.0);
		cameraData.LookAt = RTW::Point(0.0);
		cameraData.BackgroundColour = RTW::Colour(0.70, 0.80, 1.00);
	}

	static void Earth(std::shared_ptr<RayHittables> hittables, CameraData& cameraData)
	{
		std::shared_ptr<BaseTexture> earthTexture = std::make_shared<ImageTexture>("earthmap.jpg");
		std::shared_ptr<BaseMaterial> earthMaterial = std::make_shared<Lambertian>(earthTexture);
		hittables->add(std::make_shared<Sphere>(Point(0.0), 2, earthMaterial));

		cameraData.AspectRatio = 16.0 / 9.0;
		cameraData.FOV = 20.0;
		cameraData.LookFrom = Point(12.0, 1.5, 0.0);
		cameraData.LookAt = RTW::Point(0.0);
		cameraData.BackgroundColour = RTW::Colour(0.70, 0.80, 1.00);
	}

	static void PerlinNoiseSpheres(std::shared_ptr<RayHittables> hittables, CameraData& cameraData)
	{
		std::shared_ptr<BaseTexture> perlinNoiseTexture = std::make_shared<PerlinNoiseTexture>(4.0);
		//		perlinNoiseTexture = std::make_shared<PerlinNoiseTexture>(perlinNoiseTexture, 32);
		hittables->add(std::make_shared<Sphere>(Point(0, -1000, 0), 1000, std::make_shared<Lambertian>(perlinNoiseTexture)));
		hittables->add(std::make_shared<Sphere>(Point(0, 2, 0), 2, std::make_shared<Lambertian>(perlinNoiseTexture)));

		cameraData.AspectRatio = 16.0 / 9.0;
		cameraData.FOV = 20.0;
		cameraData.LookFrom = RTW::Point(13.0, 2.0, 3.0);
		cameraData.LookAt = RTW::Point(0.0);
		cameraData.BackgroundColour = RTW::Colour(0.70, 0.80, 1.00);
	}

	static void MarbleSpheres(std::shared_ptr<RayHittables> hittables, CameraData& cameraData)
	{
		std::shared_ptr<BaseTexture> marbleTexture = std::make_shared<MarbleTexture>(4.0);
		hittables->add(std::make_shared<Sphere>(Point(0, -1000, 0), 1000, std::make_shared<Lambertian>(marbleTexture)));
		hittables->add(std::make_shared<Sphere>(Point(0, 2, 0), 2, std::make_shared<Lambertian>(marbleTexture)));

		cameraData.AspectRatio = 16.0 / 9.0;
		cameraData.FOV = 20.0;
		cameraData.LookFrom = RTW::Point(13.0, 2.0, 3.0);
		cameraData.LookAt = RTW::Point(0.0);
		cameraData.BackgroundColour = RTW::Colour(0.70, 0.80, 1.00);
	}

	static void Parallelograms(std::shared_ptr<RayHittables> hittables, CameraData& cameraData)
	{
		// Materials
		std::shared_ptr<BaseMaterial> leftRed = std::make_shared<Lambertian>(Colour(1.0, 0.2, 0.2));
		std::shared_ptr<BaseMaterial> backGreen = std::make_shared<Lambertian>(Colour(0.2, 1.0, 0.2));
		std::shared_ptr<BaseMaterial> rightBlue = std::make_shared<Lambertian>(Colour(0.2, 0.2, 1.0));
		std::shared_ptr<BaseMaterial> topOrange = std::make_shared<Lambertian>(Colour(1.0, 0.5, 0.0));
		std::shared_ptr<BaseMaterial> bottomTeal = std::make_shared<Lambertian>(Colour(0.2, 0.8, 0.8));

		// UV Points
		UVvec3 leftUV(Vec3(0.0, 0.0, -4.0), Vec3(0, 4, 0));
		UVvec3 backUV(Vec3(4, 0, 0), Vec3(0, 4, 0));
		UVvec3 rightUV(Vec3(0, 0, 4), Vec3(0, 4, 0));
		UVvec3 topUV(Vec3(4, 0, 0), Vec3(0, 0, 4));
		UVvec3 bottomUV(Vec3(4, 0, 0), Vec3(0, 0, -4));

		// Quads
		hittables->add(std::make_shared<Parallelogram>(Point(-3, -2, 5), leftUV, leftRed));
		hittables->add(std::make_shared<Parallelogram>(Point(-2, -2, 0), backUV, backGreen));
		hittables->add(std::make_shared<Parallelogram>(Point(3, -2, 1), rightUV, rightBlue));
		hittables->add(std::make_shared<Parallelogram>(Point(-2, 3, 1), topUV, topOrange));
		hittables->add(std::make_shared<Parallelogram>(Point(-2, -3, 5), bottomUV, bottomTeal));

		cameraData.AspectRatio = 1.0;
		cameraData.FOV = 80.0;
		cameraData.LookFrom = RTW::Point(0.0, 0.0, 9.0);
		cameraData.LookAt = RTW::Point(0.0);
		cameraData.BackgroundColour = RTW::Colour(0.70, 0.80, 1.00);
	}

	static void LightTest(std::shared_ptr<RayHittables> hittables, CameraData& cameraData)
	{
		std::shared_ptr<BaseTexture> pertext = std::make_shared<MarbleTexture>(4.0);
		hittables->add(std::make_shared<Sphere>(Point(0.0, -1000.0, 0.0), 1000.0, std::make_shared<Lambertian>(pertext)));
		hittables->add(std::make_shared<Sphere>(Point(0.0, 2.0, 0), 2.0, std::make_shared<Lambertian>(pertext)));

		auto difflight = std::make_shared<DiffusedLight>(Colour(20.0));
		hittables->add(std::make_shared<Parallelogram>(Point(3.0, 1.0, -2.0), UVvec3(Vec3(2.0, 0.0, 0.0), Vec3(0.0, 2.0, 0.0)), difflight));

		cameraData.AspectRatio = 16.0 / 9.0;
		cameraData.FOV = 20.0;
		cameraData.LookFrom = RTW::Point(26.0, 3.0, 6.0);
		cameraData.LookAt = RTW::Point(0.0, 2.0, 0.0);
		cameraData.BackgroundColour = RTW::Colour(0.0);
	}

	static void CornelBox(std::shared_ptr<RayHittables> hittables, CameraData& cameraData)
	{
		auto red(std::make_shared<Lambertian>(Colour(0.65, 0.05, 0.05)));
		auto white(std::make_shared<Lambertian>(Colour(0.73, 0.73, 0.73)));
		auto green(std::make_shared<Lambertian>(Colour(0.12, 0.45, 0.15)));
		auto glass(std::make_shared<Dielectric>(1.6));
		auto light(std::make_shared<DiffusedLight>(Colour(1.0), 50.0));

		hittables->add(std::make_shared<Parallelogram>(Point(555.0,   0.0,   0.0), UVvec3(Vec3(   0.0, 555.0, 0.0), Vec3(0, 0,  555)), green));
		hittables->add(std::make_shared<Parallelogram>(Point(0.0),                 UVvec3(Vec3(   0.0, 555.0, 0.0), Vec3(0, 0,  555)), red  ));
		hittables->add(std::make_shared<Parallelogram>(Point(343.0, 554.0, 332.0), UVvec3(Vec3(-130.0,   0.0, 0.0), Vec3(0, 0, -105)), light));
		hittables->add(std::make_shared<Parallelogram>(Point(0.0),                 UVvec3(Vec3( 555.0,   0.0, 0.0), Vec3(0, 0,  555)), white));
		hittables->add(std::make_shared<Parallelogram>(Point(555.0),               UVvec3(Vec3(-555.0,   0.0, 0.0), Vec3(0, 0, -555)), white));
		hittables->add(std::make_shared<Parallelogram>(Point(0.0,     0.0, 555.0), UVvec3(Vec3( 555.0,   0.0, 0.0), Vec3(0, 555,  0)), white));

		hittables->add(CreateBox(Point(105.0, -1e-8, 65.0), Point(270.0, 165.0, 230.0), white, Vec3(18.0, 0.0, 0.0)));
		hittables->add(CreateBox(Point(290.0, 0.0, 295.0), Point(455.0, 330.0, 460.0), white, Vec3(-15.0, 0.0, 0.0)));

		hittables->add(std::make_shared<Sphere>(Point(350.0, 50.0, 100.0), 50.0, glass));

		cameraData.AspectRatio = 1.0;
		cameraData.FOV = 40.0;
		cameraData.LookFrom = RTW::Point(278.0, 278.0, -800.0);
		cameraData.LookAt = RTW::Point(278, 278, 0);
		cameraData.BackgroundColour = RTW::Colour(0.0);
	}

	static void CornelSmoke(std::shared_ptr<RayHittables> hittables, CameraData& cameraData)
	{
		auto red(std::make_shared<Lambertian>(Colour(0.65, 0.05, 0.05)));
		auto white(std::make_shared<Lambertian>(Colour(0.73, 0.73, 0.73)));
		auto green(std::make_shared<Lambertian>(Colour(0.12, 0.45, 0.15)));
		auto glass(std::make_shared<Dielectric>(1.6));
		auto light(std::make_shared<DiffusedLight>(Colour(1.0), 7.0));
		auto whiteSmoke(std::make_shared<ConstantMedium>(1, Colour(1.0)));
		auto blackSmoke(std::make_shared<ConstantMedium>(1, Colour(0.0)));

		hittables->add(std::make_shared<Parallelogram>(Point(555.0,   0.0,   0.0), UVvec3(Vec3(   0.0, 555.0, 0.0), Vec3(0, 0,  555)), green));
		hittables->add(std::make_shared<Parallelogram>(Point(0.0),                 UVvec3(Vec3(   0.0, 555.0, 0.0), Vec3(0, 0,  555)), red  ));
		hittables->add(std::make_shared<Parallelogram>(Point(113.0, 554.0, 127.0), UVvec3(Vec3( 330.0,   0.0, 0.0), Vec3(0, 0,  305)), light));
		hittables->add(std::make_shared<Parallelogram>(Point(0.0),                 UVvec3(Vec3( 555.0,   0.0, 0.0), Vec3(0, 0,  555)), white));
		hittables->add(std::make_shared<Parallelogram>(Point(555.0),               UVvec3(Vec3(-555.0,   0.0, 0.0), Vec3(0, 0, -555)), white));
		hittables->add(std::make_shared<Parallelogram>(Point(0.0,     0.0, 555.0), UVvec3(Vec3( 555.0,   0.0, 0.0), Vec3(0, 555,  0)), white));

		hittables->add(CreateBox(Point(105.0, -1e-8, 65.0),  Point(270.0, 165.0, 230.0), blackSmoke, Vec3( 18.0, 0.0, 0.0)));
		hittables->add(CreateBox(Point(290.0, -1e-8, 295.0), Point(455.0, 330.0, 460.0), whiteSmoke, Vec3(-15.0, 0.0, 0.0)));

		hittables->add(std::make_shared<Sphere>(Point(350.0, 50.0, 100.0), 50.0, glass));
		hittables->add(std::make_shared<Sphere>(Point(350.0, 170.0, 380.0), 50.0, glass));

		cameraData.AspectRatio = 1.0;
		cameraData.FOV = 40.0;
		cameraData.LookFrom = RTW::Point(278.0, 278.0, -800.0);
		cameraData.LookAt = RTW::Point(278, 278, 0);
		cameraData.BackgroundColour = RTW::Colour(0.0);
	}

	void FinalSceneBook2(std::shared_ptr<RayHittables> hittables, CameraData& cameraData)
	{
		auto ground = std::make_shared<Lambertian>(Colour(0.48, 0.83, 0.53));

		int boxes_per_side = 20;
		for (int i = 0; i < boxes_per_side; i++) {
			for (int j = 0; j < boxes_per_side; j++) {
				auto w = 100.0;
				auto x0 = -1000.0 + i * w;
				auto z0 = -1000.0 + j * w;
				auto y0 = 0.0;
				auto x1 = x0 + w;
				auto y1 = glm::linearRand(1.0, 101.0);
				auto z1 = z0 + w;

				hittables->add(CreateBox(Point(x0, y0, z0), Point(x1, y1, z1), ground));
			}
		}

		hittables->add(std::make_shared<Parallelogram>(Point(123.0, 554.0, 147.0), UVvec3(Vec3(300.0, 0.0, 0.0), Vec3(0.0, 0.0, 265.0)), std::make_shared<DiffusedLight>(Colour(7.0))));

		hittables->add(std::make_shared<MovingSphere>(Point(400.0, 400.0, 200.0), Point(430.0, 400.0, 200.0), 50.0, std::make_shared<Lambertian>(Colour(0.7, 0.3, 0.1))));

		hittables->add(std::make_shared<Sphere>(Point(260.0, 150.0, 45.0), 50.0, std::make_shared<Dielectric>(1.5)));
		hittables->add(std::make_shared<Sphere>(Point(0.0, 150.0, 145.0), 50.0, std::make_shared<Metal>(Colour(0.8, 0.8, 0.9), 1.0)));

//		hittables->add(std::make_shared<Sphere>(Point(360.0, 150.0, 145.0), 70.0, std::make_shared<Metal>(Colour(0.1, 0.2, 0.9), 0.8)));
		hittables->add(std::make_shared<Sphere>(Point(360.0, 150.0, 145.0), 70.0, std::make_shared<ConstantMedium>(0.2, Colour(0.2, 0.4, 0.9))));
		hittables->add(std::make_shared<Sphere>(Point(360.0, 150.0, 145.0), 70.05, std::make_shared<Dielectric>(1.5)));
//		hittables->add(std::make_shared<Sphere>(Point(478.0, 278.0, -600.0), 277.0, std::make_shared<ConstantMedium>(0.0001, Colour(1.0))));
//		hittables->add(std::make_shared<Sphere>(Point(478.0, 278.0, -600.0), 277.0, std::make_shared<ConstantMedium>(0.005, Colour(1.0))));
//		hittables->add(std::make_shared<Sphere>(Point(0.0), 277.0, std::make_shared<ConstantMedium>(0.0001, Colour(1.0))));
//		hittables->add(std::make_shared<Sphere>(Point(0.0), 5000.0, std::make_shared<ConstantMedium>(0.0001, Colour(1.0))));

		hittables->add(std::make_shared<Sphere>(Point(400.0, 200.0, 400.0), 100.0, std::make_shared<Lambertian>(std::make_shared<ImageTexture>("earthmap.jpg"))));

		hittables->add(std::make_shared<Sphere>(Point(220.0, 280.0, 300.0), 80.0, std::make_shared<Lambertian>(std::make_shared<PerlinNoiseTexture>(0.2))));

		auto white = std::make_shared<Lambertian>(Colour(0.73, 0.73, 0.73));
		int ns = 1000;
		Vec3 min = RotateAroundPoint3D(glm::radians(Vec3(0.0, 15.0, 0.0)), Point(0.0), Vec3(165.0 / 2.0));
		Vec3 max = RotateAroundPoint3D(glm::radians(Vec3(0.0, 15.0, 0.0)), Point(165.0), Vec3(165.0 / 2.0));
		for (int j = 0; j < ns; j++) {
			hittables->add(make_shared<Sphere>(Vec3(-100, 270, 395) + glm::linearRand(min, max), 10.0, white));
		}

		cameraData.AspectRatio = 1.0;
		cameraData.BackgroundColour = Colour(0, 0, 0);

		cameraData.FOV = 40;
		cameraData.LookFrom = Point(478.0, 278.0, -600.0);
		cameraData.LookAt = Point(278.0, 278.0, 0);
	}

	void SceneSelect(Scenes scene, std::shared_ptr<RayHittables> hittables, CameraData& cameraData)
	{
		switch (scene)
		{
		case RTW::Scenes::CollectionOfSpheres:
			CollectionOfSpheres<false, 22>(hittables, cameraData);
			return;
		case RTW::Scenes::LargeCollectionOfSpheres:
			CollectionOfSpheres<false, 220>(hittables, cameraData);
			return;
		case RTW::Scenes::MovingSpheres:
			CollectionOfSpheres<true, 22>(hittables, cameraData);
			return;
		case RTW::Scenes::LargeMovingSpheres:
			CollectionOfSpheres<true, 220>(hittables, cameraData);
			return;
		case Scenes::CheckeredShperes:
			CheckeredShperes(hittables, cameraData);
			return;
		case Scenes::Earth:
			Earth(hittables, cameraData);
			return;
		case Scenes::PerlinNoiseSpheres:
			PerlinNoiseSpheres(hittables, cameraData);
			return;
		case Scenes::MarbleSpheres:
			MarbleSpheres(hittables, cameraData);
			return;
		case Scenes::Parallelograms:
			Parallelograms(hittables, cameraData);
			return;
		case Scenes::LightTest:
			LightTest(hittables, cameraData);
			return;
		case Scenes::CornelBox:
			CornelBox(hittables, cameraData);
			return;
		case Scenes::CornelSmoke:
			CornelSmoke(hittables, cameraData);
			return;
		case Scenes::FinalSceneBook2:
			FinalSceneBook2(hittables, cameraData);
			return;
		default:
			return;
		}
	}
}
