#pragma once
#include "Core.h"
#include "BaseMaterial.h"

#include "BaseTexture.h"
#include "SolidColour.h"


namespace RTW
{
    class DiffusedLight :  public BaseMaterial
    {
    public:
		explicit DiffusedLight(const Colour& colour);
		DiffusedLight(const Colour& colour, double brightness);
		DiffusedLight(const Colour& colour, const Colour& brightness);
		DiffusedLight(const std::shared_ptr<BaseTexture>& texture, double brightness);
		DiffusedLight(const std::shared_ptr<BaseTexture>& texture, const Colour& brightness);

        Colour EmittedColour(const UV& uv, const Point& point) const override;

    private:
        std::shared_ptr<BaseTexture> m_Texture;
        Colour m_Brightness;
    };
}
