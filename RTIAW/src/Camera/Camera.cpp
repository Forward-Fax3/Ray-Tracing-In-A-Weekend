#include <iostream>
#include <stdint.h>

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"

#include "ctpl_stl.h"

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
		: m_AspectRatio(0.0), m_FOV(0.0), m_DefocusAngle(0.0), m_FocusDistance(0.0), m_LookFrom(0.0), m_LookAt(0.0), m_VUp(0.0), m_Gamma(0.0), m_ImageWidth(0), m_SamplesPerPixel(0), m_MaxBounces(0) {}


	Camera::Camera(double AspectRatio, int16_t imageWidth, double FOV, double defocusAngle, double focusDistance, Point lookFrom, Point lookAt, Vec3 VUp, Vec3 gamma, int16_t samplesPerPixel, int16_t maxBounces)
		: m_AspectRatio(AspectRatio), m_FOV(FOV), m_DefocusAngle(defocusAngle), m_FocusDistance(focusDistance), m_LookFrom(lookFrom), m_LookAt(lookAt), m_VUp(VUp), m_Gamma(gamma), m_ImageWidth(imageWidth), m_SamplesPerPixel(samplesPerPixel), m_MaxBounces(maxBounces) {}

	void Camera::Render(const RayHittable& objects)
	{
		Init();

		std::cout << "P3\n" << m_ImageWidth << ' ' << m_ImageHeight << "\n255\n";

		for (int16_t i = 0; i < m_ImageHeight; i++)
		{
			std::clog << "\rScanlines remaining: " << (m_ImageHeight - i) << ' ' << std::flush;

			for (int16_t j = 0; j < m_ImageWidth; j++)
			{
				Colour colour(0.0);
				for (int16_t k = 0; k < m_SamplesPerPixel; k++)
				{
					Ray ray = CreateRay(j, i);
					colour += RayColour(ray, m_MaxBounces, objects);
				}
				WriteColour(std::cout, ColourCorrection(colour));
			}
		}
	}

	void Camera::RenderMultiThreaded(const int16_t numberOfThreads, const RayHittable& objects)
	{
		Init();
		m_ColourPixelArray = new Colour[m_ImageWidth * m_ImageHeight];

		ctpl::thread_pool threads(numberOfThreads);
		m_NumberOfPixels = static_cast<int64_t>(m_ImageWidth) * static_cast<int64_t>(m_ImageHeight);

		for (int16_t i = 0; i < numberOfThreads; i++)
			threads.push(Camera::StaticMultiThreadRenderLoop, *this, i, numberOfThreads, &objects);

		threads.~thread_pool(); // Waits for all threads to finish and then deletes the thread pool.

		std::clog << "\rDone.                 \nWriting pixels to file" << std::flush;

		std::cout << "P3\n" << m_ImageWidth << ' ' << m_ImageHeight << "\n255\n";
		for (int64_t i = 0; i < m_NumberOfPixels; i++)
			WriteColour(std::cout, m_ColourPixelArray[i]);

		delete[] m_ColourPixelArray;
	}

	void Camera::Init()
	{
		m_GammaInv = 1.0 / m_Gamma;

		m_ImageHeight = static_cast<int16_t>(m_ImageWidth / m_AspectRatio);
		m_ImageHeight = (m_ImageHeight < 1) ? 1 : m_ImageHeight;

		m_SampleScale = 1.0 / m_SamplesPerPixel;

		double focalLength = (m_FocusDistance == 0.0) ? (m_LookFrom - m_LookAt).length() : m_FocusDistance;
		double theta = glm::radians(m_FOV);
		double h = glm::tan(theta / 2);
		double viewportHeight = 2 * h * focalLength;
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

		double defocusRadius = m_FocusDistance * std::tan(glm::radians(m_DefocusAngle * 0.5));
		m_DefocusDiskU = m_U * defocusRadius;
		m_DefocusDiskV = m_V * defocusRadius;
	}

	Colour Camera::RayColour(const Ray& ray, int16_t bouncesLeft, const RayHittable& object)
	{
		if (bouncesLeft <= 0)
			return { 0.0, 0.0, 0.0 };

		HitData data;
		if (object.IsRayHit(ray, Interval(0.001, DoubleInf), data))
		{
			Ray newRay;
			Colour colour;
			return data.material->Scatter(ray, data, colour, newRay) ?
				colour * RayColour(newRay, bouncesLeft - 1, object) : Colour(0.0);
		}

//		if (object.IsRayHit(ray, Interval(0.001, DoubleInf), data))
//		{
//			Ray newRay;
//			Colour colour;
//			Colour returnColour(0.0);
//
//			int16_t maxNewRays = 3;
//
//			for (int16_t i = 0; i < maxNewRays; i++)
//				returnColour += data.material->Scatter(ray, data, colour, newRay) ? colour * RayColour(newRay, bouncesLeft - 1, object) : Colour(0.0);
//
//			return returnColour / static_cast<double>(maxNewRays);
//		}
			
		Vec3 normalizedDirection = glm::normalize(ray.direction());
		double a = 0.5 * (normalizedDirection.y + 1.0);
		return Colour(1.0 - a) + a * Colour(0.5, 0.7, 1.0);
	}

	Ray Camera::CreateRay(int16_t j, int16_t i)
	{
		glm::dvec2 offset = SampleSquare() + glm::dvec2(j, i);
		Vec3 pixelSample = m_Pixel100Location + offset.x * m_PixelDeltaU + offset.y * m_PixelDeltaV;

		Point rayOrigin = (m_DefocusAngle <= 0.0) ? m_Position : DefocusDiskSample();
		Vec3 rayDirection = pixelSample - rayOrigin;

		return { rayOrigin, rayDirection };
	}

	glm::dvec2 Camera::SampleSquare()
	{
		return glm::linearRand(glm::vec2(-0.5), glm::vec2(0.5));
	}

	RTW::Point Camera::DefocusDiskSample()
	{
		Point point = RandomOnUnitDisk();
		return m_Position + point.x * m_DefocusDiskU + point.y * m_DefocusDiskV;
	}

	Colour Camera::ColourCorrection(const Colour colour) const
	{
		return Interval(0.0, 0.999).Clamp(glm::pow(colour * m_SampleScale, m_GammaInv)) * 256.0;
	}

	void Camera::MultiThreadRenderLoop(int64_t offset, int64_t increment, const RayHittable& object)
	{
		for (int64_t i = offset; i < m_NumberOfPixels; i += increment)
		{
			if (i % m_ImageWidth == 0)
				std::clog << "\rScanlines remaining: " << (m_ImageHeight - (i / m_ImageWidth)) << ' ' << std::flush;

			Colour colour(0.0);
			for (int16_t k = 0; k < m_SamplesPerPixel; k++)
			{
				Ray ray = CreateRay(i % m_ImageWidth, i / m_ImageWidth);
				colour += RayColour(ray, m_MaxBounces, object);
			}
			m_ColourPixelArray[i] = ColourCorrection(colour);
		}
	}

	void Camera::StaticMultiThreadRenderLoop([[maybe_unused]] int id, Camera& camera, int64_t offset, int64_t increment, const RayHittable* object)
	{
		camera.MultiThreadRenderLoop(offset, increment, *object);
	}
}
