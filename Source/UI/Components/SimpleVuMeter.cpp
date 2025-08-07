#include "SimpleVuMeter.h"

UI_SimpleVuMeter::UI_SimpleVuMeter()
{
}

UI_SimpleVuMeter::~UI_SimpleVuMeter()
{
}

void UI_SimpleVuMeter::setLevel(float newLevel)
{
    // Clamp level to [0, 1]
    newLevel = juce::jlimit(0.0f, 1.0f, newLevel);

    if (level != newLevel)
    {
        level = newLevel;
        repaint();
    }
}

void UI_SimpleVuMeter::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::green);

    auto meterWidth = static_cast<int>(getWidth() * level);

    g.fillRect(0, 0, meterWidth, getHeight());
}
