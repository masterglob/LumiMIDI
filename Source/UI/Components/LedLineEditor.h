#pragma once

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include "UI/Components/NumericTextEditor.h"

class LedConfigurationPage;
struct LedLineEditor {
  LedLineEditor(LedConfigurationPage* mgr, const juce::String name);
  void setVisible(bool v);
  void setEnabled(bool e);
  void resized(juce::Rectangle<int>& r);
  void setupComponents(juce::Component& c);
  void setNumericValue(int v) { mMidiValueEditor.setNumericValue(v, false); }
  int getNumericValue() const { return mMidiValueEditor.getNumericValue(); }
  void setSelectedId(int v) { mMidiTypeCombo.setSelectedId(v, juce::dontSendNotification); }
  int getSelectedId() const { return mMidiTypeCombo.getSelectedId(); }
  void setCcNumber(int v);

  void refresh(void);

  LedConfigurationPage* mMgr;

  juce::Label mLabel;
  juce::ComboBox mMidiTypeCombo;       // CC / Note On
  juce::Label mMidiPrefix;             // "CC#" or "Note#"
  NumericTextEditor mMidiValueEditor;  // Numeric input 0-127 with wheel support
};