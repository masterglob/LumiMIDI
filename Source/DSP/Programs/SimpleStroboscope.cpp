
// ============================================================================
// Source/DSP/SimpleStroboscope.cpp
// ============================================================================
#include "DSP/Programs/SimpleStroboscope.h"
#include "Parameters/ParameterManager.h"
#include "UI/Resources/ColourPalette.h"

#include <vector>

namespace
{
    inline LineValue toCCValue(float v)
    {
        if (v < 0.0f) return 0;
        if (v > 127.0) return 127;
        return TO_LINE_VALUE(v);
    }

    inline juce::uint32 floatToPeriod(float f)
    {
        static const juce::uint32 MIN_ms(50);
        static const juce::uint32 MAX_ms(1000);
        return MIN_ms + static_cast<juce::uint32>((1.0f - f) * (MAX_ms - MIN_ms));
    }
}

namespace PROGS
{
    SimpleStroboscope::SimpleStroboscope() :
        startMillis(juce::Time::getMillisecondCounter())
    {
    }

    void SimpleStroboscope::execute(const LedVect& leds, const ParameterManager& parameterManager, BaseProgram::Events& events)
    {
        static const float coef(127);
        const juce::uint32 periodMs(floatToPeriod(parameterManager.getSpeed()));

        juce::uint32 dtMs = juce::Time::getMillisecondCounter() - startMillis;

        if ((dtMs % periodMs) < (periodMs / 2))
        {
            DBG("dtMs=" << (int)dtMs << "(on) , mod="<< (int)(dtMs % periodMs) << ", per= "<< (int)periodMs);
            // TODO use The global color normilized?
            static const float mRed(coef);
            static const float mGreen(coef);
            static const float mBlue(coef);
            static const float mWhite(coef);
            for (const Led_RGBW* pLed : leds)
            {
                if (!pLed) continue;
                const Led_RGBW& led(*pLed);
                events.emplace_back(led.mr, toCCValue(mRed));
                events.emplace_back(led.mg, toCCValue(mGreen));
                events.emplace_back(led.mb, toCCValue(mBlue));
                events.emplace_back(led.mw, toCCValue(mWhite));
            }
        }
        else
        {
            DBG("dtMs=" << (int)dtMs << "(off) , mod=" << (int)(dtMs % periodMs) << ", per= " << (int)periodMs);
            const float mRed(parameterManager.getMainRed() * coef);
            const float mGreen(parameterManager.getMainGreen() * coef);
            const float mBlue(parameterManager.getMainBlue() * coef);
            const float mWhite(parameterManager.getMainWhite() * coef);

            for (const Led_RGBW* pLed : leds)
            {
                if (!pLed) continue;
                const Led_RGBW& led(*pLed);
                events.emplace_back(led.mr, toCCValue(mRed));
                events.emplace_back(led.mg, toCCValue(mGreen));
                events.emplace_back(led.mb, toCCValue(mBlue));
                const float fw = (mBlue + mGreen + mRed) * mWhite * coef;
                events.emplace_back(led.mw, toCCValue(fw));
            }
        }
    }
}