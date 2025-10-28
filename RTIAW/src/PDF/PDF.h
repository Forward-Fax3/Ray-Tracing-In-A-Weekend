#pragma once
#include "Core.h"


namespace RTW
{
	class PDF
	{
	public:
		virtual ~PDF() = default;

		virtual double Value(const Vec3& direction) const = 0;
		virtual Vec3 Generate() const = 0;
	};
}
