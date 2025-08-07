#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "DSP/AudioEngine.h"

class UI_SimpleVuMeter : public juce::Component
{
public:
    UI_SimpleVuMeter();
    ~UI_SimpleVuMeter() override;

    /** Set the level to display (expected range 0.0 to 1.0) */
    void setLevel(float newLevel);

    void paint(juce::Graphics& g) override;

private:
    float level = 0.0f; // Current level (0.0 to 1.0)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_SimpleVuMeter)
};
