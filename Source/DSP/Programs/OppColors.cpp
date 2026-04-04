// =============================================================================
// Source/DSP/Programs/OppColors.cpp
// =============================================================================
#include <cmath>
#include <vector>

#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"

/**********************************************************************************/
namespace {
using namespace juce;

struct Context : public ProgramContext {
  Context() : mCenter(0.0f) { Random random; }

  void init(const LedVect& leds, bool isH);
  float mCenter;
  bool even{false};
  bool isLedFar(const LedContext* ctx);

  void execute(const LedVect& leds,
               const ParameterManager& parameterManager,
               BaseProgram::Events& events);
  uint32 cycleStart{0};

 private:
  bool isInitialized{false};
  std::map<const LedContext*, bool> isFar;
};

bool Context::isLedFar(const LedContext* ctx) {
  //
  const auto it{isFar.find(ctx)};
  if (it == isFar.end())
    return false;
  return it->second;
}

void Context::init(const LedVect& leds, bool isH) {
  if (isInitialized || leds.empty())
    return;
  //

  float sum = 0.0f;
  float deltaMax = 0.0f;
  if (isH) {
    for (const auto& led : leds) {
      const LedPosition& pos(led->pos);
      sum += pos.center.getX();
    }
    mCenter = sum / static_cast<float>(leds.size());

    for (const auto& led : leds) {
      const LedPosition& pos(led->pos);
      const float delta = fabs(mCenter - pos.center.getX());
      if (delta > deltaMax)
        deltaMax = delta;
    }

    for (const auto& led : leds) {
      const LedPosition& pos(led->pos);
      const float delta = fabs(mCenter - pos.center.getX());
      isFar[led] = delta > deltaMax / 2;
    }
  } else {
    for (const auto& led : leds) {
      const LedPosition& pos(led->pos);
      sum += pos.center.getY();
    }
    mCenter = sum / static_cast<float>(leds.size());

    for (const auto& led : leds) {
      const LedPosition& pos(led->pos);
      const float delta = fabs(mCenter - pos.center.getY());
      if (delta > deltaMax)
        deltaMax = delta;
    }

    for (const auto& led : leds) {
      const LedPosition& pos(led->pos);
      const float delta = fabs(mCenter - pos.center.getY());
      isFar[led] = delta > deltaMax / 2;
    }
  }

  isInitialized = true;
}

void Context::execute(const LedVect& leds,
                      const ParameterManager& parameterManager,
                      BaseProgram::Events& events) {
  const float white = parameterManager.getMainWhite();
  const float phase = parameterManager.getPhase();
  const float mainHue = parameterManager.getMainHue();
  const float sat = parameterManager.getMainSat();
  const float intensity = 0.8f;

  for (const auto& led : leds) {
    float hue = (float)(mainHue * 360.0f);
    if (isLedFar(led)) {
      hue += phase * 360.0f;
    }

    hue = std::fmod(hue, 360.0f);
    if (hue < 0.0f)
      hue += 360.0f;

    const Colour rgbColor = Colour::fromHSV(hue / 360.0f, sat, sat, sat);
    // Apply instensity
    events.emplace_back(
        led->ctrl.mr, FLOAT_TO_LINE_VALUE(rgbColor.getFloatRed() * intensity));
    events.emplace_back(
        led->ctrl.mg,
        FLOAT_TO_LINE_VALUE(rgbColor.getFloatGreen() * intensity));
    events.emplace_back(
        led->ctrl.mb, FLOAT_TO_LINE_VALUE(rgbColor.getFloatBlue() * intensity));
    events.emplace_back(led->ctrl.mw, FLOAT_TO_LINE_VALUE(white));
  }
}

static const float pi{3.1415926f};
}  // namespace

/******************************************************************/
namespace PROGS {

OppColorsH::OppColorsH() : BaseProgram("OppColors-H") {}

void OppColorsH::reset() {
  mContext.reset(new ::Context());
}

void OppColorsH::execute(const LedVect& leds,
                         const ParameterManager& parameterManager,
                         BaseProgram::Events& events) {
  if (!mContext) {
    mContext.reset(new ::Context());
  }

  ::Context& ctx(*reinterpret_cast<::Context*>(mContext.get()));
  ctx.init(leds, true);
  ctx.execute(leds, parameterManager, events);
}

OppColorsV::OppColorsV() : BaseProgram("OppColors-V") {}

void OppColorsV::reset() {
  mContext.reset(new ::Context());
}

void OppColorsV::execute(const LedVect& leds,
                         const ParameterManager& parameterManager,
                         BaseProgram::Events& events) {
  if (!mContext) {
    mContext.reset(new ::Context());
  }

  ::Context& ctx(*reinterpret_cast<::Context*>(mContext.get()));
  ctx.init(leds, false);
  ctx.execute(leds, parameterManager, events);
}

}  // namespace PROGS
