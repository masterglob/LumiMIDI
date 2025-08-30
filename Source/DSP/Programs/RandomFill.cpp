// =============================================================================
// Source/DSP/Programs/RandomFill.cpp
// =============================================================================

#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"
#include <cmath>
#include <vector>

/**********************************************************************************/
namespace {
juce::Colour getRandomColor() {
  // Generate a random hue between 0.0 and 1.0
  float hue = juce::Random::getSystemRandom().nextFloat();

  // Set saturation and brightness to 0.8 and 0.8 for more vibrant colors
  float saturation = 0.8f;
  float brightness = 0.8f;

  // Return a color from the generated HSV values
  return juce::Colour::fromHSV(hue, saturation, brightness, 1.0f);
}

    struct Context : public ProgramContext {
        Context() 
        {
            leds.reserve(100);
        }
        struct Data
        {
            Data() = delete;
            Data(juce::uint32 period) :
                color(getRandomColor()),
                periodMs(period) {
            }
            juce::Colour color;
            juce::uint32 periodMs;
        };
        std::vector<Data> leds;
    };


    const juce::uint32 periods[4] = { 250, 333, 500, 666 };
    }  // namespace

/******************************************************************/
namespace PROGS {

    RandomFill::RandomFill() : BaseProgram("RandomFill") {
    }

    void RandomFill::reset() {
        mContext.reset(new ::Context());
    }

    void RandomFill::execute(const LedVect& leds,
        const ParameterManager& parameterManager,
        BaseProgram::Events& events)
    {
        if (!mContext)
        {
            mContext.reset(new ::Context());
        }

        ::Context& ctx(*reinterpret_cast<::Context*>(mContext.get()));

        const juce::uint32 periodMs(floatToPeriod(parameterManager.getSpeed()));
        const juce::uint32 dtMs = elapsedMs();

        // Apply to all LEDs (global effect)
        size_t i{ 0 };
        
        for (const auto& led : leds) {
            if (ctx.leds.size() <= i)
            {
                ctx.leds.emplace_back(periods[i % 4]);
            }

            const ::Context::Data& data(ctx.leds[i]);

            if ((dtMs / data.periodMs) & 1)
            {
                events.emplace_back(led->ctrl.mr, FLOAT_TO_LINE_VALUE(data.color.getFloatRed()));
                events.emplace_back(led->ctrl.mg, FLOAT_TO_LINE_VALUE(data.color.getFloatGreen()));
                events.emplace_back(led->ctrl.mb, FLOAT_TO_LINE_VALUE(data.color.getFloatBlue()));
            }
            else
            {
                events.emplace_back(led->ctrl.mr, MIN_CC_VALUE);
                events.emplace_back(led->ctrl.mg, MIN_CC_VALUE);
                events.emplace_back(led->ctrl.mb, MIN_CC_VALUE);
            }

            if (led->ctrl.hasWhite) {
                events.emplace_back(led->ctrl.mw, MIN_CC_VALUE);
            }

            i++;
        }
    }

} // namespace PROGS
