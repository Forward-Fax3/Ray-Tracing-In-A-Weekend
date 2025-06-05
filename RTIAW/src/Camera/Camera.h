#pragma once
#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "RayHittable.h"


namespace RTW
{
	class Camera
	{
	public:
		Camera();
		Camera(double AspectRatio, int16_t imageWidth, int16_t samplesPerPixel, int16_t maxBounces);

		void Render(const RayHittable& objects);
		void RenderMultiThreaded(const RayHittable& objects);

		inline void SetAspectRatio(double AR) { m_AspectRatio = AR; }
		inline void SetImageWidth(int16_t imageWidth) { m_ImageWidth = imageWidth; }

	// Helper structs
	private:
		struct MultiThreadedData
		{
			Camera* camera = nullptr;
			const RayHittable* object = nullptr;
			Colour* colourArray = NULL;
			int64_t numberOfPixels = NULL;
			int64_t offset = NULL;
		};

	// Private helper functions
	private:
		void Init();

		Colour RayColour(const Ray& ray, int16_t bouncesLeft, const RayHittable& object);
		Ray CreateRay(int16_t i, int16_t j);
		glm::dvec2 SampleSquare();

		void MultiThreadRenderLoop(MultiThreadedData* data);
		static void StaticMultiThreadRenderLoop(int id, void* data);

	// Private Variables
	private:
		// Externally changeable
		double m_AspectRatio;
		int16_t m_ImageWidth;
		int16_t m_SamplesPerPixel;
		int16_t m_MaxBounces;

		// Only internally changeable
		int16_t m_ImageHeight = 0;
		double m_SampleScale = 0.0;
		Colour* m_ColourPixelArray = nullptr;
		Point m_Position = Point(0.0, 0.0, 3.0);
//		Point m_Position = Point(0.0);
		Point m_Pixel100Location = Point(0.0);
		Vec3 m_PixelDeltaU = Vec3(0.0);
		Vec3 m_PixelDeltaV = Vec3(0.0);
	};
}
