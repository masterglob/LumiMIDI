
// ============================================================================
// Source/DSP/SimpleStroboscope.cpp
// ============================================================================
#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"
#include "UI/Resources/ColourPalette.h"

#define WHITEONLY 1

namespace PROGS {

/**********************************************************************************/
SimpleStroboscope::SimpleStroboscope() : BaseProgram("SimpleStroboscope") {}

/**********************************************************************************/
void SimpleStroboscope::reset() {}

/**********************************************************************************/
void SimpleStroboscope::execute(const LedVect& leds,
                                const ParameterManager& parameterManager,
                                BaseProgram::Events& events) {
  const juce::uint32 periodMs(floatToPeriod(parameterManager.getSpeed()));

  juce::uint32 dtMs = elapsedMs();

  if ((dtMs % periodMs) < (periodMs / 2)) {
    // TODO use The global color normilized?
#if !WHITEONLY
    static const float mRed(coef);
    static const float mGreen(coef);
    static const float mBlue(coef);
#endif
    static const float mWhite(mVelocity);
    for (const LedContext* pLed : leds) {
      if (!pLed)
        continue;
      const LedCtrlLine& led(pLed->ctrl);
#if !WHITEONLY
      events.emplace_back(led.mr, floatToCcValue(mRed));
      events.emplace_back(led.mg, floatToCcValue(mGreen));
      events.emplace_back(led.mb, floatToCcValue(mBlue));
#endif
      events.emplace_back(led.mw, floatToCcValue(mWhite));
    }
  }
}
}  // namespace PROGS
