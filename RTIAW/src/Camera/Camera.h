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
		Camera(double AspectRatio, int16_t imageWidth, double FOV, double defocusAngle, double focusDistance, Point lookFrom, Point lookAt, Vec3 VUp, Vec3 gamma, int16_t samplesPerPixel, int16_t maxBounces);

		void Render(const BaseRayHittable& objects);
		void RenderMultiThreaded(const int32_t numberOfThreads, const BaseRayHittable& objects);

		inline void SetAspectRatio(const double AR) { m_AspectRatio = AR; }
		inline void SetFOV(const double FOV) { m_FOV = FOV; }
		inline void SetDefocusAngle(const double defocusAngle) { m_DefocusAngle = defocusAngle; }
		inline void SetFocusDistance(const double focusDistance) { m_FocusDistance = focusDistance; }
		inline void SetPosition(const Point position) { m_Position = position; }
		inline void SetLookFrom(const Point lookFrom) { m_LookFrom = lookFrom; }
		inline void SetLookAt(const Point lookAt) { m_LookAt = lookAt; }
		inline void SetUp(const Vec3 VUp) { m_VUp = VUp; }
		inline void SetGamma(const Vec3 gamma) { m_Gamma = gamma; }
		inline void SetImageWidth(const int16_t imageWidth) { m_ImageWidth = imageWidth; }
		inline void SetSamplesPerPixel(const int16_t samplesPerPixel) { m_SamplesPerPixel = samplesPerPixel; }
		inline void SetMaxBounces(const int16_t maxBounces) { m_MaxBounces = maxBounces; }

	// Private helper functions
	private:
		void Init();

		Colour RayColour(const Ray& ray, int16_t bouncesLeft, const BaseRayHittable& object);
		Ray CreateRay(int16_t i, int16_t j);
		glm::dvec2 SampleSquare();
		Point DefocusDiskSample() const;

		inline Colour ColourCorrection(const Colour colour) const;

		void MultiThreadRenderLoop([[maybe_unused]] int id, size_t offset, size_t increment, const BaseRayHittable& object);

	// Private Variables
	private:
		// Externally changeable
		double m_AspectRatio;
		double m_FOV;
		double m_DefocusAngle;
		double m_FocusDistance;
		union {
			Point m_Position;
			Point m_LookFrom;
		};
		Point m_LookAt;
		Vec3 m_VUp;
		Vec3 m_Gamma;
		int16_t m_ImageWidth;
		int16_t m_SamplesPerPixel;
		int16_t m_MaxBounces;

		// Only internally changeable
		int16_t m_ImageHeight = 0;
		double m_SampleScale = 0.0;
		size_t m_NumberOfPixels = 0;
		Colour* m_ColourPixelArray = nullptr;
		Point m_Pixel100Location = Point(0.0);
		Vec3 m_PixelDeltaU = Vec3(0.0);
		Vec3 m_PixelDeltaV = Vec3(0.0);
		Vec3 m_DefocusDiskU = Vec3(0.0);
		Vec3 m_DefocusDiskV = Vec3(0.0);
		Vec3 m_U = Vec3(0.0);
		Vec3 m_V = Vec3(0.0);
		Vec3 m_W = Vec3(0.0);
		Vec3 m_InvGamma = Vec3(0.0);
	};
}
