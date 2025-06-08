
// ============================================================================
// UI/Components/KnobComponent.h
// ============================================================================
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class KnobComponent : public juce::Component
{
public:
    KnobComponent(const juce::String& labelText, 
                  juce::AudioProcessorValueTreeState& apvts,
                  const juce::String& parameterID);
    
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider knob;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobComponent)
};
