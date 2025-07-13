#include "Core.h"
#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm/glm.hpp"

#include <iostream>
#include <string>
#include <cstdint>


namespace RTW
{
	ImageLoader::ImageLoader(const std::string& fileName)
	{
		if (Load("Assets/" + fileName)) return;

		std::cerr << "Unable to open file: " << fileName << '.' << std::flush;
	}

	ImageLoader::~ImageLoader()
	{
		delete[] m_Colours;
	}

	bool ImageLoader::Load(const std::string& filePath)
	{
		stbi_set_flip_vertically_on_load(true);

		int32_t temp = m_ComponentsPerPixel;
		glm::i32vec2 tempSizes(0i32);
		float* data = stbi_loadf(filePath.c_str(), &tempSizes.x, &tempSizes.y, &temp, m_ComponentsPerPixel);
		if (data == nullptr) return false;

		m_ImageSize = tempSizes;
		m_Colours = new Colour[m_ImageSize.x * m_ImageSize.y];
		
		for (size_t i = 0ui64, j = 0ui64; j < m_ImageSize.x * m_ImageSize.y * 3ui64; i++, j += 3)
		{
			m_Colours[i].r = data[j + 0];
			m_Colours[i].g = data[j + 1];
			m_Colours[i].b = data[j + 2];
		}

		STBI_FREE(data);

		m_NumberOfPixelsPerScanline = m_ImageSize.x;
		return true;
	}

	const Colour& ImageLoader::GetPixelColour(glm::i64vec2 pixelCoordinates) const
	{
		static const Colour errorMegenta(1.0, 0.0, 1.0);
		return (m_Colours != nullptr) ? m_Colours[m_NumberOfPixelsPerScanline * pixelCoordinates.y + pixelCoordinates.x] : errorMegenta;
	}
}
