
// ============================================================================
// UI/Components/CcSender.cpp
// ============================================================================

#include <map>
#include "CcSender.h"

#define LED_CONTOUR 1

using namespace std;
namespace
{
}

UI_CcSender::UI_CcSender(juce::AudioProcessorValueTreeState& apvts, const AudioEngine& engine):
    mEngine(engine)
{
    (void)apvts;
}


void UI_CcSender::setup(juce::Component aComp, juce::Rectangle<int> bounds)
{
    (void)bounds;
    (void)aComp;
}
    
void UI_CcSender::paint(juce::Graphics& g)
{
}

void UI_CcSender::resized()  {}
