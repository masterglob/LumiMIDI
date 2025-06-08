// =============================================================================
// PluginProcessor.h
// =============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "DSP/AudioEngine.h"
#include "Parameters/ParameterManager.h"

class LumiMIDIProcessor : public juce::AudioProcessor
{
public:
    LumiMIDIProcessor();
    ~LumiMIDIProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Accès aux paramètres
    ParameterManager& getParameterManager() { return parameterManager; }
    AudioEngine& getAudioEngine() { return audioEngine; }
private:
    ParameterManager parameterManager;
    AudioEngine audioEngine;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LumiMIDIProcessor)
};