// ============================================================================
// Source/DSP/AudioEngine.h
// ============================================================================
#pragma once

#include <list>
#include <vector>

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
    void setGlobalHueLevel(double level);

    juce::Colour getLedColor(unsigned short ledId) const;

    class Led_RGBW
    {
    public:
        Led_RGBW(unsigned short r, unsigned short g, unsigned short b, unsigned short w) : mr(r), mg(g), mb(b), mw(w) {}
        Led_RGBW(unsigned short i0, unsigned short delta) : mr(i0), mg(mr + delta), mb(mg + delta), mw(mb + delta) {}
        const unsigned short mr, mg, mb, mw;
    };

    using LedVect = std::vector<const Led_RGBW*>;

    /**********************************************************************************/
    class Program
    {
    public:
        Program(void) = default;
        virtual ~Program(void) = default;

        struct Event
        {
            Event(unsigned short alineIdx, unsigned char avalue):lineIdx(alineIdx), value(avalue){}
            unsigned short lineIdx;
            unsigned char value;
        };
        using Events = std::vector<Event>;

        virtual Events execute(const AudioEngine::LedVect& leds, const ParameterManager& parameterManager) = 0;
        virtual bool done(void)const { return false; }

    protected:

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Program)
    };

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
        unsigned char channel{ 0 };
        unsigned char ccR{ 0 };
        unsigned char ccG{ 0 };
        unsigned char ccB{ 0 };
        unsigned char ccW{ 0 };
    };
    LedMapping mLedMapping[NB_MAX_CMDS];

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


    /** 
    * Handles programs to apply. When no program is defined, a static value is applied to all LEDs
    */
    class ProgramManager
    {
    public:
        ProgramManager(AudioEngine&);

        /** Apply a new program. (removes all stored programs) */
        void set(Program* program);
        /** Push a new program on FIFO-like stack. Once terminated, the previous program goes on */
        void push(Program* program);

        void operator()(juce::MidiBuffer&);
    private:
        AudioEngine& mEngine;

        juce::CriticalSection mLock; // Protects mPrograms
        std::list<Program*> mPrograms;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgramManager)
    };
    ProgramManager mProgramManager;

    friend class ProgramManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};