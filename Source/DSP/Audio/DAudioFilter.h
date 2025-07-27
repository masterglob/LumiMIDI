#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

class BandePassFilter
{
public:
    // Constructeur avec paramètres de fréquence et de Q
    BandePassFilter(float frequency, float Q);

    // Méthodes surchargées de AudioProcessor
    void prepareToPlay(double sampleRate, int samplesPerBlock, int nbChannels);
    void releaseResources() ;
    void processBlock(juce::AudioBuffer<float>& buffer);

    float getRms()const
    {
        return rms;
    }
private:
    float cutoffFrequency; // Fréquence de coupure
    float resonance; // Facteur Q (résonance)

    juce::dsp::StateVariableTPTFilter<float> bandPassFilter; // Le filtre passe-bande
    float rms{ 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandePassFilter)
};
