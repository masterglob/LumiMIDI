
// =============================================================================
// Parameters/ParameterManager.cpp
// =============================================================================
#include "ParameterManager.h"

ParameterManager::ParameterManager(juce::AudioProcessor& processor)
    : parameters(processor, nullptr, "LumiMIDI", createParameterLayout())
{
}

ParameterManager::~ParameterManager()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout ParameterManager::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Main Red
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParameterIDs::mainR,
        "Main Red",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100)) + "%"; }
    ));

    // Main Green
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParameterIDs::mainG,
        "Main Green",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100)) + "%"; }
    ));

    // Main Blue
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParameterIDs::mainB,
        "Main Blue",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100)) + "%"; }
    ));

    // Main White
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParameterIDs::mainW,
        "Main White",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100)) + "%"; }
    ));

    // Main Hue
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParameterIDs::mainHue,
        "Main Hue",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100)) + "%"; }
    ));
    
    return { params.begin(), params.end() };
}

float ParameterManager::getParameterValue(const juce::String& parameterID)
{
    auto* param = parameters.getRawParameterValue(parameterID);
    return param ? param->load() : 0.0f;
}

void ParameterManager::setParameterValue(const juce::String& parameterID, float value)
{
    auto* param = parameters.getParameter(parameterID);
    if (param)
        param->setValueNotifyingHost(param->convertTo0to1(value));
}

void ParameterManager::saveState(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    juce::AudioProcessor::copyXmlToBinary(*xml, destData);
}

void ParameterManager::loadState(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(juce::AudioProcessor::getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

float ParameterManager::getMainRed() const
{
    auto* param = parameters.getRawParameterValue(ParameterIDs::mainR);
    return param ? param->load() : 1.0f;
}

float ParameterManager::getMainGreen() const
{
    auto* param = parameters.getRawParameterValue(ParameterIDs::mainG);
    return param ? param->load() : 1.0f;
}

float ParameterManager::getMainBlue() const
{
    auto* param = parameters.getRawParameterValue(ParameterIDs::mainB);
    return param ? param->load() : 1.0f;
}
float ParameterManager::getMainWhite() const
{
    auto* param = parameters.getRawParameterValue(ParameterIDs::mainW);
    return param ? param->load() : 1.0f;
}

