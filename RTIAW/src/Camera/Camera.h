#pragma once
#include "Core.h"
#include "Interval.h"
#include "Ray.h"
#include "RayHittable.h"

#include <vector>


namespace RTW
{
	struct CameraData
	{
		double AspectRatio;
		double FOV;
		double DefocusAngle;
		double FocusDistance;
		Point LookFrom;
		Point LookAt;
		Vec3 VUp;
		Vec3 Gamma;
		Colour BackgroundColour;
		int16_t ImageWidth;
		int16_t SamplesPerPixel;
		int16_t MaxBounces;
	};

	class Camera
	{
	public:
		explicit Camera(const CameraData& data);
		Camera();

		void Render(const std::shared_ptr<BaseRayHittable> objects);
		void RenderMultiThreaded(const int32_t numberOfThreads, const std::shared_ptr<BaseRayHittable> objects);

		inline void SetAspectRatio(const double AR) { m_AspectRatio = AR; }
		inline void SetFOV(const double FOV) { m_FOV = FOV; }
		inline void SetDefocusAngle(const double DefocusAngle) { m_DefocusAngle = DefocusAngle; }
		inline void SetFocusDistance(const double FocusDistance) { m_FocusDistance = FocusDistance; }
		inline void SetPosition(const Point& position) { m_Position = position; }
		inline void SetLookFrom(const Point& LookFrom) { m_LookFrom = LookFrom; }
		inline void SetLookAt(const Point& LookAt) { m_LookAt = LookAt; }
		inline void SetUp(const Vec3& VUp) { m_VUp = VUp; }
		inline void SetGamma(const Vec3& Gamma) { m_Gamma = Gamma; }
		inline void SetImageWidth(const int16_t ImageWidth) { m_ImageWidth = ImageWidth; }
		inline void SetSamplesPerPixel(const int16_t SamplesPerPixel) { m_SamplesPerPixel = SamplesPerPixel; }
		inline void SetMaxBounces(const int16_t MaxBounces) { m_MaxBounces = MaxBounces; }
		inline void SetBAckgroundColour(const Colour& colour) { m_BackgroundColour = colour; };

	// Private helper functions
	private:
		void Init();

		Colour RayColour(Ray& ray, int16_t bouncesLeft, const std::shared_ptr<BaseRayHittable> object) const;
		Ray CreateRay(int16_t i, int16_t j) const;
		glm::dvec2 SampleSquare() const;
		Point DefocusDiskSample() const;

		inline Colour ColourCorrection(const Colour& colour) const;

		void MultiThreadRenderLoop(size_t offset, size_t increment, const std::shared_ptr<BaseRayHittable> object);

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
		Colour m_BackgroundColour;
		int16_t m_ImageWidth;
		int16_t m_SamplesPerPixel;
		int16_t m_MaxBounces;

		// Only internally changeable
		int16_t m_ImageHeight = 0;
		double m_SampleScale = 0.0;
		size_t m_NumberOfPixels = 0;
		std::vector<Colour> m_ColourPixelArray;
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
