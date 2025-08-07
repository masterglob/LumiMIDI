#include "DAudioFilter.h"

BandePassFilter::BandePassFilter(float frequency, float Q)
    : cutoffFrequency(frequency), resonance(Q)
{
}

void BandePassFilter::prepareToPlay(double sampleRate, int samplesPerBlock, int nbChannels)
{
    // Initialisation du filtre avec les spécifications
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = nbChannels;

    bandPassFilter.prepare(spec);

    // Configurer les paramètres du filtre
    bandPassFilter.setCutoffFrequency(cutoffFrequency);
    bandPassFilter.setResonance(resonance);
}

void BandePassFilter::releaseResources()
{
    bandPassFilter.reset();
}
float toLogScale(float rmsValue)
{
    // Ajouter un petit epsilon pour éviter log(0)
    const float epsilon = 1e-6f;

    // Calculer le logarithme de la valeur
    return std::log(rmsValue + epsilon) / std::log(1.0f + epsilon);
}
void BandePassFilter::processBlock(juce::AudioBuffer<float>& buffer)
{
    // Appliquer le filtre sur chaque échantillon du buffer
    juce::dsp::AudioBlock<float> block(buffer);
    bandPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));

    // Calcul RMS des basses (1 canal ou moyenner les 2)
    rms = 0.0f;
    int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        const float* data = buffer.getReadPointer(ch);
        float sumSquares = 0.0f;
        for (int i = 0; i < numSamples; ++i)
            sumSquares += data[i] * data[i];

        rms += std::sqrt(sumSquares / numSamples);
    }

    rms /= static_cast<float>(buffer.getNumChannels());
    // rms = toLogScale(rms);
}
