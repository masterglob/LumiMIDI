
// ============================================================================
// UI/Components/KnobComponent.cpp
// ============================================================================
#include "KnobComponent.h"

#include <juce_graphics/juce_graphics.h>

KnobComponent::KnobComponent(const juce::String& labelText, 
                           juce::AudioProcessorValueTreeState& apvts,
                           const juce::String& parameterID)
{
    // Configuration du slider
    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(knob);
    
    // Configuration du label
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);

    label.setFont(juce::FontOptions()
        .withName("Arial")
        .withPointHeight(12.0f));
    addAndMakeVisible(label);
    
    // Attachment au paramètre
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, parameterID, knob);
}

void KnobComponent::paint(juce::Graphics& g)
{
    // Optionnel : dessiner un fond ou des décorations
    (void)g;
}

void KnobComponent::resized()
{
    auto bounds = getLocalBounds();
    
    label.setBounds(bounds.removeFromTop(20));
    knob.setBounds(bounds.removeFromTop(bounds.getHeight() - 20));
}
