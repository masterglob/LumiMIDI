// ============================================================================
// Source/DSP/DAudioFilter.h
// ============================================================================
#pragma once

#include <juce_dsp/juce_dsp.h>

class DHysteresisTrigger {
 public:
  DHysteresisTrigger(float thresholdLowP, float thresholdHighP, int holdTimeSamplesP, float alphaP)
      : thresholdLow(thresholdLowP),
        thresholdHigh(thresholdHighP),
        holdTimeSamples(holdTimeSamplesP),
        alpha(alphaP),
        active(false),
        timerSamplesLeft(0),
        emaValue(0.0f) {}

  void reset() {
    active = false;
    timerSamplesLeft = 0;
    emaValue = 0.0f;
  }

  // Appeler � chaque processBlock ou �chantillon (numSamples dans le block)
  // Retourne la valeur de sortie (entre 0 et 1)
  float process(float mLowFreqLevel, int numSamples) {
    if (active) {
      timerSamplesLeft -= numSamples;
      if (timerSamplesLeft <= 0) {
        active = false;
        emaValue = 1.0f;  // d�marrage d�croissance � 1
      }
      return 1.0f;
    } else {
      if (mLowFreqLevel > thresholdHigh && !active && emaValue <= 0.0f) {
        active = true;
        timerSamplesLeft = holdTimeSamples;
        return 1.0f;
      } else {
        // EMA d�croissance de 1 vers 0
        emaValue = alpha * 0.0f + (1.0f - alpha) * emaValue;  // = emaValue * (1 - alpha)
        if (emaValue < 0.0001f)                               // clamp � 0 pour �viter flottements
          emaValue = 0.0f;

        // Reset trigger quand on redescend sous thresholdLow
        if (mLowFreqLevel < thresholdLow) {
          // On ne fait rien ici car on bloque la r�activation tant que mLowFreqLevel > thresholdLow
          // L'�tat "active" est d�j� false ici
        }

        return emaValue;
      }
    }
  }

 private:
  const float thresholdLow;
  const float thresholdHigh;
  const int holdTimeSamples;
  const float alpha;

  bool active;
  int timerSamplesLeft;
  float emaValue;
};