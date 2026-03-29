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
  Context() : centerX(0.0f) { Random random; }

  void init(const LedVect& leds);
  float centerX;
  bool even{false};
  bool isLedFar(const LedContext* ctx);
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

void Context::init(const LedVect& leds) {
  if (isInitialized || leds.empty())
    return;
  //

  float sumX = 0.0f;
  for (const auto& led : leds) {
    const LedPosition& pos(led->pos);
    sumX += pos.center.getX();
  }
  centerX = sumX / static_cast<float>(leds.size());
  DBG("centerX:  = " << centerX);

  float deltaXMax = 0.0f;
  for (const auto& led : leds) {
    const LedPosition& pos(led->pos);
    const float dx = fabs(centerX - pos.center.getX());
    DBG("Dx:  = " << dx);
    if (dx > deltaXMax)
      deltaXMax = dx;
  }
  DBG("deltaXMax:  = " << deltaXMax);

  for (const auto& led : leds) {
    const LedPosition& pos(led->pos);
    const float dx = fabs(centerX - pos.center.getX());
    DBG("LED " << led->name << ", dx= " << dx
               << " is far=" << int(dx > deltaXMax / 2));
    isFar[led] = dx > deltaXMax / 2;
  }

  isInitialized = true;
}

static const float pi{3.1415926f};
}  // namespace

/******************************************************************/
namespace PROGS {

OppColors::OppColors() : BaseProgram("OppColors") {}

void OppColors::reset() {
  mContext.reset(new ::Context());
}

void OppColors::execute(const LedVect& leds,
                        const ParameterManager& parameterManager,
                        BaseProgram::Events& events) {
  if (!mContext) {
    mContext.reset(new ::Context());
  }

  ::Context& ctx(*reinterpret_cast<::Context*>(mContext.get()));
  ctx.init(leds);

  const float white = parameterManager.getMainWhite();
  const float phase = parameterManager.getPhase();
  const float mainHue = parameterManager.getMainHue();
  const float sat = parameterManager.getMainSat();
  const float intensity = 0.8f;

  for (const auto& led : leds) {
    float hue = (float)(mainHue * 360.0f);
    if (ctx.isLedFar(led)) {
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

}  // namespace PROGS
