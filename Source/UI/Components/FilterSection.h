
// ============================================================================
// UI/Components/FilterSection.h
// ============================================================================
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "KnobComponent.h"

class FilterSection : public juce::Component
{
public:
    FilterSection(juce::AudioProcessorValueTreeState& apvts);
    
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    /*KnobComponent cutoffKnob;
    KnobComponent resonanceKnob;
    juce::Label sectionLabel; */
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterSection)
};