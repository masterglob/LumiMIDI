
// =============================================================================
// PluginEditor.cpp
// =============================================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"

LumiMIDIEditor::LumiMIDIEditor (LumiMIDIProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : AudioProcessorEditor (&p), audioProcessor (p), apvts(apvts), filterSection(apvts)
{
    // Taille de l'interface
    setSize (800, 600);
    
    // Démarrage du timer pour les animations
    startTimer(25); // 60 FPS

    // Apply the custom look and feel
    mBtnLearn.setLookAndFeel(&customLookAndFeel);
    // Optional: Set up button color (this can be used in drawButtonBackground)
    mBtnLearn.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

    // Add button to the editor
    addAndMakeVisible(mBtnLearn);
    mBtnLearn.onClick = [this]()
        {
            audioProcessor.getAudioEngine().startLearn();
        };

    mBottomInfo.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(mBottomInfo);
}

LumiMIDIEditor::~LumiMIDIEditor()
{
    stopTimer();
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
    mBottomInfo.setBounds(20, 560, 760, 30);
    
}

void LumiMIDIEditor::timerCallback()
{
    // Mise à jour des animations
    AudioEngine& engine(audioProcessor.getAudioEngine());

    const juce::String& newMsg(engine.message());
    if (newMsg != mPrevMsg)
    {
        mPrevMsg = newMsg;
        mBottomInfo.setText(newMsg, juce::dontSendNotification);
    }
}
