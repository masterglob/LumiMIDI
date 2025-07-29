// =============================================================================
// ProgrammingPage.h
// =============================================================================
#pragma once
#include "PageBase.h"
#include "LumiMIDIProcessor.h"
#include "UI/Components/CustomMidiKeyboard.h"
#include "UI/Components/CcSender.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>

class ProgrammingPage : public PageBase,
    public juce::MidiKeyboardState::Listener {
public:
    ProgrammingPage(LumiMIDIProcessor& processor,
        juce::AudioProcessorValueTreeState& apvts,
        juce::MidiKeyboardState& keyboardState);
    ~ProgrammingPage() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;

    // PageBase overrides
    void activate() override;
    void deactivate() override;

    // MidiKeyboardState::Listener overrides
    void handleNoteOn(juce::MidiKeyboardState* source,
        int midiChannel,
        int midiNoteNumber,
        float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState* source,
        int midiChannel,
        int midiNoteNumber,
        float velocity) override;

    // Public interface
    void onSend_CC_Clicked(unsigned int cc);

private:
    void setupComponents();
    void setupLayout();

private:
    LumiMIDIProcessor& mProcessor;
    juce::AudioProcessorValueTreeState& mApvts;
    juce::MidiKeyboardState& mKeyboardState;

    CustomMidiKeyboard mMidiKeyboard;
    juce::TextButton mBtnLearn;
    UI_CcSender mCcSender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgrammingPage)
};