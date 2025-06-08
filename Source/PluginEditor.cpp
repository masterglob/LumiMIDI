
// =============================================================================
// PluginEditor.cpp
// =============================================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"

LumiMIDIEditor::LumiMIDIEditor (LumiMIDIProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : AudioProcessorEditor (&p), mAudioProcessor (p), mApvts(apvts), // filterSection(apvts),
     midiKeyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{

    // KEYBOARD
    midiKeyboard.setKeyWidth(12.0f);
    midiKeyboard.setLowestVisibleKey(36); // C2
    midiKeyboard.setKeyPressBaseOctave(4);
    keyboardState.addListener(this);

    midiKeyboard.setNoteColour(60, juce::Colours::red);      // C
    midiKeyboard.setNoteColour(62, juce::Colours::orange);   // D
    midiKeyboard.setNoteColour(64, juce::Colours::yellow);   // E
    midiKeyboard.setNoteColour(65, juce::Colours::green);    // F
    midiKeyboard.setNoteColour(67, juce::Colours::blue);     // G
    midiKeyboard.setNoteColour(69, juce::Colours::indigo);   // A
    midiKeyboard.setNoteColour(71, juce::Colours::violet);   // B

    // Ou marquer des keyswitch comme dans Kontakt
    midiKeyboard.setNoteColour(36, juce::Colours::cyan);     // Keyswitch C1
    midiKeyboard.setNoteColour(37, juce::Colours::magenta);  // Keyswitch C#1

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

    // Taille de l'interface
    setSize(800, 600);

    // Démarrage du timer pour les animations
    startTimer(25); // 60 FPS
}

LumiMIDIEditor::~LumiMIDIEditor()
{
    stopTimer();
    keyboardState.removeListener(this);
    midiKeyboard.setLookAndFeel(nullptr);
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
    
    // Titre du plugin
    g.setColour(juce::Colours::white);

    g.setFont(juce::FontOptions().withName("Arial").withPointHeight(32.0f).withStyle("Bold"));
    g.drawText("LumiMIDI", getLocalBounds().removeFromTop(80), juce::Justification::centred);
    
    // Sous-titre
    g.setFont(juce::FontOptions().withName("Arial").withPointHeight(14.0f).withStyle("Italic"));
    g.setColour(juce::Colour(0xff0ea5e9));
    g.drawText("by CMM Studios", getLocalBounds().removeFromTop(100).removeFromBottom(20), juce::Justification::centred);
}

void LumiMIDIEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(100); // Espace pour le titre
    mBtnLearn.setBounds(20, 20, 100, 30); // x, y, width, height
    mBottomInfo.setBounds(bounds.removeFromBottom(40));
    midiKeyboard.setBounds(bounds.removeFromBottom(80));
    
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
}
