#pragma once
#include "Core.h"


namespace RTW
{
	class BaseNoise
	{
	public:
		~BaseNoise() = default;

		virtual double Noise(const Point& point) const = 0;
		virtual double Turbulation(const Point& point, size_t depth) const = 0;
	};
}
