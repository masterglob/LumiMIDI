// ============================================================================
// Source/DSP/BaseProgram.h
// ============================================================================
#pragma once

#include <list>
#include <map>
#include <memory>
#include <vector>

#include <juce_graphics/juce_graphics.h>

#include "LEdDB.h"

class ParameterManager;

/**********************************************************************************/
struct ProgramContext {};
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
  virtual bool done(void) const { return mDone; }

  const std::string name;

 protected:
  virtual void reset(void) {};
  static juce::uint32 floatToPeriod(float f); /* Input Range : [0..1] */
  LineValue floatToCcValue(float f);          /* Input Range : [0..127] */

  std::unique_ptr<ProgramContext> mContext{nullptr};
  juce::uint32 elapsedMs(void) const;
  bool mDone{false};
  CCValue mVelocity{0};

 private:
  BaseProgram(void) = delete;
  juce::uint32 startMillis{0};
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

namespace PROGS {
DECLARE_PROGRAM_CLASS(DefaultProgram);
DECLARE_PROGRAM_CLASS(SimpleStroboscope);
DECLARE_PROGRAM_CLASS(SimpleWave);
DECLARE_PROGRAM_CLASS(RandomSparkle);

}  // namespace PROGS
