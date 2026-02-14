// =============================================================================
// Source/DSP/Programs/WarmCoolCycle.cpp
// =============================================================================

/* Oscillation between warm and cool versions of main color.
 Temperature shift on color wheel while maintaining hue family.*/
#include <cmath>

#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"

/**********************************************************************************/
namespace {

const float temperatureRange{50.0f / 360.0f};
static const float speedFactor = 8.0f;

juce::Colour oscillateColorTemperature(const juce::Colour& mainCol, float phase, float speed) {
  // Extraire la teinte (hue), saturation et luminosit� de la couleur principale
  float hue = mainCol.getHue();                // Hue  [0.0, 1.0]
  float saturation = mainCol.getSaturation();  // Saturation  [0.0, 1.0]
  float brightness = mainCol.getBrightness();  // Brightness  [0.0, 1.0]

  // Calculer la variation de la teinte avec la phase et la vitesse
  float oscillationAmount = sin(phase * speed * speedFactor) * temperatureRange;

  // Appliquer cette oscillation � la teinte, tout en maintenant le reste
  // constant
  float newHue = hue + oscillationAmount;

  // Assurez-vous que la teinte reste dans la plage [0.0, 1.0]
  if (newHue < 0.0f) newHue += 1.0f;
  if (newHue > 1.0f) newHue -= 1.0f;

  // Retourner la nouvelle couleur avec la teinte ajust�e
  return juce::Colour::fromHSV(newHue, saturation, brightness, 1.0f);
}

const float twoPi = 2.0f * juce::MathConstants<float>::pi;
}  // namespace

/**********************************************************************************/
namespace PROGS {

WarmCoolCycle::WarmCoolCycle() : BaseProgram("WarmCoolCycle") {
}

void WarmCoolCycle::reset() {
}

void WarmCoolCycle::execute(const LedVect& leds,
                            const ParameterManager& parameterManager,
                            BaseProgram::Events& events) {
  // Get parameters
  const juce::Colour mainCol = parameterManager.getMainHueColor();
  const float speed = parameterManager.getSpeed();

  // Calculate breathing intensity using sine wave
  const float timeMs = static_cast<float>(elapsedMs());
  const float period = floatToPeriod(speed) * speedFactor;  // Convert speed to period in ms
  const float phase((timeMs / period) * twoPi);

  juce::Colour newCol = oscillateColorTemperature(mainCol, phase, speed);

  // Apply to all LEDs (global effect)
  for (const auto& led : leds) {
    events.emplace_back(led->ctrl.mr, FLOAT_TO_LINE_VALUE(newCol.getFloatRed()));
    events.emplace_back(led->ctrl.mg, FLOAT_TO_LINE_VALUE(newCol.getFloatGreen()));
    events.emplace_back(led->ctrl.mb, FLOAT_TO_LINE_VALUE(newCol.getFloatBlue()));
    if (led->ctrl.hasWhite) {
      events.emplace_back(led->ctrl.mw, FLOAT_TO_LINE_VALUE(0.0f));
    }
  }

  // Breathing effect never stops automatically
  mDone = false;
}

}  // namespace PROGS
