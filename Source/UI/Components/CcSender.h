
// ============================================================================
// UI/Components/CcSender.h
// ============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "DSP/AudioEngine.h"

class UI_CcSender : public juce::Component {
 public:
  UI_CcSender(const AudioEngine&);

  void resized() override;
  void paint(juce::Graphics& g) override;

 private:
  const AudioEngine& mEngine;

  juce::TextEditor mSend_CC_TextEditor;
  juce::Label mSend_CC_Label;
  juce::TextButton mSend_CC_Button;
  void send_CC_TextChanged();
  void send_CC_Clicked();

  // Optional: to restrict input to integers only
  class IntegerTextEditorFilter : public juce::TextEditor::InputFilter {
   public:
    juce::String filterNewText(juce::TextEditor&,
                               const juce::String& newInput) override {
      // Allow only digits, minus sign, and backspace/delete
      juce::String filtered;
      for (auto c : newInput) {
        if (juce::CharacterFunctions::isDigit(c))  // || c == '-')
          filtered += c;
      }
      return filtered;
    }
  };
  std::unique_ptr<IntegerTextEditorFilter> integerFilter;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_CcSender)
};
