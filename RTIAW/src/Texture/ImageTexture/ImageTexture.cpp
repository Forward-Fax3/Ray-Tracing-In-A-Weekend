#include "Core.h"
#include "Interval.h"
#include "BaseTexture.h"
#include "ImageLoader.h"
#include "ImageTexture.h"

#include "glm/glm.hpp"

#include <string>


namespace RTW
{
	ImageTexture::ImageTexture(const std::string& fileName)
		: m_Image(fileName) {}

	Colour ImageTexture::GetColour(const UV& uv, const Point&) const
	{
		static const Colour errorCyan(0.0, 1.0, 1.0);
		if (m_Image.GetHeight() == 0) return errorCyan;

		UV clampedUV = Interval(0.0, 1.0).Clamp(uv);
		clampedUV.y = 1.0 - clampedUV.y;

		glm::i64vec2 pixelCoordiates = uv * glm::f64vec2(static_cast<double>(m_Image.GetWidth()), static_cast<double>(m_Image.GetHeight()));
		return m_Image.GetPixelColour(pixelCoordiates);
	}
}
