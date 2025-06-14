
// =============================================================================
// PluginEditor.h
// =============================================================================
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "PluginProcessor.h"
#include "UI/LookAndFeel/CustomLookAndFeel.h"
#include "UI/Components/FilterSection.h"
#include "UI/Components/WorldView.h"
#include "UI/Components/CcSender.h"
#include "UI/Resources/ColourPalette.h"

class LumiMIDIEditor : public juce::AudioProcessorEditor,
                       public juce::Timer,
                       public juce::MidiKeyboardState::Listener
{
public:
    LumiMIDIEditor (LumiMIDIProcessor&, juce::AudioProcessorValueTreeState&);
    ~LumiMIDIEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    void onSend_CC_Clicked(unsigned int cc);

private:
    void handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

private:
    LumiMIDIProcessor& mAudioProcessor;
    juce::AudioProcessorValueTreeState& mApvts;
    
    CustomLookAndFeel customLookAndFeel;
    //FilterSection filterSection;

    juce::TextButton mBtnLearn{ "Learn" };
    juce::Label mBottomInfo{ "Welcome!" };

    juce::String mPrevMsg{ "" };

    juce::MidiKeyboardState keyboardState;
    CustomMidiKeyboard midiKeyboard;

    KnobComponent mWhiteGlobalKnob;
    void whiteKnobValueChanged(double value);

    KnobComponent mHueGlobalKnob;
    KnobComponent mSpeedKnob;

    UI_WorldView mWorldView;
    UI_CcSender mCcSender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LumiMIDIEditor)
};
