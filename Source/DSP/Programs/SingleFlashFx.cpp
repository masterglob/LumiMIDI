
// ============================================================================
// Source/DSP/SimpleStroboscope.cpp
// ============================================================================
#include <vector>

#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"
#include "UI/Resources/ColourPalette.h"

/**********************************************************************************/
namespace {

struct Context : public ProgramContext {
  Context(const LedVect& leds) {
    for (const LedContext* pLed : leds) {
      if (!pLed)
        continue;
      const LedPosition& pos(pLed->pos);
      int x(pos.center.getX());
      int y(pos.center.getY());
      if (x > maxX) {
        maxX = x;
      }
      if (y > maxY) {
        maxY = y;
      }
      if (x < minX) {
        minX = x;
      }
      if (y < minY) {
        minY = y;
      }
    }
    dx = (maxX - minX);
    dy = (maxY - minY);
    centerX = dx / 2;
    centerY = dy / 2;
    if (dx < 100)
      dx = 100;
    if (dy < 100)
      dy = 100;
  }

  int maxX{0};
  int minX{0x7FFFFFFF};
  int maxY{0};
  int minY{0x7FFFFFFF};
  int centerX{0};
  int dx{0};
  int centerY{0};
  int dy{0};
};

void execCommon(const LedVect& leds,
                const ParameterManager& parameterManager,
                BaseProgram::Events& events,
                std::unique_ptr<ProgramContext>& mContext,
                CCValue mVelocity,
                float peak,
                bool isY) {
  if (!mContext) {
    mContext.reset(new ::Context(leds));
  }
  ::Context& ctx(*reinterpret_cast<::Context*>(mContext.get()));
  /*Compute color from velocity :
   -vel =0 : black
   - val = 64 : FxHue
   - val = 127 : White
   */
  juce::Colour baseCol = parameterManager.getFx1HueColor();
  float rBase = baseCol.getFloatRed();
  float gBase = baseCol.getFloatGreen();
  float bBase = baseCol.getFloatBlue();

  float r, g, b, w;

  if (mVelocity <= 64) {
    // Interpolation noir -> baseCol
    float t = static_cast<float>(mVelocity) / 64.0f;  // 0..1
    r = 0.0f + t * (rBase - 0.0f);
    g = 0.0f + t * (gBase - 0.0f);
    b = 0.0f + t * (bBase - 0.0f);
    w = 0.0f;  // pas de blanc
  } else {
    // Interpolation baseCol -> blanc
    float t = static_cast<float>(mVelocity - 64) / (127.0f - 64);  // 0..1
    r = rBase + t * (1.0f - rBase);
    g = gBase + t * (1.0f - gBase);
    b = bBase + t * (1.0f - bBase);
    w = t;
  }

  auto addCumulativeEvent = [&](LineId line, LineValue value) {
    // Cherche si l'événement existe déjà
    auto it = std::find_if(
        events.begin(), events.end(),
        [line](const BaseProgram::Event& e) { return e.lineIdx == line; });

    if (it != events.end()) {
      // Cumuler et clamp si besoin (exemple pour MIDI 0..127)
      it->value = std::min<LineValue>(127, it->value + value);
    } else {
      // Ajouter un nouvel événement
      events.emplace_back(line, value);
    }
  };

  for (const LedContext* pLed : leds) {
    if (!pLed)
      continue;
    const LedCtrlLine& led(pLed->ctrl);
    const LedPosition& pos(pLed->pos);

    // fact =0 on left, 1 on Right
    if (isY) {
    }
    float posNorm = isY ? (float(pos.center.getY() - ctx.minY) / float(ctx.dy))
                        : (float(pos.center.getX() - ctx.minX) / float(ctx.dx));
    float dist = abs(posNorm - peak);
    static const float BELL_SIZE{0.25f};
    float fact(dist > BELL_SIZE ? 0.0f : 1.0f - dist / BELL_SIZE);

    addCumulativeEvent(led.mr, BaseProgram::float01ToCcValue(r * fact));
    addCumulativeEvent(led.mg, BaseProgram::float01ToCcValue(g * fact));
    addCumulativeEvent(led.mb, BaseProgram::float01ToCcValue(b * fact));
    addCumulativeEvent(led.mw, BaseProgram::float01ToCcValue(w * fact));
  }
}

}  // namespace

namespace PROGS {

/**********************************************************************************/
SingleFlashFxC::SingleFlashFxC() : BaseProgram("SingleFlash-Center") {}

/**********************************************************************************/
void SingleFlashFxC::reset() {}

/**********************************************************************************/
void SingleFlashFxC::execute(const LedVect& leds,
                             const ParameterManager& parameterManager,
                             BaseProgram::Events& events) {
  execCommon(leds, parameterManager, events, mContext, mVelocity, 0.5f, false);
}

/**********************************************************************************/
SingleFlashFxL::SingleFlashFxL() : BaseProgram("SingleFlash-Left") {}

/**********************************************************************************/
void SingleFlashFxL::reset() {}

/**********************************************************************************/
void SingleFlashFxL::execute(const LedVect& leds,
                             const ParameterManager& parameterManager,
                             BaseProgram::Events& events) {
  execCommon(leds, parameterManager, events, mContext, mVelocity, 0.0f, false);
}

/**********************************************************************************/
SingleFlashFxLM::SingleFlashFxLM() : BaseProgram("SingleFlash-LeftMid") {}

/**********************************************************************************/
void SingleFlashFxLM::reset() {}

/**********************************************************************************/
void SingleFlashFxLM::execute(const LedVect& leds,
                              const ParameterManager& parameterManager,
                              BaseProgram::Events& events) {
  execCommon(leds, parameterManager, events, mContext, mVelocity, 0.25f, false);
}

/**********************************************************************************/
SingleFlashFxR::SingleFlashFxR() : BaseProgram("SingleFlash-Right") {}

/**********************************************************************************/
void SingleFlashFxR::reset() {}

/**********************************************************************************/
void SingleFlashFxR::execute(const LedVect& leds,
                             const ParameterManager& parameterManager,
                             BaseProgram::Events& events) {
  execCommon(leds, parameterManager, events, mContext, mVelocity, 1.0f, false);
}

/**********************************************************************************/
SingleFlashFxRM::SingleFlashFxRM() : BaseProgram("SingleFlash-RightMid") {}

/**********************************************************************************/
void SingleFlashFxRM::reset() {}

/**********************************************************************************/
void SingleFlashFxRM::execute(const LedVect& leds,
                              const ParameterManager& parameterManager,
                              BaseProgram::Events& events) {
  execCommon(leds, parameterManager, events, mContext, mVelocity, 0.75f, false);
}

/**********************************************************************************/
SingleFlashFxTop::SingleFlashFxTop() : BaseProgram("SingleFlash-Top") {}

/**********************************************************************************/
void SingleFlashFxTop::reset() {}

/**********************************************************************************/
void SingleFlashFxTop::execute(const LedVect& leds,
                               const ParameterManager& parameterManager,
                               BaseProgram::Events& events) {
  execCommon(leds, parameterManager, events, mContext, mVelocity, 1.0f, true);
}

/**********************************************************************************/
SingleFlashFxBot::SingleFlashFxBot() : BaseProgram("SingleFlash-Bot") {}

/**********************************************************************************/
void SingleFlashFxBot::reset() {}

/**********************************************************************************/
void SingleFlashFxBot::execute(const LedVect& leds,
                               const ParameterManager& parameterManager,
                               BaseProgram::Events& events) {
  execCommon(leds, parameterManager, events, mContext, mVelocity, 0.0f, true);
}
}  // namespace PROGS
