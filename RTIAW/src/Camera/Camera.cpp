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
		: m_AspectRatio(-DoubleInf), m_ImageWidth(-1), m_SamplesPerPixel(0), m_MaxBounces(0) {}

	Camera::Camera(double AspectRatio, int16_t imageWidth, int16_t samplesPerPixel, int16_t maxBounces)
		: m_AspectRatio(AspectRatio), m_ImageWidth(imageWidth), m_SamplesPerPixel(samplesPerPixel), m_MaxBounces(maxBounces) {}

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
				WriteColour(std::cout, colour * m_SampleScale);
			}
		}
	}

	void Camera::RenderMultiThreaded(const RayHittable& objects)
	{
		Init();
		m_ColourPixelArray = new Colour[m_ImageWidth * m_ImageHeight];

		ctpl::thread_pool threads(32);
		int64_t numberOfPixels = static_cast<int64_t>(m_ImageWidth) * static_cast<int64_t>(m_ImageHeight);
		int64_t numberOfPixelsPerThread = numberOfPixels / 12;
		int64_t pixelsLeft = numberOfPixels % 12;

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		for (int16_t i = 0; i < 12; i++)
		{
			MultiThreadedData* data = new MultiThreadedData;
			data->camera = this;
			data->object = &objects;
			data->offset = numberOfPixelsPerThread * i;
			data->colourArray = &m_ColourPixelArray[data->offset];
			data->numberOfPixels = numberOfPixelsPerThread;

			threads.push(Camera::StaticMultiThreadRenderLoop, data);
		}

		if (0 < pixelsLeft)
		{
			MultiThreadedData data;
			data.camera = this;
			data.object = &objects;
			data.offset = numberOfPixels - pixelsLeft;
			data.colourArray = &m_ColourPixelArray[data.offset];
			data.numberOfPixels = pixelsLeft;
			MultiThreadRenderLoop(&data);
		}

		threads.~thread_pool();

		std::cout << "P3\n" << m_ImageWidth << ' ' << m_ImageHeight << "\n255\n";
		for (int64_t i = 0; i < numberOfPixels; i++)
			WriteColour(std::cout, m_ColourPixelArray[i]);

		delete[] m_ColourPixelArray;
	}

	void Camera::Init()
	{
		m_ImageHeight = static_cast<int16_t>(m_ImageWidth / m_AspectRatio);
		m_ImageHeight = (m_ImageHeight < 1) ? 1 : m_ImageHeight;

		m_SampleScale = 1.0 / m_SamplesPerPixel;

		double focalLength = 2.2;
		double viewportHeight = 1.2;
//		double focalLength = 1.0;
//		double viewportHeight = 2.0;
		double viewportWidth = viewportHeight * (static_cast<double>(m_ImageWidth) / static_cast<double>(m_ImageHeight));

		RTW::Point viewportU(viewportWidth, 0.0, 0.0);
		RTW::Point viewportV(0.0, -viewportHeight, 0.0);

		m_PixelDeltaU = viewportU / static_cast<double>(m_ImageWidth);
		m_PixelDeltaV = viewportV / static_cast<double>(m_ImageHeight);

		RTW::Point viewportUpperLeft = m_Position - RTW::Point(0.0, 0.0, focalLength) - 0.5 * (viewportU + viewportV);

		m_Pixel100Location = viewportUpperLeft + 0.5 * (m_PixelDeltaU + m_PixelDeltaV);
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
//			int16_t maxNewRays = bouncesLeft;
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

		Vec3 rayOrigin = m_Position;
		Vec3 rayDirection = pixelSample - rayOrigin;

		return { rayOrigin, rayDirection };
	}

	glm::dvec2 Camera::SampleSquare()
	{
		return glm::linearRand(glm::vec2(-0.5), glm::vec2(0.5));
	}

	void Camera::MultiThreadRenderLoop(MultiThreadedData* data)
	{
		for (int64_t i = 0; i < data->numberOfPixels; i++)
		{
			Colour colour(0.0);
			for (int16_t k = 0; k < m_SamplesPerPixel; k++)
			{
				int64_t pixel = i + data->offset;
				Ray ray = CreateRay(pixel % m_ImageWidth, pixel / m_ImageWidth);
				colour += RayColour(ray, m_MaxBounces, *data->object);
			}
			data->colourArray[i] = colour * m_SampleScale;
		}
	}

	void Camera::StaticMultiThreadRenderLoop([[maybe_unused]] int id, void* data)
	{
		MultiThreadedData* d = reinterpret_cast<MultiThreadedData*>(data);
		d->camera->MultiThreadRenderLoop(d);
		delete d;
	}
}
