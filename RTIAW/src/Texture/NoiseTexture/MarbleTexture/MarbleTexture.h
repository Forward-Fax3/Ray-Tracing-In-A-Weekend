#pragma once
#include "BaseNoiseTexture.h"


namespace RTW
{
	class MarbleTexture : public BaseNoiseTexture
	{
	public:
		explicit MarbleTexture(double scale = 1.0);
		explicit MarbleTexture(std::shared_ptr<BaseTexture> baseTexture, double scale = 1.0);

		Colour GetColour(const UV& uv, const Point& point) const override;

	private:
		double m_Scale;
		std::shared_ptr<BaseTexture> m_BaseTexture;
	};
}
