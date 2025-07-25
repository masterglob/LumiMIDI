
// ============================================================================
// UI/Components/FilterSection.cpp
// ============================================================================
#include "FilterSection.h"

FilterSection::FilterSection(juce::AudioProcessorValueTreeState& apvts)
//    : cutoffKnob("Cutoff", apvts, "cutoff"),
//      resonanceKnob("Resonance", apvts, "resonance")
{
  (void)apvts;
  /*
  // Section label
  sectionLabel.setText("FILTER", juce::dontSendNotification);
  sectionLabel.setJustificationType(juce::Justification::centred);
  sectionLabel.setFont(juce::FontOptions().withName("Arial").withPointHeight(14.0f).withStyle("Bold"));
  addAndMakeVisible(sectionLabel);

  // Ajouter les knobs
  addAndMakeVisible(cutoffKnob);
  addAndMakeVisible(resonanceKnob);*/
}

void FilterSection::paint(juce::Graphics& g) {
  (void)g; /*
   auto bounds = getLocalBounds().toFloat();

   // Dessiner un cadre autour de la section
   g.setColour(juce::Colours::white.withAlpha(0.1f));
   g.drawRoundedRectangle(bounds.reduced(2), 4.0f, 1.0f); */
}

void FilterSection::
    resized() { /*
                   auto bounds = getLocalBounds();
                   bounds.reduce(10, 10);

                   sectionLabel.setBounds(bounds.removeFromTop(25));
                   bounds.removeFromTop(5); // Espacement

                   auto knobArea = bounds;
                   auto knobWidth = knobArea.getWidth() / 2;

                   cutoffKnob.setBounds(knobArea.removeFromLeft(knobWidth).reduced(5));
                   resonanceKnob.setBounds(knobArea.reduced(5));*/
}
