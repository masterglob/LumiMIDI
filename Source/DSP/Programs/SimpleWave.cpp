
// ============================================================================
// Source/DSP/SimpleStroboscope.cpp
// ============================================================================
#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"
#include "UI/Resources/ColourPalette.h"

#include <vector>

// TODO : Start point (left or right)
// TODO : support Y also ?

namespace PROGS {

/**********************************************************************************/
SimpleWave::SimpleWave() {}

/**********************************************************************************/
void SimpleWave::execute(const LedVect& leds,
                         const ParameterManager& parameterManager,
                         BaseProgram::Events& events) {
  juce::uint32 periodMs(floatToPeriod(parameterManager.getSpeed()));
  const float mRed(parameterManager.getMainRed() * MAX_CC_VALUE_F);
  const float mGreen(parameterManager.getMainGreen() * MAX_CC_VALUE_F);
  const float mBlue(parameterManager.getMainBlue() * MAX_CC_VALUE_F);
  if (periodMs < 10)
    periodMs = 10;

  juce::uint32 dtMs = elapsedMs();

  int maxX(0);
  int minX(0x7FFFFFFF);
  int maxY(0);
  int minY(0x7FFFFFFF);

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
  int dx = (maxX - minX);
  if (dx < 100)
    dx = 100;
  for (const LedContext* pLed : leds) {
    if (!pLed)
      continue;
    const LedCtrlLine& led(pLed->ctrl);
    const LedPosition& pos(pLed->pos);

    /*  Apply a coef on White color as a bell curve centered on a point which
     * moves on sinusoidal curve. t=0 => bell is centered at minX t=periodMs =>
     * bell is centered at maxX
     *
     * COS (dtMs *2PI /periodMs)  = 1 => center
     */

    static const float pi{3.1415926f};

    mDone = dtMs > periodMs;

    /* t=0 => alpha =1 => bellCenterX = minX
     * t = periodMs/2 => alpha = -1 => bellCenterX = maxX
     * etc
     */
    float alpha(cos((dtMs * 2 * pi) / periodMs));
    // F2 in [0 1]
    float f2(0.5f * (alpha + 1.0f));
    int bellCenterX{static_cast<int>(minX + f2 * dx)};
    static const int bellSize(dx / 8);
    /*DBG("dtMs=" << dtMs << ", periodMs=" << periodMs << ", alpha=" << alpha
                << ", f2=" << f2 << alpha << ", bellCenterX=" << bellCenterX
                << ", bellSize=" << bellSize);*/
    float waveVal(0.0f);

    const int dxBell(abs(pos.center.getX() - bellCenterX));
    if (dxBell < bellSize) {
      waveVal = MAX_CC_VALUE_F;
    } else if (dxBell < 1.5f * bellSize) {
      waveVal = 0.5f * MAX_CC_VALUE_F;
    }
    //
    events.emplace_back(led.mw, floatToCcValue(waveVal));
    events.emplace_back(led.mr,
                        floatToCcValue(waveVal > mRed ? waveVal : mRed));
    events.emplace_back(led.mg,
                        floatToCcValue(waveVal > mGreen ? waveVal : mGreen));
    events.emplace_back(led.mb,
                        floatToCcValue(waveVal > mBlue ? waveVal : mBlue));
  }
}
}  // namespace PROGS
