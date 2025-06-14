
// =============================================================================
// PluginEditor.cpp
// =============================================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"

LumiMIDIEditor::LumiMIDIEditor (LumiMIDIProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : AudioProcessorEditor (&p), mAudioProcessor (p), mApvts(apvts), // filterSection(apvts),
     midiKeyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
    mWhiteGlobalKnob("White", apvts, ParameterIDs::mainW, [this](double val) {mAudioProcessor.getAudioEngine().setGlobalWhiteLevel(val); }),
    mHueGlobalKnob("Hue", apvts, ParameterIDs::mainHue, 
        [this](double val) {mAudioProcessor.getAudioEngine().setGlobalHueLevel(val); }),
    mWorldView(apvts, p.getAudioEngine()),
    mCcSender(mAudioProcessor.getAudioEngine())
{

    // KEYBOARD
    keyboardState.addListener(this);
    addAndMakeVisible(midiKeyboard);

    // Apply the custom look and feel
    mBtnLearn.setLookAndFeel(&customLookAndFeel);
    // Optional: Set up button color (this can be used in drawButtonBackground)
    mBtnLearn.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

    // Add button to the editor
    addAndMakeVisible(mBtnLearn);
    mBtnLearn.onClick = [this]()
        {
            mAudioProcessor.getAudioEngine().startLearn();
        };

    mBottomInfo.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(mBottomInfo);
    addAndMakeVisible(mWhiteGlobalKnob);
    addAndMakeVisible(mHueGlobalKnob);
    addAndMakeVisible(mCcSender);

    // Taille de l'interface
    setSize(1000, 800);

    // Démarrage du timer pour les animations
    startTimer(25); // 60 FPS
}

LumiMIDIEditor::~LumiMIDIEditor()
{
    stopTimer();
    keyboardState.removeListener(this);
    midiKeyboard.setLookAndFeel(nullptr);
}

void LumiMIDIEditor::onSend_CC_Clicked(unsigned int cc)
{
    // Get the current value from text editor
    mAudioProcessor.sendDirectMidiEvent(juce::MidiMessage::controllerEvent(1, cc, 63));
}

void LumiMIDIEditor::whiteKnobValueChanged(double value)
{
    DBG("White Knob value: " + juce::String(value));
    mAudioProcessor.getAudioEngine().setGlobalWhiteLevel(value);
}

void LumiMIDIEditor::handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    (void)source;
    // Créer le message MIDI
    auto message = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);

    // Envoyer au processeur via une méthode sécurisée
    mAudioProcessor.addMidiEvent(message);
}

void LumiMIDIEditor::handleNoteOff(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    (void)source;
    auto message = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
    mAudioProcessor.addMidiEvent(message);
}
void LumiMIDIEditor::paint (juce::Graphics& g)
{
    // Gradient de fond
    juce::ColourGradient gradient(juce::Colour(0xff1a1a2e), 0, 0,
                                  juce::Colour(0xff16213e), static_cast<float>(getWidth()), static_cast<float>(getHeight()), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    mWorldView.paint(g);
}

void LumiMIDIEditor::resized()
{
    auto bounds = getLocalBounds();
    mBtnLearn.setBounds(270, 20, 100, 30); // x, y, width, height
    mBottomInfo.setBounds(bounds.removeFromBottom(40));
    midiKeyboard.setBounds(bounds.removeFromBottom(80));

    // top pane
    {
        auto topPane = bounds.removeFromTop(40);
        {
            mCcSender.setBounds(topPane);
        }
    }
    // Left pane
    {
        auto leftSide = bounds.removeFromLeft(80);
        {
            mWhiteGlobalKnob.setBounds(leftSide.removeFromTop(120));
            mHueGlobalKnob.setBounds(leftSide.removeFromTop(120));
        }
    }

    //auto rigthSide = bounds.removeFromRight(100);


    mWorldView.resized();
}

void LumiMIDIEditor::timerCallback()
{
    // Mise à jour des animations
    AudioEngine& engine(mAudioProcessor.getAudioEngine());

    const juce::String& newMsg(engine.message());
    if (newMsg != mPrevMsg)
    {
        mPrevMsg = newMsg;
        mBottomInfo.setText(newMsg, juce::dontSendNotification);
    }

    repaint();
}
