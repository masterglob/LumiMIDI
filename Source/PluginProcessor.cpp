
// =============================================================================
// PluginProcessor.cpp
// =============================================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"

LumiMIDIProcessor::LumiMIDIProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
       parameterManager(*this),
       audioEngine(parameterManager)
{
}

LumiMIDIProcessor::~LumiMIDIProcessor()
{
}

const juce::String LumiMIDIProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LumiMIDIProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LumiMIDIProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LumiMIDIProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double LumiMIDIProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LumiMIDIProcessor::getNumPrograms()
{
    return 1;
}

int LumiMIDIProcessor::getCurrentProgram()
{
    return 0;
}

void LumiMIDIProcessor::setCurrentProgram (int index)
{
    (void)index;
}

const juce::String LumiMIDIProcessor::getProgramName (int index)
{
    (void)index;
    return {};
}

void LumiMIDIProcessor::changeProgramName (int index, const juce::String& newName)
{
    (void)index;
    (void)newName;
}

void LumiMIDIProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    audioEngine.prepareToPlay(sampleRate, samplesPerBlock);
}

void LumiMIDIProcessor::releaseResources()
{
    audioEngine.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LumiMIDIProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void LumiMIDIProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    {
        juce::ScopedLock lock(midiEventLock);
        midiMessages.addEvents(pendingMidiEvents, 0, buffer.getNumSamples(), 0);
        pendingMidiEvents.clear();
    }

    // Traitement MIDI avec l'AudioEngine
    audioEngine.processBlock(buffer, midiMessages);

    {
        juce::ScopedLock lock(midiEventLock);
        for (const auto metadata : pendingDirectMidiEvents)
        {
            auto message = metadata.getMessage();
            midiMessages.addEvent(message, metadata.samplePosition);
        }
                
        pendingDirectMidiEvents.clear();
    }
}

void LumiMIDIProcessor::addMidiEvent(const juce::MidiMessage& message)
{
    juce::ScopedLock lock(midiEventLock);
    pendingMidiEvents.addEvent(message, 0);
}

void LumiMIDIProcessor::sendDirectMidiEvent(const juce::MidiMessage& message)
{
    juce::ScopedLock lock(midiEventLock);
    pendingDirectMidiEvents.addEvent(message, 0);

}

void LumiMIDIProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    {
        juce::ScopedLock lock(midiEventLock);
        midiMessages.addEvents(pendingMidiEvents, 0, buffer.getNumSamples(), 0);
        pendingMidiEvents.clear();
    }

    // Traitement MIDI avec l'AudioEngine
    audioEngine.processBlock(buffer, midiMessages);
}

bool LumiMIDIProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* LumiMIDIProcessor::createEditor()
{
    return new LumiMIDIEditor (*this, parameterManager.getParameters());
}

void LumiMIDIProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    parameterManager.saveState(destData);
}

void LumiMIDIProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    parameterManager.loadState(data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LumiMIDIProcessor();
}