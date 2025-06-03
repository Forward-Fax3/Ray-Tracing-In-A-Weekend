#include <iostream>

#include "glm/gtc/random.hpp"

#include "Core.h"
#include "Interval.h"
#include "WriteColour.h"
#include "Ray.h"
#include "RayHittable.h"
#include "Camera.h"


namespace RTW
{
	Camera::Camera()
		: m_AspectRatio(-DoubleInf), m_ImageWidth(-1), m_SamplesPerPixel(0) {}

	Camera::Camera(double AspectRatio, int16_t imageWidth, int16_t samplesPerPixel)
		: m_AspectRatio(AspectRatio), m_ImageWidth(imageWidth), m_SamplesPerPixel(samplesPerPixel) {}

	void Camera::Render(const RayHittable& objects)
	{
		Init();

		std::cout << "P3\n" << m_ImageWidth << ' ' << m_ImageHeight << "\n255\n";

		for (int16_t j = 0; j < m_ImageHeight; ++j)
		{
			std::clog << "\rScanlines remaining: " << (m_ImageHeight - j) << ' ' << std::flush;

			for (int16_t i = 0; i < m_ImageWidth; i++)
			{
				Colour pixelColour(0.0);
				for (int16_t k = 0; k < m_SamplesPerPixel; k++)
				{
					Ray ray = CreateRay(i, j);
					pixelColour += RayColour(ray, objects);
				}

				WriteColour(std::cout, pixelColour * m_SampleScale);
			}
		}
	}

	void Camera::Init()
	{
		m_ImageHeight = static_cast<int16_t>(m_ImageWidth / m_AspectRatio);
		m_ImageHeight = (m_ImageHeight < 1) ? 1 : m_ImageHeight;

		m_SampleScale = 1.0 / m_SamplesPerPixel;

		double focalLength = 2.2;
		double viewportHeight = 1.2;
		double viewportWidth = viewportHeight * (static_cast<double>(m_ImageWidth) / static_cast<double>(m_ImageHeight));

		RTW::Point viewportU(viewportWidth, 0.0, 0.0);
		RTW::Point viewportV(0.0, -viewportHeight, 0.0);

		m_PixelDeltaU = viewportU / static_cast<double>(m_ImageWidth);
		m_PixelDeltaV = viewportV / static_cast<double>(m_ImageHeight);

		RTW::Point viewportUpperLeft = m_Position - RTW::Point(0.0, 0.0, focalLength) - 0.5 * (viewportU + viewportV);

		m_Pixel100Location = viewportUpperLeft + 0.5 * (m_PixelDeltaU + m_PixelDeltaV);
	}

	Colour Camera::RayColour(const Ray& ray, const RayHittable& object)
	{
		RTW::HitData data;
		if (object.IsRayHit(ray, RTW::Interval(0, RTW::DoubleInf), data))
			return 0.5 * (data.normal + 1.0);

		RTW::Vec3 normalizedDirection = glm::normalize(ray.direction());
		double a = 0.5 * (normalizedDirection.y + 1.0);
		return RTW::Colour(1.0 - a) + a * RTW::Colour(0.5, 0.7, 1.0);
	}

	Ray Camera::CreateRay(int16_t i, int16_t j)
	{
		glm::dvec2 offset = SampleSquare() + glm::dvec2(i, j);
		Vec3 pixelSample = m_Pixel100Location + offset.x * m_PixelDeltaU + offset.y * m_PixelDeltaV;

		Vec3 rayOrigin = m_Position;
		Vec3 rayDirection = pixelSample - rayOrigin;

		return { rayOrigin, rayDirection };
	}

	glm::dvec2 Camera::SampleSquare()
	{
		return { glm::linearRand(-0.5, 0.5), glm::linearRand(-0.5, 0.5) };
	}
}