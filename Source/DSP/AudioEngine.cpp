
// ============================================================================
// Source/DSP/AudioEngine.cpp
// ============================================================================
#include "AudioEngine.h"
#include "../Parameters/ParameterManager.h"
#include "../UI/Resources/ColourPalette.h"

AudioEngine::AudioEngine(ParameterManager& paramManager)
    : parameterManager(paramManager)
{
    int note{ ColourPalette::colorPaletteFirstNote };

    for (const juce::Colour& col : ColourPalette::getBalancedSatColors())
    {
        noteColours[note] = col;
        note = ColourPalette::getNextWhiteKey(note);
    }
    for (const juce::Colour& col : ColourPalette::getBalancedHueColors())
    {
        noteColours[note] = col;
        note = ColourPalette::getNextWhiteKey(note);
    }
}

void AudioEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    // Rien d'autre à préparer pour le moment
}

void AudioEngine::releaseResources()
{
    // Nettoyer les ressources si nécessaire
}

void AudioEngine::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Effacer le buffer audio (pas de génération d'audio)
    buffer.clear();

    // Traiter les messages MIDI entrants
    processMidiMessages(midiMessages);
}

void AudioEngine::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    // Effacer le buffer audio (pas de génération d'audio)
    buffer.clear();

    // Traiter les messages MIDI entrants
    processMidiMessages(midiMessages);
}

void AudioEngine::learn(const juce::MidiMessage& message)
{
    mLearning = false;
    mMessage = message.getDescription();
}

void AudioEngine::processMidiMessages(juce::MidiBuffer& midiMessages)
{
    // Parcourir tous les messages MIDI du buffer
    for (const auto metadata : midiMessages)
    {
        juce::MidiMessage message = metadata.getMessage();
        if (mLearning)
        {
            learn(message);
        }

        // Exemple de traitement des messages MIDI
        if (message.isNoteOn())
        {
            // Message Note On reçu
            auto noteNumber = message.getNoteNumber();
            auto velocity = message.getVelocity();
            (void)noteNumber;
            (void)velocity;

            auto it(noteColours.find(noteNumber));
            if (it != noteColours.end())
            {
                static const float coef(1 / 128.0);
                const juce::Colour col(it->second);
                // Apply color mode
                DBG("Note ON: " << noteNumber << " Color = " << col.toString());
                parameterManager.setParameterValue(ParameterIDs::mainR, coef * col.getRed());
                parameterManager.setParameterValue(ParameterIDs::mainG, coef * col.getGreen());
                parameterManager.setParameterValue(ParameterIDs::mainB, coef * col.getBlue());

            }
            else
            {
                DBG("Note ON: " << noteNumber << " Velocity: " << velocity);
            }
        }
        else if (message.isNoteOff())
        {
            // Message Note Off reçu
            auto noteNumber = message.getNoteNumber();
            (void)noteNumber;

            DBG("Note OFF: " << noteNumber);
        }
        else if (message.isController())
        {
            // Message Control Change reçu
            auto controllerNumber = message.getControllerNumber();
            auto controllerValue = message.getControllerValue();
            (void)controllerNumber;
            (void)controllerValue;

            DBG("CC: " << controllerNumber << " Value: " << controllerValue);
        }
        else if (message.isPitchWheel())
        {
            // Message Pitch Bend reçu
            auto pitchWheelValue = message.getPitchWheelValue();
            (void)pitchWheelValue;

            DBG("Pitch Wheel: " << pitchWheelValue);
        }
    }
}