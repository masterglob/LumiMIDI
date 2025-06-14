// ============================================================================
// Source/DSP/AudioEngine.h
// ============================================================================
#pragma once

#include <list>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include "BaseProgram.h"

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
    void setGlobalHueLevel(double level);

    juce::Colour getLedColor(LineId ledId) const;


private:
    void processMidiMessages(juce::MidiBuffer& midiMessages);
    void learn(const juce::MidiMessage& message);

    ParameterManager& parameterManager;

    // État du moteur
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;

    bool mLearning{ false };
    float mWhiteLevel{ 0.0f };
    float mHueLevel{ 0.0f };
    juce::String mMessage{ "Welcome" };

    // Main colors by Note
    std::map<int, juce::Colour> noteColours;

    static const unsigned NB_MAX_CMDS{ 512 };
    struct LedMapping
    {
        LineValue channel{ 0 };
        LineValue ccR{ 0 };
        LineValue ccG{ 0 };
        LineValue ccB{ 0 };
        LineValue ccW{ 0 };
    };
    LedMapping mLedMapping[NB_MAX_CMDS];

    struct OutputMidiMsg
    {
        LineValue channel{ 0 };
        LineValue lastSent{ 0 };
    };

    struct OutputMidiContext
    {
        OutputMidiMsg mOutputContext[NB_MAX_CMDS];

        void insertEvent(juce::MidiBuffer& midiMessages, LineId lineId, LineValue value);
    };
    juce::SpinLock mColorLock;
    OutputMidiContext mOutMidiCtxt;


    /** 
    * Handles programs to apply. When no program is defined, a static value is applied to all LEDs
    */
    class ProgramManager
    {
    public:
        ProgramManager(AudioEngine&);

        /** Apply a new program. (removes all stored programs) */
        void set(BaseProgram* program);
        /** Push a new program on FIFO-like stack. Once terminated, the previous program goes on */
        void push(BaseProgram* program);

        void operator()(juce::MidiBuffer&);
    private:
        AudioEngine& mEngine;

        juce::CriticalSection mLock; // Protects mPrograms
        std::list<BaseProgram*> mPrograms;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgramManager)
    };
    ProgramManager mProgramManager;

    friend class ProgramManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};