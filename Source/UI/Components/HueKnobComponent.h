
// ============================================================================
// UI/Components/KnobComponent.h
// ============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "KnobComponent.h"

class HueKnob : public KnobComponent {
 public:
  HueKnob(const juce::String& labelText,
          juce::AudioProcessorValueTreeState& apvts,
          const juce::String& parameterID)
      : KnobComponent(labelText, apvts, parameterID, [this](double val) {
          setColor(floatHueParamToColor(float(val)));
        }) {
    setColor(floatHueParamToColor(float(knob.getValue())));
  }

  void setColor(juce::Colour c) {
    knob.setColour(juce::Slider::textBoxBackgroundColourId, c);
    knob.setColour(juce::Slider::textBoxTextColourId, c);
    knob.repaint();
  };

 private:
};
