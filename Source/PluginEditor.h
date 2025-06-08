
// =============================================================================
// PluginEditor.h
// =============================================================================
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "PluginProcessor.h"
#include "UI/LookAndFeel/CustomLookAndFeel.h"
#include "UI/Components/FilterSection.h"
#include "UI/Resources/ColourPalette.h"

class LumiMIDIEditor : public juce::AudioProcessorEditor,
                       public juce::Timer
{
public:
    LumiMIDIEditor (LumiMIDIProcessor&, juce::AudioProcessorValueTreeState&);
    ~LumiMIDIEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    LumiMIDIProcessor& mAudioProcessor;
    juce::AudioProcessorValueTreeState& mApvts;
    
    CustomLookAndFeel customLookAndFeel;
    //FilterSection filterSection;

    juce::TextButton mBtnLearn{ "Learn" };
    juce::Label mBottomInfo{ "Welcome!" };

    juce::String mPrevMsg{ "" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LumiMIDIEditor)
};
