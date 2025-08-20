#pragma once
#include "Core.h"
#include "BaseTexture.h"


namespace RTW
{
    class SolidColour : public BaseTexture
    {
    public:
        explicit SolidColour(const Colour& colour);

        Colour GetColour(const UV&, const Point&) const override { return m_Albedo; }

    private:
        Colour m_Albedo;
    };
}
