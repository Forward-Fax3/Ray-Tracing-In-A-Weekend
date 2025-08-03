#include "PerlinNoiseTexture.h"

#include "PerlinNoise.h"
#include "SolidColour.h"


namespace RTW
{
	PerlinNoiseTexture::PerlinNoiseTexture(double scale /*= 1.0*/)
		: m_Scale(scale)
	{
		m_BaseTexture = std::make_shared<SolidColour>(Colour(1.0));
		m_Noise = std::make_shared<PerlinNoise>();
	}

	PerlinNoiseTexture::PerlinNoiseTexture(std::shared_ptr<BaseTexture> baseTexture, double scale /*= 1.0*/)
		: m_Scale(scale), m_BaseTexture(baseTexture)
	{
		m_Noise = std::make_shared<PerlinNoise>();
	}

	Colour PerlinNoiseTexture::GetColour(const UV& uv, const Point& point) const
	{
		return m_BaseTexture->GetColour(uv, point) * m_Noise->Noise(m_Scale * point);
	}
}
