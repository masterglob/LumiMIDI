
// ============================================================================
// UI/Components/KnobComponent.h
// ============================================================================
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class KnobComponent : public juce::Component
{
public:
    using OnChanged = std::function<void(double)>;
    KnobComponent(const juce::String& labelText, 
                  juce::AudioProcessorValueTreeState& apvts,
                  const juce::String& parameterID,
                  OnChanged onChanged);
    
    void resized() override;

private:

    juce::Slider knob;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    OnChanged mOnChanged;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobComponent)
};
