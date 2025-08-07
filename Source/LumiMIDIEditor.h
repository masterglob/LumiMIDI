
// =============================================================================
// LumiMIDIEditor.h
// =============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "LumiMIDIProcessor.h"
#include "UI/Components/WorldView.h"
#include "UI/Pages/PageManager.h"
#include "UI/LookAndFeel/CustomLookAndFeel.h"

class LumiMIDIEditor : public juce::AudioProcessorEditor,
                       public juce::Timer {
 public:
  LumiMIDIEditor(LumiMIDIProcessor&, juce::AudioProcessorValueTreeState&);
  ~LumiMIDIEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  void timerCallback() override;

private:
    void setupComponents();

 private:
  LumiMIDIProcessor& mProcessor;
  juce::AudioProcessorValueTreeState& mApvts;

  CustomLookAndFeel customLookAndFeel;
  // Shared components
  UI_WorldView mWorldView;
  juce::MidiKeyboardState keyboardState;

  // Main manager
  PageManager mPageManager;

  juce::Label mBottomInfo;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiMIDIEditor)
};
