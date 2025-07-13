#pragma once
#include "Core.h"



namespace RTW
{
	class BaseTexture
	{
	public:
		virtual ~BaseTexture() = default;

		virtual const Colour& GetColour(const UV& uv, const Point& point) const = 0;
	};
}
