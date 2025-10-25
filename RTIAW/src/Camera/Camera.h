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
		Camera() = delete;
		explicit Camera(const CameraData& data);

		void Render(const std::shared_ptr<BaseRayHittable> objects);
		void RenderMultiThreaded(const int32_t numberOfThreads, const std::shared_ptr<BaseRayHittable> objects);

		RTW_FORCE_INLINE void SetAspectRatio(const double AR) { m_AspectRatio = AR; }
		RTW_FORCE_INLINE void SetFOV(const double FOV) { m_FOV = FOV; }
		RTW_FORCE_INLINE void SetDefocusAngle(const double DefocusAngle) { m_DefocusAngle = DefocusAngle; }
		RTW_FORCE_INLINE void SetFocusDistance(const double FocusDistance) { m_FocusDistance = FocusDistance; }
		RTW_FORCE_INLINE void SetPosition(const Point& position) { m_Position = position; }
		RTW_FORCE_INLINE void SetLookFrom(const Point& LookFrom) { m_LookFrom = LookFrom; }
		RTW_FORCE_INLINE void SetLookAt(const Point& LookAt) { m_LookAt = LookAt; }
		RTW_FORCE_INLINE void SetUp(const Vec3& VUp) { m_VUp = VUp; }
		RTW_FORCE_INLINE void SetGamma(const Vec3& Gamma) { m_Gamma = Gamma; }
		RTW_FORCE_INLINE void SetImageWidth(const int16_t ImageWidth) { m_ImageWidth = ImageWidth; }
		RTW_FORCE_INLINE void SetSamplesPerPixel(const int16_t SamplesPerPixel) { m_SamplesPerPixel = SamplesPerPixel; }
		RTW_FORCE_INLINE void SetMaxBounces(const int16_t MaxBounces) { m_MaxBounces = MaxBounces; }
		RTW_FORCE_INLINE void SetBAckgroundColour(const Colour& colour) { m_BackgroundColour = colour; }

		RTW_FORCE_INLINE const std::vector<Colour>& GetColours() const { return m_ColourPixelArray; }
		RTW_FORCE_INLINE int16_t GetImageWidth() const { return m_ImageWidth; }
		RTW_FORCE_INLINE int16_t GetImageHeight() const { return m_ImageHeight; }

		RTW_FORCE_INLINE static const std::shared_ptr<BaseRayHittable>& GetObjects() { return s_Instance->m_Objects; }
		RTW_FORCE_INLINE static const Colour& GetBackgroundColour() { return s_Instance->m_BackgroundColour; }

	// Private helper functions
	private:
		void Init();

		Colour RayColour(Ray& ray, int16_t& bouncesLeft) const;
		Ray CreateRay(int16_t i, int16_t j, int16_t sI, int16_t sJ) const;

		RTW_FORCE_INLINE glm::dvec2 SampleSquare() const
		{
			return glm::linearRand(glm::vec2(-0.5), glm::vec2(0.5));
		}

		RTW_FORCE_INLINE Point DefocusDiskSample() const
		{
			Point point = RandomOnUnitDisk();
			return m_Position + point.x * m_DefocusDiskU + point.y * m_DefocusDiskV;
		}

		RTW_FORCE_INLINE Vec2 RandomSquareStratified(int16_t sI, int16_t sJ) const
		{
			auto p = Vec2(static_cast<double>(sI), static_cast<double>(sJ));
			p += glm::linearRand(Vec2(0.0), Vec2(1.0));
			p *= m_RsqrtSamplesPerPixel;
			p -= 0.5;

			return p;
		}

		inline Colour ColourCorrection(const Colour& colour) const;

		void MultiThreadRenderLoop(size_t offset, size_t increment);

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
		std::shared_ptr<BaseRayHittable> m_Objects = nullptr;
		Point m_Pixel100Location = Point(0.0);
		Vec3 m_PixelDeltaU = Vec3(0.0);
		Vec3 m_PixelDeltaV = Vec3(0.0);
		Vec3 m_DefocusDiskU = Vec3(0.0);
		Vec3 m_DefocusDiskV = Vec3(0.0);
		Vec3 m_U = Vec3(0.0);
		Vec3 m_V = Vec3(0.0);
		Vec3 m_W = Vec3(0.0);
		Vec3 m_InvGamma = Vec3(0.0);
		double m_RsqrtSamplesPerPixel = 0.0;
		int16_t m_SqrtSamplesPerPixel = 0;

		static Camera* s_Instance;
	};
}
