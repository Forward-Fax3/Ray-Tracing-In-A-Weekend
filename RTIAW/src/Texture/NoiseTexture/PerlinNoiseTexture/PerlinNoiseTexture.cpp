#include "PerlinNoiseTexture.h"

#include "PerlinNoise.h"


namespace RTW
{
	PerlinNoiseTexture::PerlinNoiseTexture(double scale /*= 1.0*/)
		: m_Scale(scale)
	{
		m_Noise = std::make_shared<PerlinNoise>();
	}

	Colour PerlinNoiseTexture::GetColour(const UV& uv, const Point& point) const
	{
		(void)uv;
		return Colour(1.0) * m_Noise->Noise(m_Scale * point);
	}
}
