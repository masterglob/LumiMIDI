// ============================================================================
// Source/DSP/SimpleStroboscope.h
// ============================================================================
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "DSP/BaseProgram.h"

namespace PROGS {
class SimpleStroboscope : public BaseProgram {
 public:
  SimpleStroboscope();

 private:
  void execute(const LedVect& leds,
               const ParameterManager& parameterManager,
               BaseProgram::Events&) override;

  juce::uint32 startMillis{0};
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleStroboscope)
};
}  // namespace PROGS
