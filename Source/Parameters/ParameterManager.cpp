
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
    
    // Paramètre de mise à l'échelle de la vélocité
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParameterIDs::velocityScale,
        "Velocity Scale",
        juce::NormalisableRange<float>(0.1f, 2.0f, 0.01f),
        1.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 2) + "x"; }
    ));
    
    // Paramètre de luminosité
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParameterIDs::brightness,
        "Brightness",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.8f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100)) + "%"; }
    ));
    
    // Paramètre de décalage de teinte
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParameterIDs::hueShift,
        "Hue Shift",
        juce::NormalisableRange<float>(0.0f, 360.0f, 1.0f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value)) + "°"; }
    ));
    
    // Activation du filtre de canal
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParameterIDs::channelFilterEnabled,
        "Channel Filter",
        false
    ));
    
    // Canal à filtrer
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        ParameterIDs::filteredChannel,
        "Filtered Channel",
        1, 16, 1,
        juce::String(),
        [](int value, int) { return juce::String(value); }
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

float ParameterManager::getVelocityScale() const
{
    auto* param = parameters.getRawParameterValue(ParameterIDs::velocityScale);
    return param ? param->load() : 1.0f;
}

float ParameterManager::getBrightness() const
{
    auto* param = parameters.getRawParameterValue(ParameterIDs::brightness);
    return param ? param->load() : 0.8f;
}

float ParameterManager::getHueShift() const
{
    auto* param = parameters.getRawParameterValue(ParameterIDs::hueShift);
    return param ? param->load() : 0.0f;
}

bool ParameterManager::isChannelFilterEnabled() const
{
    auto* param = parameters.getRawParameterValue(ParameterIDs::channelFilterEnabled);
    return param ? (param->load() > 0.5f) : false;
}

int ParameterManager::getFilteredChannel() const
{
    auto* param = parameters.getRawParameterValue(ParameterIDs::filteredChannel);
    return param ? int(param->load()) : 1;
}
