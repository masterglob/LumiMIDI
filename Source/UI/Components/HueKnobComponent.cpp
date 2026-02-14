
// ============================================================================
// UI/Components/KnobComponent.h
// ============================================================================

#include "HueKnobComponent.h"

#include "Parameters/ParameterManager.h"

HueKnob::HueKnob(const juce::String& labelText,
                 juce::AudioProcessorValueTreeState& apvts,
                 const juce::String& parameterID)
    : KnobComponent(labelText, apvts, parameterID, [this](double val) { setHue(float(val)); }) {
  repaint();
}

void HueKnob::repaint() {
  juce::Colour c{floatHueParamToColor(mHue, mSat)};
  knob.setColour(juce::Slider::textBoxBackgroundColourId, c);
  knob.setColour(juce::Slider::textBoxTextColourId, c);
  knob.repaint();
}

void HueKnob::setHue(float hue) {
  mHue = hue;
  repaint();
};

void HueKnob::setSaturation(float sat) {
  mSat = sat;
  repaint();
}

HueSatKnob::HueSatKnob(const juce::String& labelText,
                       juce::AudioProcessorValueTreeState& apvts,
                       const juce::String& parameterID)
    : KnobComponent(labelText, apvts, parameterID) {
  setOnChanged([this](double val) { setSaturation(float(val)); });
  repaint();
}

void HueSatKnob::repaint() {
  juce::Colour c{juce::Colour::fromFloatRGBA(mSat, mSat, mSat, 1.0f)};
  knob.setColour(juce::Slider::textBoxBackgroundColourId, c);
  knob.setColour(juce::Slider::textBoxTextColourId, c);
  knob.repaint();
}

void HueSatKnob::setSaturation(float sat) {
  mSat = sat;
  repaint();
};