// =============================================================================
// ProgrammingPage.h
// =============================================================================
#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "DSP/AudioEngine.h"
#include "LumiMIDIProcessor.h"
#include "PageBase.h"
#include "UI/Components/CustomMidiKeyboard.h"
#include "UI/Components/KnobComponent.h"
#include "UI/Components/WorldView.h"

class ProgramList : public juce::ListBoxModel {
 public:
  using ItemClickedCallback = std::function<void(const BaseProgram* pPrg)>;
  ProgramList(const AudioEngine::ProgramsVect& itemsRef);

  inline int getNumRows() override { return (int) items.size(); }

  void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;

  void listBoxItemClicked(int row, const juce::MouseEvent&) override;

  bool selectProgram(const BaseProgram*);

  ItemClickedCallback onItemClicked{nullptr};

  void setupComponents(juce::Component&);
  void resized(const juce::Rectangle<int>&);

 private:
  juce::ListBox mList;
  const AudioEngine::ProgramsVect& items;
  using ProgramToRaw = std::map<const BaseProgram*, int>;
  ProgramToRaw mProgramToRaw;
};

class ProgrammingPage : public PageBase, public juce::MidiKeyboardState::Listener {
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

  void setProgram(const BaseProgram* pPrg);

 private:
  void setupComponents();
  void setupLayout();

  const BaseProgram* mCurrPrg{nullptr};

 private:
  LumiMIDIProcessor& mProcessor;
  juce::AudioProcessorValueTreeState& mApvts;
  juce::MidiKeyboardState& mKeyboardState;

  UI_WorldView& mWorldView;
  juce::Label mProgramName;
  KnobComponent mWhiteGlobalKnob;
  KnobComponent mHueGlobalKnob;
  KnobComponent mFx1HueGlobalKnob;
  KnobComponent mFx2HueGlobalKnob;
  KnobComponent mSpeedKnob;
  KnobComponent mPhaseKnob;
  CustomMidiKeyboard mMidiKeyboard;

  // Zone Program/FX
  juce::Label mMainProgramLabel;
  ProgramList mMainProgramList;

  juce::Label mFxLabel;
  ProgramList mFxList;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgrammingPage)
};
