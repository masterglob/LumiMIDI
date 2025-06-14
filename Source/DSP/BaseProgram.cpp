
// ============================================================================
// Source/DSP/AudioEngine.cpp
// ============================================================================
#include "AudioEngine.h"
#include "../Parameters/ParameterManager.h"
#include "../UI/Resources/ColourPalette.h"
#include "DSP/BaseProgram.h"

#include <vector>

namespace
{
    inline LineValue toCCValue(float v)
    {
        if (v < 0.0f) return 0.0f;
        if (v > 127.0) return 127;
        return TO_LINE_VALUE(v);
    }
}

DefaultProgram& DefaultProgram::instance(void)
{
    static DefaultProgram prg;
    return prg;
}

BaseProgram::Events DefaultProgram::execute(const LedVect& leds, const ParameterManager& parameterManager)
{
    Events result;
    result.reserve(256);

    static const float coef(127);

    const float mRed(parameterManager.getMainRed() * coef);
    const float mGreen(parameterManager.getMainGreen() * coef);
    const float mBlue(parameterManager.getMainBlue() * coef);
    const float mWhite(parameterManager.getMainWhite() * coef);
    // const float mHue(parameterManager.getMainHue());

    for (const Led_RGBW* pLed : leds)
    {
        if (!pLed) continue;
        const Led_RGBW& led(*pLed);
        result.emplace_back(led.mr, toCCValue(mRed));
        result.emplace_back(led.mg, toCCValue(mGreen));
        result.emplace_back(led.mb, toCCValue(mBlue));
        const float fw = (mBlue + mGreen + mRed) * mWhite * coef;
        result.emplace_back(led.mw, toCCValue(fw));
    }
    return result;
}
