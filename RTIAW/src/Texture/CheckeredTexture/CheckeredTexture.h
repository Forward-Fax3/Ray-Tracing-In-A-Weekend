#pragma once
#include "Core.h"
#include "BaseTexture.h"

#include <memory>


namespace RTW
{
    class CheckeredTexture : public BaseTexture
    {
    public:
        CheckeredTexture(std::shared_ptr<BaseTexture> even, std::shared_ptr<BaseTexture> odd, double scale = 1.0);
        CheckeredTexture(const Colour& even, const Colour& odd, double scale = 1.0);

        Colour GetColour(const UV& uv, const Point& point) const override;

    private:
        std::shared_ptr<BaseTexture> m_Even;
        std::shared_ptr<BaseTexture> m_Odd;
        double m_InvScale;
    };
}
