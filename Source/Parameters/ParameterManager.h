
// =============================================================================
// Parameters/ParameterManager.h
// =============================================================================
#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "ParameterIDs.h"

class ParameterManager
{
public:
    ParameterManager(juce::AudioProcessor& processor);
    ~ParameterManager();

    // Accès aux paramètres
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    
    // Méthodes utilitaires
    float getParameterValue(const juce::String& parameterID);
    void setParameterValue(const juce::String& parameterID, float value);
    
    // Sauvegarde/chargement de l'état
    void saveState(juce::MemoryBlock& destData);
    void loadState(const void* data, int sizeInBytes);
    
    // Accès rapide aux paramètres principaux
    float getMainRed() const;
    float getMainGreen() const;
    float getMainBlue() const;

private:
    juce::AudioProcessorValueTreeState parameters;
    
    // Création des paramètres
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterManager)
};