
// =============================================================================
// LumiMIDIEditor.cpp
// =============================================================================
#include "LumiMIDIEditor.h"
#include "LumiMIDIProcessor.h"
LumiMIDIEditor::LumiMIDIEditor(LumiMIDIProcessor& processorP,
    juce::AudioProcessorValueTreeState& apvts)
    : juce::AudioProcessorEditor(&processorP)
    , mProcessor(processorP)
    , mApvts(apvts)
    , mWorldView(apvts, mProcessor.getAudioEngine())
    , mPageManager(mProcessor, apvts, mWorldView, keyboardState)
    , mBottomInfo("Welcome!")
{
    setupComponents();
    setSize(800, 600);

    startTimerHz(30); // 30 FPS pour les mises à jour temps réel
}

LumiMIDIEditor::~LumiMIDIEditor() {
    setLookAndFeel(nullptr);
    mPageManager.setLookAndFeel(nullptr);
    mBottomInfo.setLookAndFeel(nullptr);
    stopTimer();
}

void LumiMIDIEditor::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  
}

void LumiMIDIEditor::resized() {
    auto bounds = getLocalBounds();

    // Info en bas
    auto bottomHeight = 25;
    mBottomInfo.setBounds(bounds.removeFromBottom(bottomHeight));

    // Le reste pour le gestionnaire de pages
    mPageManager.setBounds(bounds);
}

void LumiMIDIEditor::timerCallback() {
}

void LumiMIDIEditor::setupComponents() {
    addAndMakeVisible(mPageManager);
    addAndMakeVisible(mBottomInfo);

    setLookAndFeel(&customLookAndFeel);
}
