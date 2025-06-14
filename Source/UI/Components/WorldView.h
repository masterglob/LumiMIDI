
// ============================================================================
// UI/Components/WorldView.h
// ============================================================================
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "DSP/AudioEngine.h"

class UI_WorldView : public juce::Component
{
public:
    UI_WorldView(juce::AudioProcessorValueTreeState& apvts, const AudioEngine&);
    
    void setup(juce::Component aComp, juce::Rectangle<int> bounds);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    const AudioEngine& mEngine;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_WorldView)
};