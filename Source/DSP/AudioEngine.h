// ============================================================================
// Source/DSP/AudioEngine.h
// ============================================================================
#pragma once

#include <list>
#include <vector>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "BaseProgram.h"

// Forward declaration
class ParameterManager;

class AudioEngine {
 public:
  AudioEngine(ParameterManager& paramManager);
  ~AudioEngine() = default;

  void prepareToPlay(double sampleRate, int samplesPerBlock);
  void releaseResources();
  void processBlock(juce::AudioBuffer<float>& buffer,
                    juce::MidiBuffer& midiMessages);
  void processBlock(juce::AudioBuffer<double>& buffer,
                    juce::MidiBuffer& midiMessages);

  void startLearn(void) { mLearning = true; }
  const juce::String& message() const { return mMessage; }

  void setGlobalWhiteLevel(double level);
  void setGlobalHueLevel(double level);
  void setGlobalSpeedLevel(double level);

  juce::Colour getLedColor(LedId ledId) const;
  const LedVect& getLeds(void) const { return mLeds; }

 private:
  void processMidiMessages(juce::MidiBuffer& midiMessages);
  void learn(const juce::MidiMessage& message);

  ParameterManager& parameterManager;

  // État du moteur
  double currentSampleRate = 44100.0;
  int currentBlockSize = 512;

  bool mLearning{false};
  float mWhiteLevel{0.0f};
  float mHueLevel{0.0f};
  float mSpeedLevel{0.0f};
  juce::String mMessage{"Welcome"};

  // Main colors by Note
  std::map<int, juce::Colour> noteColours;

  static const unsigned NB_MAX_LEDS{128};
  static const unsigned NB_MAX_CMDS{NB_MAX_LEDS * 4};

  /* mLedsVect and mLedsArray contain the smae information but are both used for
   * optimisation */
  LedVect mLeds;

  struct OutputMidiMsg {
    LineValue channel{0};
    LineValue lastSent{0};
  };

  struct OutputMidiContext {
    OutputMidiMsg mOutputContext[NB_MAX_CMDS];

    void insertEvent(juce::MidiBuffer& midiMessages,
                     LineId lineId,
                     LineValue value);
  };
  juce::SpinLock mColorLock;
  OutputMidiContext mOutMidiCtxt;

  /**
   * Handles programs to apply. When no program is defined, a static value is
   * applied to all LEDs
   */
  class ProgramManager {
   public:
    ProgramManager(AudioEngine&);

    /** Apply a new program. (removes all stored programs) */
    void set(BaseProgram* program);
    /** Push a new program on FIFO-like stack. Once terminated, the previous
     * program goes on */
    void push(BaseProgram* program, juce::uint32 duration = 0u);

    void pop(BaseProgram* program);

    void operator()(juce::MidiBuffer&);

   private:
    AudioEngine& mEngine;

    juce::CriticalSection mLock;  // Protects mPrograms
    using TimedProgram = std::pair<BaseProgram*, juce::uint32>;
    std::list<TimedProgram> mPrograms;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgramManager)
  };
  ProgramManager mProgramManager;

  friend class ProgramManager;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};
