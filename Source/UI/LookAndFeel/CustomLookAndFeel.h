// ============================================================================
// UI/LookAndFeel/CustomLookAndFeel.h
// ============================================================================
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    
    // Personnalisation des sliders
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override;
    
    // Personnalisation des boutons
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                             const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown) override;

private:
    juce::Colour primaryColour;
    juce::Colour secondaryColour;
    juce::Colour accentColour;
};
