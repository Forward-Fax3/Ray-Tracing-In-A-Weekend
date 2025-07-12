#pragma once
#include "Core.h"
#include "BaseTexture.h"


namespace RTW
{
    class SolidColour : public BaseTexture
    {
    public:
        SolidColour(const Colour& colour);

        virtual const Colour& GetColour([[maybe_unused]] const UV& uv, [[maybe_unused]] const Point& point) const override { return m_Albedo; }

    private:
        Colour m_Albedo;
    };
}
