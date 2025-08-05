
// =============================================================================
// ProgrammingPage.cpp
// =============================================================================
#include "UI/Pages/ProgrammingPage.h"

ProgrammingPage::ProgrammingPage(LumiMIDIProcessor& processor,
    juce::AudioProcessorValueTreeState& apvts,
    juce::MidiKeyboardState& keyboardState,
    UI_WorldView& worldView)
    : mProcessor(processor)
    , mApvts(apvts)
    , mKeyboardState(keyboardState)
    , mWorldView( worldView)
    , mWhiteGlobalKnob("White",
        apvts,
        ParameterIDs::mainW,
        [this](double val) {
            mProcessor.getAudioEngine().setGlobalWhiteLevel(val);
        })
    , mHueGlobalKnob("Hue",
        apvts,
        ParameterIDs::mainHue,
        [this](double val) {
            mProcessor.getAudioEngine().setGlobalHueLevel(val);
        })
    , mSpeedKnob("Speed",
        apvts,
        ParameterIDs::speed,
        [this](double val) {
            mProcessor.getAudioEngine().setGlobalSpeedLevel(val);
        })
    , mMidiKeyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setupComponents();
    setupLayout();

    // Écouter les événements MIDI
    mKeyboardState.addListener(this);
}

ProgrammingPage::~ProgrammingPage() {
    mKeyboardState.removeListener(this);
}

void ProgrammingPage::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkblue.darker());

    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("Programming & Automation", getLocalBounds().removeFromTop(30),
        juce::Justification::centred);
}

void ProgrammingPage::resized() {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(35); // Title

    // Bottom keyboard
    auto keyboardHeight = 80;
    mMidiKeyboard.setBounds(bounds.removeFromBottom(keyboardHeight));
    bounds.removeFromBottom(10); // Spacing

    // WorldView + Controls
    auto controlArea = bounds.removeFromRight(160);
    auto btnLeft = controlArea.removeFromLeft(controlArea.getWidth() / 2);
    auto btnRight = controlArea;
    auto knobHeight = controlArea.getHeight() / 4;

    mWhiteGlobalKnob.setBounds(btnLeft.removeFromTop(knobHeight).reduced(5));
    mHueGlobalKnob.setBounds(btnLeft.removeFromTop(knobHeight).reduced(5));
    mSpeedKnob.setBounds(btnRight.removeFromTop(knobHeight).reduced(5));


    auto worldViewArea = bounds;

    if (mIsActive)
        mWorldView.setBounds(worldViewArea.reduced(5));
}

void ProgrammingPage::activate() {
    mIsActive = true;
    setVisible(true);
     // CORRECTION: Retirer le WorldView de son parent actuel avant de l'ajouter
    auto* currentParent = mWorldView.getParentComponent();
    if (currentParent && currentParent != this) {
        currentParent->removeChildComponent(&mWorldView);
        currentParent->repaint();
    }
    
    // Ajouter le WorldView à cette page
    addAndMakeVisible(mWorldView);
    mWorldView.setViewMode(UI_WorldView::ViewMode::Compact);
    mWorldView.setShowLedNames(false);
    mWorldView.setRefreshRate(15);

    mWhiteGlobalKnob.setVisible(true);
    mHueGlobalKnob.setVisible(true);
    mSpeedKnob.setVisible(true);

    repaint();
}

void ProgrammingPage::deactivate() {
    mIsActive = false;
    setVisible(false);
}

void ProgrammingPage::handleNoteOn(juce::MidiKeyboardState* source,
    int midiChannel,
    int midiNoteNumber,
    float velocity) {
    (void)source;
    // Créer le message MIDI
    auto message =
        juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);

    // Envoyer au processeur via une méthode sécurisée
    mProcessor.addMidiEvent(message);
}

void ProgrammingPage::handleNoteOff(juce::MidiKeyboardState* source,
    int midiChannel,
    int midiNoteNumber,
    float velocity) {
    (void)source;
    auto message =
        juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
    mProcessor.addMidiEvent(message);
}

void ProgrammingPage::onSend_CC_Clicked(unsigned int cc) {
    mProcessor.sendDirectMidiEvent(
        juce::MidiMessage::controllerEvent(1, cc, 63));
}

void ProgrammingPage::setupComponents() {
    mWorldView.setViewMode(UI_WorldView::ViewMode::Compact);
    mWorldView.setShowLedNames(false);
    mWorldView.setRefreshRate(15);

    addAndMakeVisible(mWorldView);
    addAndMakeVisible(mMidiKeyboard);

    addAndMakeVisible(mWhiteGlobalKnob);
    addAndMakeVisible(mHueGlobalKnob);
    addAndMakeVisible(mSpeedKnob);

}

void ProgrammingPage::setupLayout() {
    // Configuration des composants spécifiques
    // Paramètres des composants, styles, etc.
}
