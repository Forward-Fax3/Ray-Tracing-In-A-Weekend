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
		explicit ImageTexture(const std::string& fileName);

		Colour GetColour(const UV& uv, const Point&) const override;

	private:
		ImageLoader m_Image;
	};
}
