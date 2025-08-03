#pragma once
#include "BaseTexture.h"
#include "BaseNoise.h"


namespace RTW
{
    class BaseNoiseTexture : public BaseTexture
    {
    protected:
        std::shared_ptr<BaseNoise> m_Noise;
    };
}
