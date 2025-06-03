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
		Camera(double AspectRatio, int16_t imageWidth, int16_t samplesPerPixel);

		void Render(const RayHittable& objects);

		inline void SetAspectRatio(double AR) { m_AspectRatio = AR; }
		inline void SetImageWidth(int16_t imageWidth) { m_ImageWidth = imageWidth; }

	// Private helper functions
	private:
		void Init();

		Colour RayColour(const Ray& ray, const RayHittable& object);
		Ray CreateRay(int16_t i, int16_t j);
		glm::dvec2 SampleSquare();

	// Private Variables
	private:
		// externally changeable
		double m_AspectRatio;
		int16_t m_ImageWidth;
		int16_t m_SamplesPerPixel;

		// only internally changeable
		int16_t m_ImageHeight = 0;
		double m_SampleScale = 0.0;
		Point m_Position = Point(0.0, 0.0, 3.0);
		Point m_Pixel100Location = Point(0.0);
		Vec3 m_PixelDeltaU = Vec3(0.0);
		Vec3 m_PixelDeltaV = Vec3(0.0);
	};
}
