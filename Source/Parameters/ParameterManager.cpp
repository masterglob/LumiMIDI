
// =============================================================================
// Parameters/ParameterManager.cpp
// =============================================================================
#include "ParameterManager.h"

ParameterManager::ParameterManager(juce::AudioProcessor& processor)
    : parameters(processor, nullptr, "LumiMIDI", createParameterLayout()) {
}

ParameterManager::~ParameterManager() {
}

juce::AudioProcessorValueTreeState::ParameterLayout ParameterManager::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

  // Main White
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      ParameterIDs::mainW,
      "Main White",
      juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
      1.0f,
      juce::String(),
      juce::AudioProcessorParameter::genericParameter,
      [](float value, int) { return juce::String(int(value * 100)) + "%"; }));

  // Main Hue
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      ParameterIDs::mainHue,
      "Main Hue",
      juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
      1.0f,
      juce::String(),
      juce::AudioProcessorParameter::genericParameter,
      [](float value, int) { return juce::String(int(value * 100)) + "%"; }));

  // Speed
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      ParameterIDs::speed,
      "Speed",
      juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
      1.0f,
      juce::String(),
      juce::AudioProcessorParameter::genericParameter,
      [](float value, int) { return juce::String(int(value * 100)) + "%"; }));

  // Phase
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      ParameterIDs::phase,
      "Phase",
      juce::NormalisableRange<float>(-0.5f, 0.5f, 0.01f),
      1.0f,
      juce::String(),
      juce::AudioProcessorParameter::genericParameter,
      [](float value, int) { return juce::String(int(value * 180)) + "�"; }));

  return {params.begin(), params.end()};
}

float ParameterManager::getParameterValue(const juce::String& parameterID) {
  auto* param = parameters.getRawParameterValue(parameterID);
  return param ? param->load() : 0.0f;
}

void ParameterManager::setParameterValue(const juce::String& parameterID, float value) {
  auto* param = parameters.getParameter(parameterID);
  if (param) param->setValueNotifyingHost(param->convertTo0to1(value));
}

void ParameterManager::saveState(juce::MemoryBlock& destData) {
  auto state = parameters.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  juce::AudioProcessor::copyXmlToBinary(*xml, destData);
}

void ParameterManager::loadState(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(juce::AudioProcessor::getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(parameters.state.getType()))
      parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

float ParameterManager::getMainWhite() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::mainW);
  return param ? param->load() : 1.0f;
}
float ParameterManager::getMainHue() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::mainHue);
  return param ? param->load() : 1.0f;
}

juce::Colour ParameterManager::getHueColor() const {
  const float hue = getMainHue();
#if 0
  return juce::Colour::fromHSV(hue, 1.0f, 0.8f, 1.0f);  // Correct green for isual consistency
#else
  juce::Colour c = juce::Colour::fromHSV(hue, 1.0f, 1.0f, 1.0f);

  float r = c.getFloatRed();
  float g = c.getFloatGreen();
  float b = c.getFloatBlue();

  // Compression douce du vert
  g = std::pow(g, 1.15f);

  // Boost du bleu (non destructif)
  b *= 1.25f;

  // Légère réduction globale pour éviter le clamp
  constexpr float gain = 0.85f;

  return juce::Colour::fromFloatRGBA(juce::jlimit(0.0f, 1.0f, r * gain),
                                     juce::jlimit(0.0f, 1.0f, g * gain),
                                     juce::jlimit(0.0f, 1.0f, b * gain),
                                     1.0f);
#endif
}

float ParameterManager::getSpeed() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::speed);
  return param ? param->load() : 1.0f;
}
float ParameterManager::getPhase() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::phase);
  return param ? param->load() : 1.0f;
}
