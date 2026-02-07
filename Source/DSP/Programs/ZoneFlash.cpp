// =============================================================================
// Source/DSP/Programs/ZoneFlash.cpp
// =============================================================================
#include <cmath>
#include <vector>

#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"

/**********************************************************************************/
namespace {
using namespace juce;

struct Context : public ProgramContext {
  Context() : centerX(0.0f), isInitialized(false) { Random random; }

  float centerX;
  bool isInitialized;
  bool even{false};
  uint32 cycleStart{0};
};

static const float pi{3.1415926f};
}  // namespace

/******************************************************************/
namespace PROGS {

ZoneFlash::ZoneFlash() : BaseProgram("ZoneFlash") {
}

void ZoneFlash::reset() {
  mContext.reset(new ::Context());
}

void ZoneFlash::execute(const LedVect& leds,
                        const ParameterManager& parameterManager,
                        BaseProgram::Events& events) {
  if (!mContext) {
    mContext.reset(new ::Context());
  }

  ::Context& ctx(*reinterpret_cast<::Context*>(mContext.get()));

  const float phase = parameterManager.getPhase();
  // const float mainHue = parameterManager.getMainHue();
  const float intensity1 = 0.8f;
  const float intensity2 = 0.1f;

  // Calcul du centre X si premi�re ex�cution
  if (!ctx.isInitialized && !leds.empty()) {
    float sumX = 0.0f;
    // Color params
    for (const auto& led : leds) {
      const LedPosition& pos(led->pos);
      sumX += pos.center.getX();
    }
    ctx.centerX = sumX / static_cast<float>(leds.size());
    ctx.isInitialized = true;
  }

  // time parameters
  const uint32 periodMs(floatToPeriod(parameterManager.getSpeed()));
  const uint32 dtMs = elapsedMs();
  float tCycle = ((float) dtMs) / periodMs;
  bool isEven = (static_cast<int>(tCycle) % 2) == 0;

  if (isEven != ctx.even) {
    ctx.even = isEven;
    ctx.cycleStart = dtMs;
    DBG("News cycle");
    // New cycle
  }
  uint32 flashCycle = dtMs - ctx.cycleStart;
  float whiteLevel = 200.0f - flashCycle;
  const float flashIntensity = (whiteLevel > 0) ? whiteLevel * 0.01f : 0.0f;

  for (const auto& led : leds) {
    const LedPosition& pos(led->pos);
    const bool isLeftZone = (pos.center.getX() < ctx.centerX);
    const bool shouldFlash = (isLeftZone == ctx.even);

    // RGB continuuous area with changing color
    float zoneHue = (float) ((dtMs / 50) % 360);
    if (isLeftZone) {
      zoneHue += phase * 180.0f + 360.0f;
    }

    // Normalisation de la teinte (0-360�)
    while (zoneHue >= 360.0f) zoneHue -= 360.0f;

    // Conversion HSV to RGB
    const Colour rgbColor = Colour::fromHSV(zoneHue / 360.0f, 1.0f, 1.0f, 1.0f);

    const float intensity = shouldFlash ? intensity2 : intensity1;
    if (led->ctrl.hasWhite) {
      if (shouldFlash) {
        events.emplace_back(led->ctrl.mw, FLOAT_TO_LINE_VALUE(flashIntensity));
      } else {
        events.emplace_back(led->ctrl.mw, FLOAT_TO_LINE_VALUE(0.0f));
      }
    }

    // Apply instensity
    events.emplace_back(led->ctrl.mr, FLOAT_TO_LINE_VALUE(rgbColor.getFloatRed() * intensity));
    events.emplace_back(led->ctrl.mg, FLOAT_TO_LINE_VALUE(rgbColor.getFloatGreen() * intensity));
    events.emplace_back(led->ctrl.mb, FLOAT_TO_LINE_VALUE(rgbColor.getFloatBlue() * intensity));
  }
}

}  // namespace PROGS
