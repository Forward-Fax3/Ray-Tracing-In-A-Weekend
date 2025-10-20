#include "Core.h"
#include "ImageLoader.h"

#pragma warning(push)
#pragma warning(disable: 4244)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma warning(pop)

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

	bool ImageLoader::Load(const std::string& filePath)
	{
		stbi_set_flip_vertically_on_load(true);

		static constexpr int32_t componentsPerPixel = 3;
		int32_t componentsPerPixelTemp = componentsPerPixel;
		glm::i32vec2 tempSizes(0);

		std::unique_ptr<float[], decltype([](float data[]) { STBI_FREE(data); })> data(stbi_loadf(filePath.c_str(), &tempSizes.x, &tempSizes.y, &componentsPerPixelTemp, componentsPerPixel));

		if (data == nullptr) return false;

		m_ImageSize = tempSizes;
		m_Colours.reserve(m_ImageSize.x * m_ImageSize.y);
		
		for (size_t i = 0; i < m_ImageSize.x * m_ImageSize.y * 3; i += 3)
			m_Colours.emplace_back(static_cast<double>(data[i + 0]), static_cast<double>(data[i + 1]), static_cast<double>(data[i + 2]));

		m_NumberOfPixelsPerScanline = m_ImageSize.x;
		return true;
	}

	const Colour& ImageLoader::GetPixelColour(glm::u64vec2 pixelCoordinates) const
	{
		static const Colour errorMegenta(1.0, 0.0, 1.0);
		return (!m_Colours.empty()) ? m_Colours[m_NumberOfPixelsPerScanline * pixelCoordinates.y + pixelCoordinates.x] : errorMegenta;
	}
}
