#pragma once
#include "BaseNoiseTexture.h"


namespace RTW
{
    class PerlinNoiseTexture : public BaseNoiseTexture
    {
    public:
        explicit(false) PerlinNoiseTexture(double scale = 1.0);
        explicit(false) PerlinNoiseTexture(std::shared_ptr<BaseTexture> baseTexture, double scale = 1.0);

        Colour GetColour(const UV& uv, const Point& point) const override;

    private:
        double m_Scale;
        std::shared_ptr<BaseTexture> m_BaseTexture;
    };
}
