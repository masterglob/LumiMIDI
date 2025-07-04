
// ============================================================================
// UI/Components/WorldView.h
// ============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "DSP/AudioEngine.h"

class UI_WorldView : public juce::Component {
 public:
  UI_WorldView(juce::AudioProcessorValueTreeState& apvts, const AudioEngine&);

  void setup(juce::Component aComp, juce::Rectangle<int> bounds);
  void paint(juce::Graphics& g) override;
  void resized() override;

 private:
  void refreshLeds(void);
  const AudioEngine& mEngine;

  using LedsMap = std::map<LedId, LedContext>;
  LedsMap mLedsMap;
  bool mModified{true};
  bool mShowLedNames{false};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_WorldView)
};
