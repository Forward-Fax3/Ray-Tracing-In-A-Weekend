#include <iostream>

#include "Core.h"
#include "Interval.h"
#include "WriteColour.h"
#include "Ray.h"
#include "RayHittable.h"
#include "Camera.h"


namespace RTW
{
	Camera::Camera()
		: m_AspectRatio(-DoubleInf), m_ImageWidth(-1) {}

	Camera::Camera(double AspectRatio, int32_t imageWidth)
		: m_AspectRatio(AspectRatio), m_ImageWidth(imageWidth) {}

	void Camera::Render(const RayHittable& objects)
	{
		Init();

		std::cout << "P3\n" << m_ImageWidth << ' ' << m_ImageHeight << "\n255\n";

		for (size_t j = 0; j < m_ImageHeight; ++j)
		{
			std::clog << "\rScanlines remaining: " << (m_ImageHeight - j) << ' ' << std::flush;

			for (size_t i = 0; i < m_ImageWidth; i++)
			{
				RTW::Point pixelCenter = m_Pixel100Location + (static_cast<double>(i) * m_PixelDeltaU) + (static_cast<double>(j) * m_PixelDeltaV);
				RTW::Vec3 rayDirection = pixelCenter - m_Position;
				RTW::Ray ray(m_Position, rayDirection);

				RTW::Colour pixelColor = RayColour(ray, objects);
				RTW::WriteColour(std::cout, pixelColor);
			}
		}
	}

	void Camera::Init()
	{
		m_ImageHeight = static_cast<int16_t>(m_ImageWidth / m_AspectRatio);
		m_ImageHeight = (m_ImageHeight < 1) ? 1 : m_ImageHeight;

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

	RTW::Colour Camera::RayColour(const Ray& ray, const RayHittable& object)
	{
		RTW::HitData data;
		if (object.IsRayHit(ray, RTW::Interval(0, RTW::DoubleInf), data))
			return 0.5 * (data.normal + 1.0);

		RTW::Vec3 normalizedDirection = glm::normalize(ray.direction());
		double a = 0.5 * (normalizedDirection.y + 1.0);
		return RTW::Colour(1.0 - a) + a * RTW::Colour(0.5, 0.7, 1.0);
	}
}