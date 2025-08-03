#include "MarbleTexture.h"

#include "PerlinNoise.h"
#include "SolidColour.h"


namespace RTW
{
	MarbleTexture::MarbleTexture(double scale /*= 1.0*/)
		: m_Scale(scale)
	{
		m_BaseTexture = std::make_shared<SolidColour>(Colour(1.0));
		m_Noise = std::make_shared<PerlinNoise>();
	}

	MarbleTexture::MarbleTexture(std::shared_ptr<BaseTexture> baseTexture, double scale /*= 1.0*/)
		: m_Scale(scale), m_BaseTexture(baseTexture)
	{
		m_Noise = std::make_shared<PerlinNoise>();
	}

	Colour MarbleTexture::GetColour(const UV& uv, const Point& point) const
	{
		return m_BaseTexture->GetColour(uv, point) * 0.5 * (1.0 + glm::sin(m_Scale * point.z + 10.0 * m_Noise->Turbulation(point, 7)));
	}
}
