// ============================================================================
// Source/DSP/BaseProgram.h
// ============================================================================
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_graphics/juce_graphics.h>

#include <list>
#include <map>
#include <memory>
#include <vector>

#include "LedDB.h"

class ParameterManager;

/**********************************************************************************/
struct ProgramContext {};

/**********************************************************************************/
struct ProgramTrigger
{
    ProgramTrigger(const uint8_t id) : pId{id} {}
    virtual ~ProgramTrigger()         = default;
    virtual juce::String name() const = 0;
    const uint8_t        pId;
};

/**********************************************************************************/
struct ProgramTriggerCC : public ProgramTrigger
{
    using ProgramTrigger::ProgramTrigger;
    juce::String name() const override { return "CC#" + std::to_string(pId); }
};

/**********************************************************************************/
struct ProgramTriggerPC : public ProgramTrigger
{
    using ProgramTrigger::ProgramTrigger;
    juce::String name() const override { return "PC#" + std::to_string(pId); }
};

/**********************************************************************************/
struct ProgramTriggerNote : public ProgramTrigger
{
    using ProgramTrigger::ProgramTrigger;
    juce::String name() const override
    {
        const juce::String name(juce::MidiMessage::getMidiNoteName(pId,   // MIDI
                                                                   true,  // useSharps
                                                                   true,  // includeOctaveNumber
                                                                   4      // octaveNumberForMiddleC = 4
                                                                   ));
        return "(" + name + ")";
    }
};

/**********************************************************************************/
class BaseProgram {
 public:
  BaseProgram(const std::string& name);
  virtual ~BaseProgram(void) = default;

  struct Event {
    Event(LineId alineIdx, LineValue avalue)
        : lineIdx(alineIdx), value(avalue) {}
    LineId lineIdx;
    LineValue value;
  };
  using Events = std::vector<Event>;
  void reset(const CCValue velocity);
  virtual void execute(const LedVect& leds,
                       const ParameterManager& parameterManager,
                       Events&) = 0;
  const juce::String triggerName() const { return mTrigger == nullptr ? "??" : mTrigger->name(); }
  virtual bool done(void) const { return mDone; }
  virtual bool isFx(void) const { return false; }
  const std::string name;
  void setTrigger(ProgramTrigger* trg) { mTrigger.reset(trg); }
 protected:
  virtual void reset(void) {};
  static juce::uint32 floatToPeriod(float f); /* Input Range : [0..1] */
  LineValue floatToCcValue(float f);          /* Input Range : [0..127] */
  LineValue float01ToCcValue(float f);        /* Input Range : [0..1] */

  std::unique_ptr<ProgramContext> mContext{nullptr};
  juce::uint32 elapsedMs(void) const;
  bool mDone{false};
  CCValue mVelocity{0};

 private:
  BaseProgram(void) = delete;
  juce::uint32 startMillis{0};
  std::unique_ptr<ProgramTrigger> mTrigger;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseProgram)
};

/**********************************************************************************/
#define DECLARE_PROGRAM_CLASS(ClassName)                    \
  class ClassName : public BaseProgram {                    \
   public:                                                  \
    ClassName();                                            \
    void execute(const LedVect& leds,                       \
                 const ParameterManager& parameterManager,  \
                 BaseProgram::Events& events);              \
                                                            \
   private:                                                 \
    void reset(void) override;                              \
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassName) \
  }

/**********************************************************************************/
#define DECLARE_FX_CLASS(ClassName)                         \
  class ClassName : public BaseProgram {                    \
   public:                                                  \
    ClassName();                                            \
    void execute(const LedVect& leds,                       \
                 const ParameterManager& parameterManager,  \
                 BaseProgram::Events& events);              \
                                                            \
   private:                                                 \
    void reset(void) override;                              \
    bool isFx(void) const override {                        \
      return true;                                          \
    }                                                       \
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassName) \
  }

namespace PROGS {
DECLARE_PROGRAM_CLASS(Template);

DECLARE_PROGRAM_CLASS(DefaultProgram);
DECLARE_PROGRAM_CLASS(Breathing);
DECLARE_PROGRAM_CLASS(WarmCoolCycle);
DECLARE_PROGRAM_CLASS(RandomFill);
DECLARE_PROGRAM_CLASS(ZoneFlash);

DECLARE_FX_CLASS(SimpleStroboscope);
DECLARE_FX_CLASS(SimpleWave);
DECLARE_FX_CLASS(RandomSparkle);

}  // namespace PROGS
