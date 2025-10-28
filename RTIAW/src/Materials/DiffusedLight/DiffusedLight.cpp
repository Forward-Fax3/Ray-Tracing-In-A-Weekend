#include "Core.h"
#include "BaseMaterial.h"
#include "DiffusedLight.h"

#include "BaseTexture.h"
#include "SolidColour.h"

#include <memory>


namespace RTW
{
	DiffusedLight::DiffusedLight(const Colour& colour)
		: m_Texture(std::make_shared<SolidColour>(Colour(1.0))), m_Brightness(colour) {}

	DiffusedLight::DiffusedLight(const Colour& colour, double brightness)
		: m_Texture(std::make_shared<SolidColour>(colour)), m_Brightness(Colour(brightness)) {}

	DiffusedLight::DiffusedLight(const Colour& colour, const Colour& brightness)
		: m_Texture(std::make_shared<SolidColour>(colour)), m_Brightness(brightness) {}

	DiffusedLight::DiffusedLight(const std::shared_ptr<BaseTexture>& texture, double brightness)
		: m_Texture(texture), m_Brightness(Colour(brightness)) {}

	DiffusedLight::DiffusedLight(const std::shared_ptr<BaseTexture>& texture, const Colour& brightness)
		: m_Texture(texture), m_Brightness(brightness) {}

	Colour DiffusedLight::EmittedColour(const HitData& data, const Point& point) const
	{
		return (!data.isFrontFace) ? Colour(0.0) : m_Texture->GetColour(data.uv, point) * m_Brightness;
	}
}
