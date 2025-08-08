#pragma once
#include "Core.h"
#include "BaseTexture.h"


namespace RTW
{
    class SolidColour : public BaseTexture
    {
    public:
        explicit SolidColour(const Colour& colour);

        Colour GetColour([[maybe_unused]] const UV& uv, [[maybe_unused]] const Point& point) const override { return m_Albedo; }

    private:
        Colour m_Albedo;
    };
}
