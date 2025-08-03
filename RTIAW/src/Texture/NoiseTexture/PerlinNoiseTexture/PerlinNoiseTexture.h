#pragma once
#include "BaseNoiseTexture.h"


namespace RTW
{
    class PerlinNoiseTexture : public BaseNoiseTexture
    {
    public:
        PerlinNoiseTexture(double scale = 1.0);

        virtual Colour GetColour(const UV& uv, const Point& point) const override;

    private:
        double m_Scale;
    };
}
