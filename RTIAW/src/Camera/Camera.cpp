#include <iostream>
#include <stdint.h>
#include <limits>

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"

#include "Core.h"
#include "Interval.h"
#include "WriteColour.h"
#include "Ray.h"
#include "RayHittable.h"
#include "Sphere.h"
#include "Camera.h"


namespace RTW
{
	Camera::Camera()
		: m_AspectRatio(0.0), m_FOV(0.0), m_DefocusAngle(0.0), m_FocusDistance(0.0), m_LookFrom(0.0), m_LookAt(0.0), m_VUp(0.0), m_Gamma(0.0), m_BackgroundColour(0.0), m_ImageWidth(0), m_SamplesPerPixel(0), m_MaxBounces(0) { }


	Camera::Camera(const CameraData& data)
		: m_AspectRatio(data.AspectRatio), m_FOV(data.FOV), m_DefocusAngle(data.DefocusAngle), m_FocusDistance(data.FocusDistance), m_LookFrom(data.LookFrom), m_LookAt(data.LookAt), m_VUp(data.VUp),
		m_Gamma(data.Gamma), m_BackgroundColour(data.BackgroundColour), m_ImageWidth(data.ImageWidth), m_SamplesPerPixel(data.SamplesPerPixel), m_MaxBounces(data.MaxBounces) { }

	void Camera::Render(const std::shared_ptr<BaseRayHittable> objects)
	{
		Init();
		m_ColourPixelArray.reserve(m_NumberOfPixels);

		for (int16_t i = 0; i < m_ImageHeight; i++)
		{
			std::clog << "\rScanlines remaining: " << (m_ImageHeight - i) << ' ' << std::flush;

			for (int16_t j = 0; j < m_ImageWidth; j++)
			{
				Colour colour(0.0);
				for (int16_t k = 0; k < m_SamplesPerPixel; k++)
				{
					Ray ray = CreateRay(i, j);
					colour += RayColour(ray, m_MaxBounces, objects);
				}
				m_ColourPixelArray.emplace_back(ColourCorrection(colour));
			}
		}
	}

	void Camera::RenderMultiThreaded(const int32_t numberOfThreads, const std::shared_ptr<BaseRayHittable> objects)
	{
		Init();
		m_ColourPixelArray.resize(m_NumberOfPixels);

		for (int16_t i = 0; i < numberOfThreads - 1; i++)
			g_Threads.push([this, i, numberOfThreads, objects](int) {
				this->MultiThreadRenderLoop(i, numberOfThreads, objects);
			});

		MultiThreadRenderLoop(numberOfThreads - 1, numberOfThreads, objects);

		g_Threads.stop(true); // Waits for all threads to finish.

		std::clog << "\rDone.                 \nWriting pixels to file" << std::flush;
	}

	void Camera::Init()
	{
		m_InvGamma = 1.0 / m_Gamma;

		m_ImageHeight = static_cast<int16_t>(m_ImageWidth / m_AspectRatio);
		m_ImageHeight = (m_ImageHeight < 1) ? 1 : m_ImageHeight;

		m_SampleScale = 1.0 / m_SamplesPerPixel;

		double focalLength = (m_FocusDistance == 0.0) ? (m_LookFrom - m_LookAt).length() : m_FocusDistance;
		double theta = glm::radians(m_FOV);
		double h = glm::tan(theta / 2.0);
		double viewportHeight = 2.0 * h * focalLength;
		double viewportWidth = viewportHeight * (static_cast<double>(m_ImageWidth) / static_cast<double>(m_ImageHeight));

		m_W = glm::normalize(m_LookFrom - m_LookAt);
		m_U = glm::normalize(glm::cross(m_VUp, m_W));
		m_V = glm::cross(m_W, m_U);

		RTW::Point viewportU(viewportWidth * m_U);
		RTW::Point viewportV(viewportHeight * -m_V);

		m_PixelDeltaU = viewportU / static_cast<double>(m_ImageWidth);
		m_PixelDeltaV = viewportV / static_cast<double>(m_ImageHeight);

		RTW::Point viewportUpperLeft = m_Position - focalLength * m_W - 0.5 * (viewportU + viewportV);

		m_Pixel100Location = viewportUpperLeft + 0.5 * (m_PixelDeltaU + m_PixelDeltaV);

		double defocusRadius = m_FocusDistance * glm::tan(glm::radians(m_DefocusAngle * 0.5));
		m_DefocusDiskU = m_U * defocusRadius;
		m_DefocusDiskV = m_V * defocusRadius;

		m_MaxBounces++;

		m_NumberOfPixels = static_cast<size_t>(m_ImageWidth) * static_cast<size_t>(m_ImageHeight);
	}

