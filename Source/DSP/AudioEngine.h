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

    void setGlobalWhiteLevel(double level);

    juce::Colour getLedColor(int ledId) const;

private:
    void processMidiMessages(juce::MidiBuffer& midiMessages);
    void learn(const juce::MidiMessage& message);

    ParameterManager& parameterManager;

    // État du moteur
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;

    bool mLearning{ false };
    float mWhiteLevel{ 0.0f };
    juce::String mMessage{ "Welcome" };

    // Main colors by Note
    std::map<int, juce::Colour> noteColours;

    static const unsigned NB_MAX_CMDS{ 512 };
    struct LedMapping
    {
        unsigned char channel;
        unsigned char ccR;
        unsigned char ccG;
        unsigned char ccB;
        unsigned char ccW;
    };
    LedMapping mLedMapping[NB_MAX_CMDS] = { 0 };

    struct OutputMidiMsg
    {
        unsigned char channel{ 0 };
        unsigned char lastSent{ 0 };
    };

    struct OutputMidiContext
    {
        OutputMidiMsg mOutputContext[NB_MAX_CMDS];

        void insertEvent(juce::MidiBuffer& midiMessages, unsigned int lineId, unsigned char value);
    };
    juce::SpinLock mColorLock;
    OutputMidiContext mOutMidiCtxt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};