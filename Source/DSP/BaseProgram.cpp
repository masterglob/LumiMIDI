
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
  if (v < 0.0f) return 0;
  if (v > 127.0) return 127;
  return TO_LINE_VALUE(v);
}

struct Context : public ProgramContext {
  Context() {}
  bool rmsActive{false};
  float rmsMaxVal{0.0f};
  juce::uint32 startTime{0};
};
}  // namespace

/**********************************************************************************/
BaseProgram::BaseProgram(const std::string& name)
    : startMillis(juce::Time::getMillisecondCounter()), name(name) {
}

/**********************************************************************************/
void BaseProgram::reset(CCValue velocity) {
  startMillis = juce::Time::getMillisecondCounter();
  mDone = false;
  mVelocity = velocity;
  reset();
}

/**********************************************************************************/
juce::uint32 BaseProgram::elapsedMs(void) const {
  return juce::Time::getMillisecondCounter() - startMillis;
}

/**********************************************************************************/
LineValue BaseProgram::float01ToCcValue(float v) {
  return floatToCcValue(v * MAX_CC_VALUE_F);
}

/**********************************************************************************/
LineValue BaseProgram::floatToCcValue(float v) {
  if (v < 0.0f) return 0;
  if (v > 127.0) return 127;
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
DefaultProgram::DefaultProgram() : BaseProgram("DefaultProgram") {
}

/**********************************************************************************/
void DefaultProgram::reset() {
  mContext.reset(new ::Context());
}

/**********************************************************************************/
void DefaultProgram::execute(const LedVect& leds, const ParameterManager& parameterManager, Events& events) {
  if (!mContext) {
    mContext.reset(new ::Context());
  }
  ::Context& ctx(*reinterpret_cast<::Context*>(mContext.get()));

  const juce::Colour colour = parameterManager.getHueColor();

  const float mainRed = colour.getFloatRed();
  const float mainGreen = colour.getFloatGreen();
  const float mainBlue = colour.getFloatBlue();

  float rms(parameterManager.getLowRms());
  static const float RMS_THR{1.0f};        // TODO : make a param?
  static const float RMS_REAL_MS{150.0f};  // TODO : make a param?

  const auto now{juce::Time::getMillisecondCounter()};

  if (ctx.rmsActive) {
    if (now - ctx.startTime > RMS_REAL_MS) {
      // End rlease
      ctx.rmsActive = false;
      rms = 0;
      ctx.rmsMaxVal = 0.0f;
      DBG("TRIG Release ");
    } else {
      // Start release

      const float ratio(1.0f - ((now - ctx.startTime) / RMS_REAL_MS));
      rms = juce::jlimit(0.0f, 1.0f, ctx.rmsMaxVal * ratio);
    }

  } else if (rms > RMS_THR) {
    DBG("TRIG WHITE " << rms);
    // Trig it!
    ctx.rmsActive = true;
    ctx.startTime = now;
    ctx.rmsMaxVal = parameterManager.getMainWhite();
    rms = ctx.rmsMaxVal;
  } else {
    rms = 0.0f;
  }

  for (const LedContext* pLed : leds) {
    if (!pLed) continue;
    const LedContext& led(*pLed);
    events.emplace_back(led.ctrl.mr, float01ToCcValue(mainRed));
    events.emplace_back(led.ctrl.mg, float01ToCcValue(mainGreen));
    events.emplace_back(led.ctrl.mb, float01ToCcValue(mainBlue));
    events.emplace_back(led.ctrl.mw, float01ToCcValue(rms));  // TODO move "White trig to some FX!"
  }
}

}  // namespace PROGS
