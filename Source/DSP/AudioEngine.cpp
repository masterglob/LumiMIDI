
// ============================================================================
// Source/DSP/AudioEngine.cpp
// ============================================================================
#include "AudioEngine.h"
#include "../Parameters/ParameterManager.h"

AudioEngine::AudioEngine(ParameterManager& paramManager)
    : parameterManager(paramManager)
{
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
            (void)noteNumber, velocity;
            // Ici vous pourriez :
            // - Enregistrer la note
            // - Déclencher des événements
            // - Mettre à jour des paramètres
            // - Envoyer des messages MIDI de sortie

            DBG("Note ON: " << noteNumber << " Velocity: " << velocity);
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
            (void)controllerNumber, controllerValue;

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