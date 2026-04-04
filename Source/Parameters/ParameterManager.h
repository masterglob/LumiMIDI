
// =============================================================================
// Parameters/ParameterManager.h
// =============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "ParameterIDs.h"

/*
 * sat=0.5 : simple normalize
 * sat= 1.0f white
 * sat = 0 : black
 */
juce::Colour floatHueParamToColor(float hue, float sat = 0.5f);

class ParameterManager {
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

  inline void setLowRms(float rms) { mLowRms = rms; }
  inline float getLowRms() const { return mLowRms; }
  // Accès rapide aux paramètres principaux
  float getMainWhite() const;
  float getMainHue() const;
  float getMainSat() const;
  float getFx1Hue() const;
  float getFx2Hue() const;
  float getFxPos() const;
  juce::Colour getMainHueColor() const;
  juce::Colour getFx1HueColor() const;
  juce::Colour getFx2HueColor() const;
  float getSpeed() const;
  float getPhase() const;

 private:
  juce::AudioProcessorValueTreeState parameters;
  float mLowRms{0.0f};

  // Création des paramètres
  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterManager)
};
