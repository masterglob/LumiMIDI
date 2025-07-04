
// ============================================================================
// Source/DSP/SimpleStroboscope.cpp
// ============================================================================
#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"
#include "UI/Resources/ColourPalette.h"

namespace PROGS {

/**********************************************************************************/
SimpleStroboscope::SimpleStroboscope() {}

/**********************************************************************************/
void SimpleStroboscope::execute(const LedVect& leds,
                                const ParameterManager& parameterManager,
                                BaseProgram::Events& events) {
  static const float coef(MAX_CC_VALUE_F);
  const juce::uint32 periodMs(floatToPeriod(parameterManager.getSpeed()));

  juce::uint32 dtMs = elapsedMs();

  if ((dtMs % periodMs) < (periodMs / 2)) {
    DBG("dtMs=" << (int)dtMs << "(on) , mod=" << (int)(dtMs % periodMs)
                << ", per= " << (int)periodMs);
    // TODO use The global color normilized?
    static const float mRed(coef);
    static const float mGreen(coef);
    static const float mBlue(coef);
    static const float mWhite(coef);
    for (const LedContext* pLed : leds) {
      if (!pLed)
        continue;
      const LedCtrlLine& led(pLed->ctrl);
      events.emplace_back(led.mr, floatToCcValue(mRed));
      events.emplace_back(led.mg, floatToCcValue(mGreen));
      events.emplace_back(led.mb, floatToCcValue(mBlue));
      events.emplace_back(led.mw, floatToCcValue(mWhite));
    }
  } else {
    DBG("dtMs=" << (int)dtMs << "(off) , mod=" << (int)(dtMs % periodMs)
                << ", per= " << (int)periodMs);
    const float mRed(parameterManager.getMainRed() * coef);
    const float mGreen(parameterManager.getMainGreen() * coef);
    const float mBlue(parameterManager.getMainBlue() * coef);
    const float mWhite(parameterManager.getMainWhite() * coef);

    for (const LedContext* pLed : leds) {
      if (!pLed)
        continue;
      const LedCtrlLine& led(pLed->ctrl);
      events.emplace_back(led.mr, floatToCcValue(mRed));
      events.emplace_back(led.mg, floatToCcValue(mGreen));
      events.emplace_back(led.mb, floatToCcValue(mBlue));
      const float fw = (mBlue + mGreen + mRed) * mWhite * coef;
      events.emplace_back(led.mw, floatToCcValue(fw));
    }
  }
}
}  // namespace PROGS
