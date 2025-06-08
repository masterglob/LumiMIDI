// ============================================================================
// Source/DSP/AudioEngine.h
// ============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>

// Forward declaration
class ParameterManager;

class AudioEngine
{
public:
    AudioEngine(ParameterManager& paramManager);
    ~AudioEngine() = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages);

    void startLearn(void) {mLearning = true;}
    const juce::String& message()const {  return mMessage; }

private:
    void processMidiMessages(juce::MidiBuffer& midiMessages);
    void learn(const juce::MidiMessage& message);

    ParameterManager& parameterManager;

    // État du moteur
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;

    bool mLearning{ false };
    juce::String mMessage{ "Welcome" };

    // Main colors by Note
    std::map<int, juce::Colour> noteColours;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};