
// ============================================================================
// UI/Components/HueKnobComponent.h
// ============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "KnobComponent.h"

class HueKnob : public KnobComponent {
 public:
  HueKnob(const juce::String& labelText,
          juce::AudioProcessorValueTreeState& apvts,
          const juce::String& parameterID);

  void setHue(float hue);
  void setSaturation(float sat);

 private:
  void repaint();
  float mHue{0.0f};
  float mSat{1.0f};
};

class HueSatKnob : public KnobComponent {
 public:
  HueSatKnob(const juce::String& labelText,
             juce::AudioProcessorValueTreeState& apvts,
             const juce::String& parameterID);

  void setSaturation(float sat);

 private:
  void repaint();
  float mSat{1.0f};
};
