
// ============================================================================
// Source/DSP/AudioEngine.cpp
// ============================================================================
#include "DSP/BaseProgram.h"

#include <vector>

#include "../Parameters/ParameterManager.h"
#include "../UI/Resources/ColourPalette.h"
#include "AudioEngine.h"

namespace {
inline LineValue toCCValue(float v) {
  if (v < 0.0f)
    return 0;
  if (v > 127.0)
    return 127;
  return TO_LINE_VALUE(v);
}
}  // namespace

/**********************************************************************************/
BaseProgram::BaseProgram(void)
    : startMillis(juce::Time::getMillisecondCounter()) {}

/**********************************************************************************/
void BaseProgram::reset() {
  startMillis = juce::Time::getMillisecondCounter();
  mDone = false;
}

/**********************************************************************************/
juce::uint32 BaseProgram::elapsedMs(void) const {
  return juce::Time::getMillisecondCounter() - startMillis;
}
/**********************************************************************************/
LineValue BaseProgram::floatToCcValue(float v) {
  if (v < 0.0f)
    return 0;
  if (v > 127.0)
    return 127;
  return TO_LINE_VALUE(v);
}

/**********************************************************************************/
juce::uint32 BaseProgram::floatToPeriod(float f) {
  static const juce::uint32 MIN_ms(50);
  static const juce::uint32 MAX_ms(1000);
  return MIN_ms + static_cast<juce::uint32>((1.0f - f) * (MAX_ms - MIN_ms));
}

namespace PROGS {

/**********************************************************************************/
DefaultProgram::DefaultProgram(void) {}

/**********************************************************************************/
void DefaultProgram::execute(const LedVect& leds,
                             const ParameterManager& parameterManager,
                             Events& events) {
  static const float coef(MAX_CC_VALUE_F);

  const float mRed(parameterManager.getMainRed() * coef);
  const float mGreen(parameterManager.getMainGreen() * coef);
  const float mBlue(parameterManager.getMainBlue() * coef);
  const float mWhite(parameterManager.getMainWhite() * coef);
  // const float mHue(parameterManager.getMainHue());

  for (const LedContext* pLed : leds) {
    if (!pLed)
      continue;
    const LedContext& led(*pLed);
    events.emplace_back(led.ctrl.mr, toCCValue(mRed));
    events.emplace_back(led.ctrl.mg, toCCValue(mGreen));
    events.emplace_back(led.ctrl.mb, toCCValue(mBlue));
    const float fw = (mBlue + mGreen + mRed) * mWhite * coef;
    events.emplace_back(led.ctrl.mw, toCCValue(fw));
  }
}

}  // namespace PROGS
