#pragma once
#include "Core.h"
#include "BaseMaterial.h"

#include "BaseTexture.h"


namespace RTW
{
    class DiffusedLight :  public BaseMaterial
    {
    public:
        explicit DiffusedLight(const std::shared_ptr<BaseTexture>& texture);
        explicit DiffusedLight(const Colour& colour);

        Colour EmittedColour(const UV& uv, const Point& point) const override;

    private:
        std::shared_ptr<BaseTexture> m_Texture;
    };
}
