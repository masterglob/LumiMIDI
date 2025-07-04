
// ============================================================================
// UI/Components/KnobComponent.cpp
// ============================================================================
#include "KnobComponent.h"

#include <juce_graphics/juce_graphics.h>

KnobComponent::KnobComponent(const juce::String& labelText,
                             juce::AudioProcessorValueTreeState& apvts,
                             const juce::String& parameterID,
                             OnChanged onChanged)
    : mOnChanged(onChanged) {
  // Configuration du slider
  knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
  knob.setRange(0.0, 1.0, 0.02);  // min, max, step
  knob.setValue(0.0);
  knob.setNumDecimalPlacesToDisplay(1);
  knob.onValueChange = [this]() {
    if (mOnChanged)
      mOnChanged(knob.getValue());
  };
  knob.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::blue);
  knob.setColour(juce::Slider::rotarySliderOutlineColourId,
                 juce::Colours::darkgrey);
  knob.setColour(juce::Slider::thumbColourId, juce::Colours::white);
  addAndMakeVisible(knob);

  // Configuration du label
  label.setText(labelText, juce::dontSendNotification);
  // label.attachToComponent(&knob, false);
  label.setJustificationType(juce::Justification::centred);

  label.setFont(juce::FontOptions().withName("Arial").withPointHeight(14.0f));
  addAndMakeVisible(label);

  // Attachment au paramètre
  attachment =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          apvts, parameterID, knob);
}

void KnobComponent::resized() {
  auto area = getLocalBounds();
  area.reduce(3, 3);
  label.setBounds(area.removeFromTop(12));
  knob.setBounds(getLocalBounds());
}
