
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
        if (v < 0.0f) return 0;
        if (v > 127.0) return 127;
        return TO_LINE_VALUE(v);
    }
}

DefaultProgram& DefaultProgram::instance(void)
{
    static DefaultProgram prg;
    return prg;
}

void DefaultProgram::execute(const LedVect& leds, const ParameterManager& parameterManager, Events& events)
{
    static const float coef(127);

    const float mRed(parameterManager.getMainRed() * coef);
    const float mGreen(parameterManager.getMainGreen() * coef);
    const float mBlue(parameterManager.getMainBlue() * coef);
    const float mWhite(parameterManager.getMainWhite() * coef);
    // const float mHue(parameterManager.getMainHue());

    for (const LedContext* pLed : leds)
    {
        if (!pLed) continue;
        const LedContext& led(*pLed);
        events.emplace_back(led.ctrl.mr, toCCValue(mRed));
        events.emplace_back(led.ctrl.mg, toCCValue(mGreen));
        events.emplace_back(led.ctrl.mb, toCCValue(mBlue));
        const float fw = (mBlue + mGreen + mRed) * mWhite * coef;
        events.emplace_back(led.ctrl.mw, toCCValue(fw));
    }
}
