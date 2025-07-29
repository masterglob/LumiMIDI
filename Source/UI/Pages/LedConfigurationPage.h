// =============================================================================
// LedConfigurationPage.h
// =============================================================================
#pragma once
#include "PageBase.h"
#include "LumiMIDIProcessor.h"
#include "UI/Components/WorldView.h"
#include "UI/Components/KnobComponent.h"
#include "UI/Components/SimpleVuMeter.h"
#include <juce_audio_processors/juce_audio_processors.h>

class LedConfigurationPage : public PageBase {
public:
    LedConfigurationPage(LumiMIDIProcessor& processor,
        juce::AudioProcessorValueTreeState& apvts,
        UI_WorldView& worldView);
    ~LedConfigurationPage() override = default;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;

    // PageBase overrides
    void activate() override;
    void deactivate() override;

private:
    void setupComponents();
    void setupLayout();

private:
    LumiMIDIProcessor& mProcessor;
    juce::AudioProcessorValueTreeState& mApvts;

    UI_WorldView& mWorldView;
    KnobComponent mWhiteGlobalKnob;
    KnobComponent mHueGlobalKnob;
    KnobComponent mSpeedKnob;
    UI_SimpleVuMeter mLowVuMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LedConfigurationPage)
};