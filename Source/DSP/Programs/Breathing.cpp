// =============================================================================
// Source/DSP/Programs/Breathing.cpp
// =============================================================================
#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"
#include <cmath>

namespace PROGS {

    Breathing::Breathing() : BaseProgram("Breathing") {
    }

    void Breathing::reset() {
        // Reset any internal state if needed
        // For breathing effect, we rely on elapsedMs() so no internal state needed
    }

    void Breathing::execute(const LedVect& leds,
        const ParameterManager& parameterManager,
        BaseProgram::Events& events) {

        // Get parameters
        const float mainRed = parameterManager.getMainRed();
        const float mainGreen = parameterManager.getMainGreen();
        const float mainBlue = parameterManager.getMainBlue();
        const float mainWhite = parameterManager.getMainWhite();
        const float speed = parameterManager.getSpeed();

        // TODO : could be some controls
        static const float minValue(0.3f);
        static const float amplitude = 1.0f;
        static const float speedFactor = 8.0f;
        static const float colPhase = 0.1f / 4.0f;
        static const float twoPi = 2.0f * juce::MathConstants<float>::pi;

        // Calculate breathing intensity using sine wave
        const float timeMs = static_cast<float>(elapsedMs());
        const float period = floatToPeriod(speed) * speedFactor; // Convert speed to period in ms
        const float phaseR = (timeMs / period) * twoPi;
        const float phaseG = phaseR + colPhase * twoPi;
        const float phaseB = phaseG + colPhase * twoPi;
        const float phaseW = phaseB + colPhase * twoPi;

        // Sine wave from 0 to 1 (breathing effect)
        const float breathingIntensityR = (std::sin(phaseR) + 1.0f + minValue) * (0.5f + minValue / 2);
        const float breathingIntensityG = (std::sin(phaseG) + 1.0f + minValue) * (0.5f + minValue / 2);
        const float breathingIntensityB = (std::sin(phaseB) + 1.0f + minValue) * (0.5f + minValue / 2);
        const float breathingIntensityW = (std::sin(phaseW) + 1.0f + minValue) * (0.5f + minValue / 2);

        // Calculate final LED values
        const LineValue redValue = float01ToCcValue(mainRed * breathingIntensityR * amplitude);
        const LineValue greenValue = float01ToCcValue(mainGreen * breathingIntensityG * amplitude);
        const LineValue blueValue = float01ToCcValue(mainBlue * breathingIntensityB * amplitude);
        const LineValue whiteValue = float01ToCcValue(mainWhite * breathingIntensityW *amplitude);

        // Apply to all LEDs (global effect)
        for (const auto& led : leds) {
            events.emplace_back(led->ctrl.mr, redValue);
            events.emplace_back(led->ctrl.mg, greenValue);
            events.emplace_back(led->ctrl.mb, blueValue);
            if (led->ctrl.hasWhite) {
                events.emplace_back(led->ctrl.mw, whiteValue);
            }
        }

        // Breathing effect never stops automatically
        mDone = false;
    }

} // namespace PROGS