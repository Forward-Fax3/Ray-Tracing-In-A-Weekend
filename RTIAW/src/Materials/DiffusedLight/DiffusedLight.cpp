#include "Core.h"
#include "BaseMaterial.h"
#include "DiffusedLight.h"

#include "BaseTexture.h"
#include "SolidColour.h"

#include <memory>


namespace RTW
{
	DiffusedLight::DiffusedLight(const std::shared_ptr<BaseTexture>& texture)
		: m_Texture(texture) {}

	DiffusedLight::DiffusedLight(const Colour& colour)
		: m_Texture(std::make_shared<SolidColour>(colour)) {}

	Colour DiffusedLight::EmittedColour(const UV& uv, const Point& point) const
	{
		return m_Texture->GetColour(uv, point);
	}
}
