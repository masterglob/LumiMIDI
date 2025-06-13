
// =============================================================================
// PluginEditor.cpp
// =============================================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"

LumiMIDIEditor::LumiMIDIEditor (LumiMIDIProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : AudioProcessorEditor (&p), mAudioProcessor (p), mApvts(apvts), // filterSection(apvts),
     midiKeyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
    mWorldView(apvts, p.getAudioEngine())
{

    // KEYBOARD
    keyboardState.addListener(this);
    addAndMakeVisible(midiKeyboard);

    // CC Sender editor
    // Set up the label
    mSend_CC_Label.setText("CC To send:", juce::dontSendNotification);
    mSend_CC_Label.attachToComponent(&mSend_CC_TextEditor, true);
    addAndMakeVisible(mSend_CC_Label);

    // SEND CC AREA
    mSend_CC_TextEditor.setMultiLine(false);
    mSend_CC_TextEditor.setReturnKeyStartsNewLine(false);
    mSend_CC_TextEditor.setReadOnly(false);
    mSend_CC_TextEditor.setScrollbarsShown(false);
    mSend_CC_TextEditor.setCaretVisible(true);
    mSend_CC_TextEditor.setPopupMenuEnabled(true);
    mSend_CC_TextEditor.setText("0"); // Default value

    mSend_CC_Button.setButtonText("Apply");
    mSend_CC_Button.onClick = [this]() { mSend_CC_Clicked(); };
    addAndMakeVisible(mSend_CC_Button);

    // Apply integer-only filter
    integerFilter = std::make_unique<IntegerTextEditorFilter>();
    mSend_CC_TextEditor.setInputFilter(integerFilter.get(), false);

    // Add listener for when text changes
    mSend_CC_TextEditor.onTextChange = [this]() { mSend_CC_TextChanged(); };

    addAndMakeVisible(mSend_CC_TextEditor);

    // "WHITE" Knob
    mWhiteGlobalKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mWhiteGlobalKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    mWhiteGlobalKnob.setRange(0.0, 1.0, 0.02);  // min, max, step
    mWhiteGlobalKnob.setValue(0.0);  // valeur par défaut
    mWhiteGlobalKnob.setNumDecimalPlacesToDisplay(1);
    mWhiteGlobalKnob.onValueChange = [this]() { whiteKnobValueChanged(); };
    addAndMakeVisible(mWhiteGlobalKnob);

    mWhiteGlobalLabel.setText("White level:", juce::dontSendNotification);
    mWhiteGlobalLabel.attachToComponent(&mWhiteGlobalKnob, false);
    mWhiteGlobalLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(mWhiteGlobalLabel);


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

void LumiMIDIEditor::mSend_CC_TextChanged()
{
    juce::String text = mSend_CC_TextEditor.getText();

    if (text.isNotEmpty())
    {
        int value = text.getIntValue();

        // Validate range if needed
        if (value < 0)
        {
            mSend_CC_TextEditor.setText(juce::String("0"), false);
        }
        if (value >127)
        {
            mSend_CC_TextEditor.setText(juce::String("127"), false);
        }
    }
}

void LumiMIDIEditor::mSend_CC_Clicked()
{
    // Get the current value from text editor
    int currentValue = mSend_CC_TextEditor.getText().getIntValue();
    DBG("Button clicked! Current value: " + juce::String(currentValue));
    mAudioProcessor.sendDirectMidiEvent(juce::MidiMessage::controllerEvent(1, currentValue, 63));
}

void LumiMIDIEditor::whiteKnobValueChanged()
{
    double value = mWhiteGlobalKnob.getValue();
    DBG("Knob value: " + juce::String(value));
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
    
    // Titre du plugin
    g.setColour(juce::Colours::white);

    g.setFont(juce::FontOptions().withName("Arial").withPointHeight(32.0f).withStyle("Bold"));
    g.drawText("LumiMIDI", getLocalBounds().removeFromTop(80), juce::Justification::centred);
    
    // Sous-titre
    g.setFont(juce::FontOptions().withName("Arial").withPointHeight(14.0f).withStyle("Italic"));
    g.setColour(juce::Colour(0xff0ea5e9));
    g.drawText("by CMM Studios", getLocalBounds().removeFromTop(100).removeFromBottom(20), juce::Justification::centred);

    mWorldView.paint(g);
}

void LumiMIDIEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(100); // Espace pour le titre
    mBtnLearn.setBounds(20, 20, 100, 30); // x, y, width, height
    mBottomInfo.setBounds(bounds.removeFromBottom(40));
    midiKeyboard.setBounds(bounds.removeFromBottom(80));

    {

        // Create a horizontal strip for our controls
        auto controlStrip = bounds.removeFromTop(30);

        // Reserve space for label (left side)
        auto labelArea = controlStrip.removeFromLeft(100);
        // Label is attached to text editor, so it will position itself

        // Reserve space for button (right side)
        auto buttonArea = controlStrip.removeFromRight(80);
        mSend_CC_Button.setBounds(buttonArea);

        // Add some spacing between text editor and button
        controlStrip.removeFromRight(10);

        // Remaining space for text editor
        mSend_CC_TextEditor.setBounds(controlStrip);
    }
    
    {
        bounds.removeFromTop(30);
        auto knobArea = bounds.removeFromTop(100);  // Réserver de l'espace pour le knob
        mWhiteGlobalKnob.setBounds(knobArea.removeFromLeft(100));  // Ajuster la taille selon vos besoins
    }

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
