
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

    juce::TextEditor mSend_CC_TextEditor;
    juce::Label mSend_CC_Label;
    juce::TextButton mSend_CC_Button;
    void mSend_CC_TextChanged();
    void mSend_CC_Clicked();

    juce::MidiKeyboardState keyboardState;
    CustomMidiKeyboard midiKeyboard;


    // Optional: to restrict input to integers only
    class IntegerTextEditorFilter : public juce::TextEditor::InputFilter
    {
    public:
        juce::String filterNewText(juce::TextEditor&, const juce::String& newInput) override
        {
            // Allow only digits, minus sign, and backspace/delete
            juce::String filtered;
            for (auto c : newInput)
            {
                if (juce::CharacterFunctions::isDigit(c)) // || c == '-')
                    filtered += c;
            }
            return filtered;
        }
    };
    std::unique_ptr<IntegerTextEditorFilter> integerFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LumiMIDIEditor)
};
