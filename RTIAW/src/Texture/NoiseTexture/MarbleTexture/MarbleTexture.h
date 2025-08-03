#pragma once
#include "BaseNoiseTexture.h"


namespace RTW
{
	class MarbleTexture : public BaseNoiseTexture
	{
	public:
		MarbleTexture(double scale = 1.0);
		MarbleTexture(std::shared_ptr<BaseTexture> baseTexture, double scale = 1.0);

		virtual Colour GetColour(const UV& uv, const Point& point) const override;

	private:
		double m_Scale;
		std::shared_ptr<BaseTexture> m_BaseTexture;
	};
}
