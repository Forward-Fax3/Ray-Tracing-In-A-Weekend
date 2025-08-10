#pragma once
#include "Core.h"

#include "glm/glm.hpp"

#include <string>
#include <string_view>

#include <cstdint>
#include <vector>


namespace RTW
{
	class ImageLoader
	{
	public:
		ImageLoader() = delete;
		explicit ImageLoader(const std::string& fileName);
		~ImageLoader() = default;

		bool Load(const std::string& filePath);

		const Colour& GetPixelColour(glm::u64vec2 pixelCoordinates) const;

		inline uint64_t GetWidth() const { return m_ImageSize.x; }
		inline uint64_t GetHeight() const { return m_ImageSize.y; }
		
	private:
		glm::u64vec2 m_ImageSize = glm::u64vec2(0);
		int64_t m_NumberOfPixelsPerScanline = 0;
		std::vector<Colour> m_Colours;
	};
}
