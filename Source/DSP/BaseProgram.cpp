
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
    const float mRed(parameterManager.getMainRed());
    const float mGreen(parameterManager.getMainGreen());
    const float mBlue(parameterManager.getMainBlue());
    const float mWhite(parameterManager.getMainWhite());
    // const float mHue(parameterManager.getMainHue());


    for (const Led_RGBW* pLed : leds)
    {
        if (!pLed) continue;
        const Led_RGBW& led(*pLed);
        result.emplace_back(led.mr, static_cast<unsigned char> (mRed * coef));
        result.emplace_back(led.mg, static_cast<unsigned char> (mGreen * coef));
        result.emplace_back(led.mb, static_cast<unsigned char> (mBlue * coef));
        const float fw = (mBlue + mGreen + mRed) * mWhite * coef;
        result.emplace_back(led.mw, static_cast<unsigned char> (fw));
    }
    return result;
}
