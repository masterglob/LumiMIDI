
// ============================================================================
// Source/DSP/RandomSparkle.cpp
// ============================================================================
#include <random>
#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"
#include "UI/Resources/ColourPalette.h"

/**********************************************************************************/
namespace {
struct Context : public ProgramContext {
  Context() : randomEngine(std::random_device{}()) {}

  std::mt19937 randomEngine;
  juce::uint32 lastSparkleUpdate{0};
};
}  // namespace

namespace PROGS {
/**********************************************************************************/
RandomSparkle::RandomSparkle() : BaseProgram("RandomSparkle") {
  mContext.reset(new Context());
}

/**********************************************************************************/
void RandomSparkle::reset() {
  auto ctx(reinterpret_cast<::Context*>(mContext.get()));
  ctx->lastSparkleUpdate = 0;
}

/**********************************************************************************/
void RandomSparkle::execute(const LedVect& leds,
                            const ParameterManager& parameterManager,
                            BaseProgram::Events& events) {
  auto ctx(reinterpret_cast<::Context*>(mContext.get()));

  const juce::uint32 periodMs(floatToPeriod(parameterManager.getSpeed()));
  const juce::uint32 dtMs = elapsedMs();

  // Contr�le la fr�quence des scintillements bas� sur la vitesse
  const juce::uint32 sparkleIntervalMs = std::max(10u, periodMs / 20);

  // Ne met � jour les scintillements qu'� intervalles r�guliers
  if (dtMs >= (ctx->lastSparkleUpdate + sparkleIntervalMs)) {
    ctx->lastSparkleUpdate = dtMs;

    // Distribution pour probabilit� d'activation (30% de chance par LED)
    std::uniform_real_distribution<float> probDist(0.0f, 1.0f);
    // Distribution pour l'intensit�
    std::uniform_real_distribution<float> intensityDist(0.3f, 1.0f);

    // R�cup�ration des param�tres de couleur
    const float hue = parameterManager.getMainHue();
    const float baseRed = parameterManager.getMainRed();
    const float baseGreen = parameterManager.getMainGreen();
    const float baseBlue = parameterManager.getMainBlue();

    for (const LedContext* pLed : leds) {
      if (!pLed)
        continue;

      const LedCtrlLine& led(pLed->ctrl);

      // Chaque LED a une chance d'�tre activ�e
      if (probDist(ctx->randomEngine) < 0.1f) {
        // Intensit� al�atoire bas�e sur mVelocity
        float intensity =  // intensityDist(ctx->randomEngine) *
            mVelocity;

        // Scintillement principalement en blanc avec une teinte color�e
        events.emplace_back(led.mw, floatToCcValue(intensity));

        // Ajouter une variation color�e bas�e sur Hue et les couleurs de base
        // TODO: Remplacer par la fonction de transformation Hue quand
        // disponible
        float colorIntensity = intensity * 0.4f;  // Plus subtil en couleur
        float hueVariation =
            0.2f * probDist(ctx->randomEngine);  // Variation al�atoire �20%

        events.emplace_back(
            led.mr, floatToCcValue(colorIntensity * (baseRed + hueVariation)));
        events.emplace_back(led.mg, floatToCcValue(colorIntensity *
                                                   (baseGreen + hueVariation)));
        events.emplace_back(
            led.mb, floatToCcValue(colorIntensity * (baseBlue + hueVariation)));
      }
    }
  }
}

}  // namespace PROGS