	Colour Camera::RayColour(Ray& ray, int16_t bouncesLeft, const std::shared_ptr<BaseRayHittable>& object) const
	{
		if (bouncesLeft <= 0)
			return { 0.0, 0.0, 0.0 };

		HitData data;
		if (!object->IsRayHit(ray, Interval(0.001, doubleInf), data))
			return m_BackgroundColour;

		Colour emittedColour = data.material->EmittedColour(data.uv, data.point);

		ScatterReturn scatteredData = data.material->Scatter(ray, data);

		if (!scatteredData.bounced)
			return emittedColour;

		return emittedColour + scatteredData.attenuation * RayColour(ray, bouncesLeft - 1, object);
	}

	Ray Camera::CreateRay(int16_t i, int16_t j) const
	{
		glm::dvec2 offset = SampleSquare() + glm::dvec2(j, i);
		Vec3 pixelSample = m_Pixel100Location + offset.x * m_PixelDeltaU + offset.y * m_PixelDeltaV;

		Point rayOrigin = (m_DefocusAngle <= 0.0) ? m_Position : DefocusDiskSample();
		Vec3 rayDirection = pixelSample - rayOrigin;
		double rayTime = glm::linearRand(0.0, 1.0);

		return { rayOrigin, rayDirection, rayTime };
	}

	glm::dvec2 Camera::SampleSquare() const
	{
		return glm::linearRand(glm::vec2(-0.5), glm::vec2(0.5));
	}

	RTW::Point Camera::DefocusDiskSample() const
	{
		Point point = RandomOnUnitDisk();
		return m_Position + point.x * m_DefocusDiskU + point.y * m_DefocusDiskV;
	}

	Colour Camera::ColourCorrection(const Colour& colour) const
	{
		static Interval minMax(0.0, 0.999);
		Vec3 scalledGammaCorrectedAndClampedColour{};
#if (RTW_AVX2 || RTW_AVX512) && (!defined(__clang__) || defined(RTW_INTEL_COMPILER))
		Vec3 scalledAndGammaCorrectedColour{};
		scalledAndGammaCorrectedColour.data = _mm256_pow_pd((colour * m_SampleScale).data, m_InvGamma.data);
		scalledGammaCorrectedAndClampedColour.data = _mm256_min_pd(_mm256_max_pd(scalledAndGammaCorrectedColour.data, _mm256_set1_pd(minMax.GetMin())), _mm256_set1_pd(minMax.GetMax()));
#elif defined(__clang__) // clang does not support SMVL so need to do the standard way
		scalledGammaCorrectedAndClampedColour = minMax.Clamp(glm::pow(colour * m_SampleScale, m_InvGamma));
#else
		Vec3 scalledColour(colour * m_SampleScale);
		Vec3 scalledAndGammaCorrectedColour{};

		scalledAndGammaCorrectedColour.data.setv(0, _mm_pow_pd(scalledColour.data.getv(0), m_InvGamma.data.getv(0)));
		scalledGammaCorrectedAndClampedColour.data.setv(0, _mm_min_pd(_mm_max_pd(scalledAndGammaCorrectedColour.data.getv(0), _mm_set1_pd(minMax.GetMin())), _mm_set1_pd(minMax.GetMax())));

		scalledAndGammaCorrectedColour.z = glm::pow(scalledColour.z, m_InvGamma.z);
		scalledGammaCorrectedAndClampedColour.z = minMax.Clamp(scalledAndGammaCorrectedColour.z);
#endif
		return scalledGammaCorrectedAndClampedColour * static_cast<double>(std::numeric_limits<uint16_t>::max());
	}

	void Camera::MultiThreadRenderLoop(size_t offset, size_t increment, const std::shared_ptr<BaseRayHittable> object)
	{
		for (size_t i = offset; i < m_NumberOfPixels; i += increment)
		{
//			if (i % m_ImageWidth == 0)
//				std::clog << "\rScanlines remaining: " << (m_ImageHeight - (i / m_ImageWidth)) << ' ' << std::flush;

			Colour colour(0.0);
			for (int16_t k = 0; k < m_SamplesPerPixel; k++)
			{
				Ray ray = CreateRay(static_cast<int16_t>(i / m_ImageWidth), i % m_ImageWidth);
				colour += RayColour(ray, m_MaxBounces, object);
			}
			m_ColourPixelArray[i] = ColourCorrection(colour);
		}
	}
}
