// =============================================================================
// ProgrammingPage.h
// =============================================================================
#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "LumiMIDIProcessor.h"
#include "PageBase.h"
#include "UI/Components/CustomMidiKeyboard.h"
#include "UI/Components/KnobComponent.h"
#include "UI/Components/WorldView.h"

class ProgrammingPage : public PageBase,
                        public juce::MidiKeyboardState::Listener {
 public:
  ProgrammingPage(LumiMIDIProcessor& processor,
                  juce::AudioProcessorValueTreeState& apvts,
                  juce::MidiKeyboardState& keyboardState,
                  UI_WorldView& worldView);
  ~ProgrammingPage() override;

  // Component overrides
  void paint(juce::Graphics& g) override;
  void resized() override;

  // PageBase overrides
  void activate() override;
  void deactivate() override;

  // MidiKeyboardState::Listener overrides
  void handleNoteOn(juce::MidiKeyboardState* source,
                    int midiChannel,
                    int midiNoteNumber,
                    float velocity) override;
  void handleNoteOff(juce::MidiKeyboardState* source,
                     int midiChannel,
                     int midiNoteNumber,
                     float velocity) override;

  // Public interface
  void onSend_CC_Clicked(unsigned int cc);

  void setProgramName(const juce::String& name);

 private:
  void setupComponents();
  void setupLayout();

 private:
  LumiMIDIProcessor& mProcessor;
  juce::AudioProcessorValueTreeState& mApvts;
  juce::MidiKeyboardState& mKeyboardState;

  UI_WorldView& mWorldView;
  juce::Label mProgramName;
  KnobComponent mWhiteGlobalKnob;
  KnobComponent mHueGlobalKnob;
  KnobComponent mSpeedKnob;
  KnobComponent mPhaseKnob;
  CustomMidiKeyboard mMidiKeyboard;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgrammingPage)
};
