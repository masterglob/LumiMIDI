// =============================================================================
// UI/Pages/LedLineEditor.cpp
// =============================================================================

#include "UI/Components/LedLineEditor.h"

#include "UI/Pages/LedConfigurationPage.h"

LedLineEditor::LedLineEditor(LedConfigurationPage* mgr, const juce::String name)
    : mMgr(mgr), mLabel(name, name) {
  mMidiTypeCombo.onChange = [this]() { mMgr->onMidiMappingChanged(true); };
  mMidiValueEditor.onTextChange = [this]() { mMgr->onMidiMappingChanged(true); };
}

void LedLineEditor::setupComponents(juce::Component& c) {
  c.addAndMakeVisible(mLabel);
  c.addAndMakeVisible(mMidiTypeCombo);
  c.addAndMakeVisible(mMidiPrefix);
  c.addAndMakeVisible(mMidiValueEditor);

  // MIDI combo configuration
  mMidiTypeCombo.addItem("None", 1);
  mMidiTypeCombo.addItem("CC", 2);
  mMidiTypeCombo.addItem("Note On", 3);
  mMidiTypeCombo.setSelectedId(2);  // Default to CC
  mMidiTypeCombo.setEditableText(false);
  mMidiTypeCombo.setEnabled(true);

  // MIDI TextEditor configuration (0-127, numbers only)
  mMidiValueEditor.setRange(0, 127);        // Set valid range for MIDI values
  mMidiValueEditor.setWheelIncrement(1);    // Default increment for mouse wheel
  mMidiValueEditor.setArrowIncrement(1);    // Default increment for arrow keys
  mMidiValueEditor.setShiftMultiplier(10);  // Shift+wheel/arrow = increment by 10

  // Default values using setNumericValue
  mMidiValueEditor.setNumericValue(0, false);

  // Default prefixes (hidden initially since None is selected)
  mMidiPrefix.setText("", juce::dontSendNotification);

  // Hide prefix and value editor initially (None is selected by default)
  mMidiPrefix.setVisible(false);
  mMidiValueEditor.setVisible(false);
}

void LedLineEditor::resized(juce::Rectangle<int>& r) {
  static const int colorLabelWidth = 50;
  static const int comboWidth = 70;
  static const int prefixWidth = 40;  // "CC#" or "Note#"
  static const int editorWidth = 50;  // TextEditor for value
  static const int spacing2 = 5;

  mLabel.setBounds(r.removeFromLeft(colorLabelWidth));
  r.removeFromLeft(spacing2);
  mMidiTypeCombo.setBounds(r.removeFromLeft(comboWidth));
  r.removeFromLeft(spacing2);
  mMidiPrefix.setBounds(r.removeFromLeft(prefixWidth));
  mMidiValueEditor.setBounds(r.removeFromLeft(editorWidth));
}

void LedLineEditor::setCcNumber(int v) {
  if (v > 0) {
    setSelectedId(2);
    mMidiValueEditor.setNumericValue(v, false);
  } else {
    setSelectedId(1);  // None
  }
}

void LedLineEditor::setEnabled(bool e) {
  mMidiTypeCombo.setEnabled(e);
  mMidiValueEditor.setEnabled(e);
  if (!e) {
    setSelectedId(1);
  }
}

void LedLineEditor::setVisible(bool v) {
  mLabel.setVisible(v);
  mMidiTypeCombo.setVisible(v);

  // For White component, also check if MIDI type is None
  if (v) {
    bool midiEnabled = mMidiTypeCombo.getSelectedId() != 1;  // Not None
    mMidiPrefix.setVisible(midiEnabled);
    mMidiValueEditor.setVisible(midiEnabled);
  } else {
    mMidiPrefix.setVisible(false);
    mMidiValueEditor.setVisible(false);
  }
}

void LedLineEditor::refresh(void) {
  // Get selected type (1 = None, 2 = CC, 3 = Note On)
  int selectedType = getSelectedId();

  if (selectedType == 1) {  // None selected
    // Hide prefix and value editor
    mMidiPrefix.setVisible(false);
    mMidiValueEditor.setVisible(false);
  } else {
    // Show prefix and value editor
    mMidiPrefix.setVisible(true);
    mMidiValueEditor.setVisible(true);

    // Set appropriate prefix
    juce::String typePrefix = (selectedType == 2) ? "CC#" : "Note#";
    mMidiPrefix.setText(typePrefix, juce::dontSendNotification);

    // Validate value in TextEditor (0-127) - now handled automatically by NumericTextEditor
    int value = mMidiValueEditor.getNumericValue();
    (void) value;  // TODO
                   // Value is automatically constrained by NumericTextEditor
                   // No need for manual validation
  }
}
