
// ============================================================================
// UI/Components/WorldView.cpp
// ============================================================================

#include <map>
#include "WorldView.h"
#include "DSP/AudioEngine.h"

#define LED_CONTOUR 1

using namespace std;
namespace
{
    struct LedPos {
        int x;
        int y;
        int w;
        int h;
    };

    static const LedPos LedSq{ 200,200, 0, 100 };
    static const LedPos LedSh{ 150,170, 50, 100 };
    static const map<int, LedPos> demoLeds{ {0, LedSq}, {1,LedSh } };
}

UI_WorldView::UI_WorldView(juce::AudioProcessorValueTreeState& apvts, const AudioEngine& engine):
    mEngine(engine)
{
    (void)apvts;
}


void UI_WorldView::setup(juce::Component aComp, juce::Rectangle<int> bounds)
{
    (void)bounds;
    (void)aComp;
}
    
void UI_WorldView::paint(juce::Graphics& g)
{
    using namespace juce;
    static const float ledWidth(4.0f);
    static const Rectangle<int> r0(100,100, 600, 400);

    Graphics::ScopedSaveState state(g);

    g.reduceClipRegion(r0);

    // Gradient de fond
    ColourGradient gradient(Colour(0xff0a0a0e), 0, 0,
        Colour(0xff060110), static_cast<float>(getWidth()), static_cast<float>(getHeight()), false);
    g.setGradientFill(gradient);
    g.fillRect(r0);

    g.setColour(Colours::white);
    g.drawRect(r0);

    g.setColour(Colours::blue);
    g.setFont(FontOptions().withName("Arial").withPointHeight(32.0f).withStyle("Bold"));

    for (const auto& it : demoLeds)
    {
        const LedPos& led(it.second);
        juce::Colour col(mEngine.getLedColor(it.first));
        // DBG("col=" << col.getRed() << ", " << col.getGreen() << ", " << col.getBlue());
        float x0(static_cast<float>(r0.getX() + led.x));
        float y0(static_cast<float>(r0.getY() + led.y));

#if LED_CONTOUR
        g.setColour(juce::Colours::lightgrey);
#if LED_CONTOUR ==  1 // Complete
        x0 += 1.0f; y0 += 1.0f;
        g.drawLine(x0, y0, x0 - led.w, y0 - led.h, ledWidth);
        x0 -= 2.0f;
        g.drawLine(x0, y0, x0 - led.w, y0 - led.h, ledWidth);
        y0 -= 2.0f;
        g.drawLine(x0, y0, x0 - led.w, y0 - led.h, ledWidth);
        x0 += 2.0f;
        g.drawLine(x0, y0, x0 - led.w, y0 - led.h, ledWidth);
        x0 -= 1.0f; y0 += 1.0f;
#elif LED_CONTOUR == 2 // Light code but incomplete contour
        g.drawLine(x0, y0, x0 - led.w, y0 - led.h, ledWidth + 2.0f);
#endif
#endif   // #if LED_CONTOUR

        g.setColour(col);
        g.drawLine(x0, y0, x0 -led.w, y0 -led.h, ledWidth);
    }
}

void UI_WorldView::resized()  {}
