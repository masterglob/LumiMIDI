// ============================================================================
// Source/DSP/DAudioFilter.h
// ============================================================================
#pragma once

#include <juce_dsp/juce_dsp.h>

class DHysteresisTrigger
{
public:
    DHysteresisTrigger(float thresholdLow,
        float thresholdHigh,
        int holdTimeSamples,
        float alpha)
        : thresholdLow(thresholdLow),
        thresholdHigh(thresholdHigh),
        holdTimeSamples(holdTimeSamples),
        alpha(alpha),
        active(false),
        timerSamplesLeft(0),
        emaValue(0.0f)
    {
    }

    void reset()
    {
        active = false;
        timerSamplesLeft = 0;
        emaValue = 0.0f;
    }

    // Appeler à chaque processBlock ou échantillon (numSamples dans le block)
    // Retourne la valeur de sortie (entre 0 et 1)
    float process(float mLowFreqLevel, int numSamples)
    {
        if (active)
        {
            timerSamplesLeft -= numSamples;
            if (timerSamplesLeft <= 0)
            {
                active = false;
                emaValue = 1.0f; // démarrage décroissance à 1
            }
            return 1.0f;
        }
        else
        {
            if (mLowFreqLevel > thresholdHigh && !active && emaValue <= 0.0f)
            {
                active = true;
                timerSamplesLeft = holdTimeSamples;
                return 1.0f;
            }
            else
            {
                // EMA décroissance de 1 vers 0
                emaValue = alpha * 0.0f + (1.0f - alpha) * emaValue; // = emaValue * (1 - alpha)
                if (emaValue < 0.0001f) // clamp à 0 pour éviter flottements
                    emaValue = 0.0f;

                // Reset trigger quand on redescend sous thresholdLow
                if (mLowFreqLevel < thresholdLow)
                {
                    // On ne fait rien ici car on bloque la réactivation tant que mLowFreqLevel > thresholdLow
                    // L'état "active" est déjà false ici
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