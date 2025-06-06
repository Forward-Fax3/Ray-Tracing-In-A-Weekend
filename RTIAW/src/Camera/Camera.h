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
		Camera(double AspectRatio, int16_t imageWidth, double FOV, Point lookFrom, Point lookAt, Vec3 VUp, int16_t samplesPerPixel, int16_t maxBounces);

		void Render(const RayHittable& objects);
		void RenderMultiThreaded(const int16_t numberOfThreads, const RayHittable& objects);

		inline void SetAspectRatio(double AR) { m_AspectRatio = AR; }
		inline void SetImageWidth(int16_t imageWidth) { m_ImageWidth = imageWidth; }

	// Private helper functions
	private:
		void Init();

		Colour RayColour(const Ray& ray, int16_t bouncesLeft, const RayHittable& object);
		Ray CreateRay(int16_t i, int16_t j);
		glm::dvec2 SampleSquare();

		void MultiThreadRenderLoop(int64_t offset, int64_t increment, const RayHittable& object);
		static void StaticMultiThreadRenderLoop(int id, Camera& camera, int64_t offset, int64_t increment, const RayHittable* object);

	// Private Variables
	private:
		// Externally changeable
		double m_AspectRatio;
		double m_FOV;
		union {
			Point m_LookFrom;
			Point m_Position;
		};
		Point m_LookAt;
		Vec3 m_VUp;
		int16_t m_ImageWidth;
		int16_t m_SamplesPerPixel;
		int16_t m_MaxBounces;

		// Only internally changeable
		int16_t m_ImageHeight = 0;
		double m_SampleScale = 0.0;
		int64_t m_NumberOfPixels = 0;
		Colour* m_ColourPixelArray = nullptr;
		Point m_Pixel100Location = Point(0.0);
		Vec3 m_PixelDeltaU = Vec3(0.0);
		Vec3 m_PixelDeltaV = Vec3(0.0);
		Vec3 m_U = Vec3(0.0);
		Vec3 m_V = Vec3(0.0);
		Vec3 m_W = Vec3(0.0);
	};
}
