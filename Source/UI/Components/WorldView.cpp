
// ============================================================================
// UI/Components/WorldView.cpp
// ============================================================================

#include <map>
#include "WorldView.h"
#include "DSP/AudioEngine.h"
#include "DSP/BaseProgram.h"

#define LED_WITH_NAME 1

using namespace std;
namespace
{
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

void UI_WorldView::refreshLeds(void)
{
    mLedsMap.clear();
    LedId ledId(0);
    for (const LedContext* it : mEngine.getLeds())
    {
        mLedsMap.emplace(ledId, *it);
        ledId++;
    }
}
    
void UI_WorldView::paint(juce::Graphics& g)
{
    if (mModified)
    {
        refreshLeds();
    }
    using namespace juce;
    // Topleft coordinates
    static const int r0x(150);
    static const int r0y(100);
    static const int r0w(800);
    static const int r0h(400);
    static const Rectangle<int> r0(r0x, r0y, r0w, r0h);

    auto toXf = [](int x) { return r0x + static_cast<float>(x); };
    auto toYf = [](int y) { return r0h + r0y - static_cast<float>(y); };
    auto toXi = [](int x) { return r0x + x; };
    auto toYi = [](int y) { return r0h + r0y - y; };

    Graphics::ScopedSaveState state(g);


    // Gradient de fond
    ColourGradient gradient(Colour(0xff0a0a0e), 0, 0,
        Colour(0xff060110), static_cast<float>(getWidth()), static_cast<float>(getHeight()), false);

    g.setColour(Colours::white);
    g.drawRect(r0);

    Rectangle<int> r1(r0.reduced(1));
    g.reduceClipRegion(r1);

    g.setGradientFill(gradient);
    g.fillRect(r1);

    for (const auto& it : mLedsMap)
    {
        const LedContext& led(it.second);
        const LedPosition& pos(led.pos);
        juce::Colour col(mEngine.getLedColor(it.first));
        // DBG("col=" << col.getRed() << ", " << col.getGreen() << ", " << col.getBlue());
        float x0(toXf(pos.topLeft.getX()));
        float y0(toYf(pos.topLeft.getY()));

        float x1(x0 + pos.size.getX());
        float y1(y0 - pos.size.getY());
        const float ledWidth(static_cast<float>(led.width));
        static const float bw{ 1.5f };

        g.setColour(juce::Colours::lightgrey); // TODO : replace by white component!
        g.drawLine(x0 + bw, y0 + bw, x1 + bw, y1 + bw, ledWidth);
        g.drawLine(x0 - bw, y0 + bw, x1 - bw, y1 + bw, ledWidth);
        g.drawLine(x0 - bw, y0 - bw, x1 - bw, y1 - bw, ledWidth);
        g.drawLine(x0 + bw, y0 - bw, x1 + bw, y1 - bw, ledWidth);

        g.setColour(col);
        g.drawLine(x0, y0, x1, y1, ledWidth);
    }
#if LED_WITH_NAME
    g.setFont(FontOptions().withName("Arial").withPointHeight(15.0f).withStyle("Bold"));
    g.setColour(juce::Colours::white);
    for (const auto& it : mLedsMap)
    {
        const LedContext& led(it.second);
        const LedPosition& pos(led.pos);
        static const int wt(100);
        static const int ht(20);
        int xt(toXi(pos.center.getX()) - wt/3);
        int yt(toYi(pos.center.getY()) -  ht/2);

        /*
        g.setColour(juce::Colours::yellow);
        g.drawEllipse(xt,yt, wt, ht, 2.0f); */

        g.drawText(led.name,
            xt, yt, wt, ht, // zone (x, y, w, h)
            juce::Justification::centred);
    }
#endif
}

void UI_WorldView::resized()  {}
