#pragma once
#include "Core.h"
#include "BaseTexture.h"
#include "ImageLoader.h"

#include <string>


namespace RTW
{
	class ImageTexture : public BaseTexture
	{
	public:
		ImageTexture(const std::string& fileName);

		virtual const Colour& GetColour(const UV& uv, const Point& point) const override;

	private:
		ImageLoader m_Image;
	};
}
