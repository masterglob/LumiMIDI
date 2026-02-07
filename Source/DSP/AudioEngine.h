// ============================================================================
// Source/DSP/AudioEngine.h
// ============================================================================
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include <list>
#include <map>
#include <vector>

#include "BaseProgram.h"
#include "DSP/Audio/DAudioFilter.h"
#include "DSP/Audio/DHysteresisTrigger.h"
#include "LedDB.h"

// Forward declaration
class ParameterManager;

struct MidiEvent {
  MidiEvent(const juce::MidiMessage& m, int64_t t) : msg(m), sampleTime(t) {}

  juce::MidiMessage msg;
  int64_t sampleTime;
};

class AudioEngine {
 public:
  AudioEngine(ParameterManager& paramManager);
  ~AudioEngine() = default;

  void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
  void releaseResources();
  void processBlock(juce::AudioBuffer<float>& buffer,
                    juce::MidiBuffer& midiMessages,
                    double blockDurationSeconds);
  void processBlock(juce::AudioBuffer<double>& buffer,
                    juce::MidiBuffer& midiMessages,
                    double blockDurationSeconds);

  inline void startLearn(void) { mLearning = true; }
  inline const juce::String& message() const { return mMessage; }

  void setGlobalWhiteLevel(double level);
  void setGlobalHueLevel(double level);
  void setGlobalPhaseLevel(double level);

  juce::Colour getLedColor(LedId ledId) const;
  juce::Colour getLedWhite(LedId ledId) const;
  inline float getLowFrqLevel() const { return mLowFreqLevel; };
  inline const LedDB& getLeds(void) const { return mLeds; }
  inline LedDB& getLeds(void) { return mLeds; }
  void updateLeds(void);

  void receiveMidiMsg(const juce::MidiMessage&);

  inline const BaseProgram* getCurrentProgram() const { return mProgramManager.getCurrentProgram(); }

  using ProgramsVect = std::vector<BaseProgram*>;
  inline ProgramsVect& getMainPrograms() { return mProgramManager.mainPrograms; }
  inline const ProgramsVect& getFxPrograms() const { return mProgramManager.fxPrograms; }

  juce::MidiMessage programToMidi(const BaseProgram*) const;
  BaseProgram* noteToProgram(int note) const;

 private:
  void processMidiMessages(juce::MidiBuffer& midiMessages, double blockDurationSeconds);
  void learn(const juce::MidiMessage& message);

  ParameterManager& parameterManager;

  double currentSampleRate = 44100.0;
  int currentBlockSize = 512;
  int mNumChannels{2};

  double mCurrentTimeS{0.0};
  juce::SpinLock mSpinLock;
  std::vector<MidiEvent> mPendingUiMidiMsg;

  bool mLearning{false};
  float mWhiteLevel{0.0f};
  float mHueLevel{0.0f};
  float mSpeedLevel{0.0f};
  float mPhaseLevel{0.0f};
  juce::String mMessage{"Welcome"};

  float mLowFreqLevel{0.0f};

  // Main colors by Note
  std::map<int, juce::Colour> noteColours;

  static const unsigned NB_MAX_LEDS{128};
  static const unsigned NB_MAX_CMDS{NB_MAX_LEDS * 4};

  LedDB mLeds;

  struct OutputMidiMsg {
    LineValue channel{0};
    LineValue lastSent{0};
  };

  struct OutputMidiContext {
    OutputMidiMsg mOutputContext[NB_MAX_CMDS];

    void insertEvent(juce::MidiBuffer& midiMessages, LineId lineId, LineValue value);
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

    void updateLeds(const LedVectId& m);
    void blockUpdate();  // Must be called during "block" processing

    BaseProgram* getByTrigger(const juce::MidiMessage& message, int& param);

    /** Apply a new program. (removes all stored programs) */
    void set(BaseProgram* program, CCValue velocity);

    /** Push a new program overlay */
    void pushFx(BaseProgram* program, CCValue velocity = MAX_CC_VALUE, juce::uint32 duration = 0u);

    void popFx(const BaseProgram* program);

    void operator()(juce::MidiBuffer&);
    inline const BaseProgram* getCurrentProgram() const { return mMainProgram; }

    ProgramsVect mainPrograms;
    ProgramsVect fxPrograms;

    using ProgramToMidi = std::map<const BaseProgram*, juce::MidiMessage>;
    ProgramToMidi mProgramToMidi;

    using NoteToProgram = std::map<int, BaseProgram*>;
    NoteToProgram mNoteToProgram;

   private:
    AudioEngine& mEngine;
    LedVect mLedsVect;
    std::unique_ptr<LedVectId> mLedsVectUpdate;

    using TimedProgram = std::pair<BaseProgram*, juce::uint32>;
    BaseProgram* mMainProgram{nullptr};
    TimedProgram mOverlayProgram = {nullptr, 0};

    using TriggerMap = std::map<juce::String, BaseProgram*>;
    TriggerMap mTriggers;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgramManager)
  };
  ProgramManager mProgramManager;

  friend class ProgramManager;
  BandePassFilter mLowFilter;
  DHysteresisTrigger mLowTrigger;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};
