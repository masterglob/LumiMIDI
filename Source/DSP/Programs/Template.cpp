// =============================================================================
// Source/DSP/Programs/Template.cpp
// =============================================================================

#include <cmath>
#include <vector>

#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"


/**********************************************************************************/
namespace {

/* This is an example of how contexts must be implemented.It may contain
 * some random-generated data for example, which will be applied each time the
 * program is triggered */
struct Context : public ProgramContext {
  Context() { mainColor = juce::Colour(10, 100, 230); }
  juce::Colour mainColor;
};
}  // namespace

/******************************************************************/
namespace PROGS {

Template::Template() : BaseProgram("Template") {
}

void Template::reset() {
  mContext.reset(new ::Context());
}

void Template::execute(const LedVect& leds,
                       const ParameterManager& parameterManager,
                       BaseProgram::Events& events) {
  (void) parameterManager;
  if (!mContext) {
    mContext.reset(new ::Context());
  }

  using namespace juce;
  // use program context:
  ::Context& ctx(*reinterpret_cast<::Context*>(mContext.get()));
  const Colour col(ctx.mainColor);

  // Can get some global parameters:
  // const uint32 periodMs(floatToPeriod(parameterManager.getSpeed()));

  /* Note : existing parameters in parameterManager:
  inline void setLowRms(float rms) { mLowRms = rms; }
  inline float getLowRms() const { return mLowRms; }
  // Main parameters:
  float getMainRed() const;
  float getMainGreen() const;
  float getMainBlue() const;
  float getMainWhite() const;
  float getMainHue() const;
  float getSpeed() const;
  float getPhase() const;
  */

  for (const auto& led : leds) {
    // FLOAT_TO_LINE_VALUE converts a 0..1 float value to controller range (int)

    // Apply RGB to outputs:
    events.emplace_back(led->ctrl.mr, FLOAT_TO_LINE_VALUE(col.getFloatRed()));
    events.emplace_back(led->ctrl.mg, FLOAT_TO_LINE_VALUE(col.getFloatGreen()));
    events.emplace_back(led->ctrl.mb, FLOAT_TO_LINE_VALUE(col.getFloatBlue()));

    if (led->ctrl.hasWhite) {
      // No White component in the program
      events.emplace_back(led->ctrl.mw, MIN_CC_VALUE);
    }
  }
}

}  // namespace PROGS
