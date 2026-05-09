
// =============================================================================
// Parameters/ParameterManager.cpp
// =============================================================================
#include "ParameterManager.h"

juce::Colour floatHueParamToColor(float hue, float sat) {
#if 0
  return juce::Colour::fromHSV(hue, 1.0f, 0.8f, 1.0f);  // Correct green for isual consistency
#else
  juce::Colour c = juce::Colour::fromHSV(hue, 1.0f, 1.0f, 1.0f);

  float r = c.getFloatRed() * sat * 2;
  float g = c.getFloatGreen() * sat * 2;
  float b = c.getFloatBlue() * sat * 2;
  /*
  // Compression douce du vert
  g = std::pow(g, 1.15f);

  // Boost du bleu (non destructif)
  b *= 1.25f;

  // Légère réduction globale pour éviter le clamp
  constexpr float gain = 0.85f;
  */

  return juce::Colour::fromFloatRGBA(juce::jlimit(0.0f, 1.0f, r),
                                     juce::jlimit(0.0f, 1.0f, g),
                                     juce::jlimit(0.0f, 1.0f, b), 1.0f);
#endif
}

ParameterManager::ParameterManager(juce::AudioProcessor& processor)
    : parameters(processor, nullptr, "LumiMIDI", createParameterLayout()) {}

ParameterManager::~ParameterManager() {}

juce::AudioProcessorValueTreeState::ParameterLayout
ParameterManager::createParameterLayout() {
  using APF = juce::AudioParameterFloat;
  using Attr = juce::AudioParameterFloatAttributes;

  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

  // Factory locale
  auto addFloat = [&](const juce::String& id, const juce::String& name,
                      float min, float max, float step, float def,
                      const juce::String& label,
                      std::function<juce::String(float)> toText) {
    auto range = juce::NormalisableRange<float>(min, max, step);

    Attr attr;
    attr = attr.withLabel(label);

    if (toText) {
      attr = attr.withStringFromValueFunction(
          [toText](float v, int) { return toText(v); });
    }

    params.push_back(std::make_unique<APF>(juce::ParameterID(id, 1), name,
                                           range, def, attr));
  };

  // ================= PARAMETERS =================

  addFloat(ParameterIDs::maxPow, "Max Power", 0.f, 1.f, 0.01f, 1.f, "%",
           [](float v) { return juce::String(int(v * 100)); });

  addFloat(ParameterIDs::mainW, "Main White", 0.f, 1.f, 0.01f, 1.f, "%",
           [](float v) { return juce::String(int(v * 100)); });

  addFloat(ParameterIDs::mainHue, "Main Hue", 0.f, 1.f, 0.01f, 1.f, "deg",
           [](float v) { return juce::String(int(v * 360)); });

  addFloat(ParameterIDs::mainSat, "Main Sat", 0.f, 1.f, 0.01f, 1.f, "%",
           [](float v) { return juce::String(int(v * 360)); });

  addFloat(ParameterIDs::fx1Hue, "Fx1 Hue", 0.f, 1.f, 0.01f, 1.f, "deg",
           [](float v) { return juce::String(int(v * 360)); });

  addFloat(ParameterIDs::fx2Hue, "Fx2 Hue", 0.f, 1.f, 0.01f, 1.f, "deg",
           [](float v) { return juce::String(int(v * 360)); });

  addFloat(ParameterIDs::speed, "Speed", 0.f, 1.f, 0.01f, 1.f, "%",
           [](float v) { return juce::String(int(v * 100)); });

  addFloat(ParameterIDs::phase, "Phase", -0.5f, 0.5f, 0.01f, 0.f, "deg",
           [](float v) { return juce::String(int(v * 180)); });

  return {params.begin(), params.end()};
}

float ParameterManager::getParameterValue(const juce::String& parameterID) {
  auto* param = parameters.getRawParameterValue(parameterID);
  return param ? param->load() : 0.0f;
}

void ParameterManager::setParameterValue(const juce::String& parameterID,
                                         float value) {
  auto* param = parameters.getParameter(parameterID);
  if (param)
    param->setValueNotifyingHost(param->convertTo0to1(value));
}

void ParameterManager::saveState(juce::MemoryBlock& destData) {
  auto state = parameters.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  juce::AudioProcessor::copyXmlToBinary(*xml, destData);
}

void ParameterManager::loadState(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      juce::AudioProcessor::getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(parameters.state.getType()))
      parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

float ParameterManager::getMaxPow() const {
  return 1.0f;
  /*
  *  For later use?
  auto* param = parameters.getRawParameterValue(ParameterIDs::maxPow);
  return param ? param->load() : 1.0f;
  */
}

float ParameterManager::getMainWhite() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::mainW);
  return param ? param->load() : 1.0f;
}

float ParameterManager::getMainHue() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::mainHue);
  return param ? param->load() : 1.0f;
}

float ParameterManager::getMainSat() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::mainSat);
  return param ? param->load() : 1.0f;
}

float ParameterManager::getFx1Hue() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::fx1Hue);
  return param ? param->load() : 1.0f;
}

float ParameterManager::getFx2Hue() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::fx2Hue);
  return param ? param->load() : 1.0f;
}

float ParameterManager::getFxPos() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::fxPos);
  return param ? param->load() : 1.0f;
}

juce::Colour ParameterManager::getMainHueColor() const {
  return floatHueParamToColor(getMainHue(), getMainSat());
}
juce::Colour ParameterManager::getFx1HueColor() const {
  return floatHueParamToColor(getFx1Hue());
}

juce::Colour ParameterManager::getFx2HueColor() const {
  return floatHueParamToColor(getFx2Hue());
}

float ParameterManager::getSpeed() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::speed);
  return param ? param->load() : 1.0f;
}
float ParameterManager::getPhase() const {
  auto* param = parameters.getRawParameterValue(ParameterIDs::phase);
  return param ? param->load() : 1.0f;
}
