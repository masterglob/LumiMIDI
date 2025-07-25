
// ============================================================================
// UI/LookAndFeel/CustomLookAndFeel.cpp
// ============================================================================
#include "CustomLookAndFeel.h"
#include "../Resources/ColourPalette.h"

CustomLookAndFeel::CustomLookAndFeel() {
  primaryColour = juce::Colour(0xff2d3142);
  secondaryColour = juce::Colour(0xff4f5d75);
  accentColour = juce::Colour(0xffef476f);

  // Couleurs par défaut
  setColour(juce::Slider::thumbColourId, accentColour);
  setColour(juce::Slider::trackColourId, secondaryColour);
  setColour(juce::Slider::backgroundColourId, primaryColour);
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                         int x,
                                         int y,
                                         int width,
                                         int height,
                                         float sliderPos,
                                         float rotaryStartAngle,
                                         float rotaryEndAngle,
                                         juce::Slider& slider) {
  (void)slider;
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
  auto toAngle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  auto centre = bounds.getCentre();

  // Fond du slider
  g.setColour(primaryColour);
  g.fillEllipse(bounds);

  // Track
  g.setColour(secondaryColour);
  g.drawEllipse(bounds, 2.0f);

  // Thumb
  juce::Path thumb;
  auto thumbWidth = radius * 0.1f;
  thumb.addRoundedRectangle(-thumbWidth / 2, -radius + 5, thumbWidth,
                            radius * 0.3f, 2.0f);
  g.setColour(accentColour);
  g.fillPath(thumb,
             juce::AffineTransform::rotation(toAngle).translated(centre));
}

void CustomLookAndFeel::drawButtonBackground(
    juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown) {
  auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
  auto baseColour =
      backgroundColour
          .withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
          .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

  if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
    baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

  g.setColour(baseColour);
  g.fillRoundedRectangle(bounds, 4.0f);
}

CustomMidiKeyboard::CustomMidiKeyboard(juce::MidiKeyboardState& state,
                                       Orientation orientation)
    : MidiKeyboardComponent(state, orientation) {
  setKeyWidth(12.0f);
  setLowestVisibleKey(36);  // C2
  setKeyPressBaseOctave(4);

  int note{12 * 3};

  for (const juce::Colour& col : ColourPalette::getBalancedSatColors()) {
    setNoteColour(note, col);
    note = ColourPalette::getNextWhiteKey(note);
  }
  for (const juce::Colour& col : ColourPalette::getBalancedHueColors()) {
    setNoteColour(note, col);
    note = ColourPalette::getNextWhiteKey(note);
  }
}

void CustomMidiKeyboard::setNoteColour(int midiNote, juce::Colour colour) {
  noteColours[midiNote] = colour;
  repaint();
}

void CustomMidiKeyboard::clearNoteColour(int midiNote) {
  noteColours.erase(midiNote);
  repaint();
}

juce::Colour CustomMidiKeyboard::getNoteColour(int midiNote, bool isWhiteNote) {
  auto it = noteColours.find(midiNote);
  if (it != noteColours.end())
    return it->second;

  return isWhiteNote ? juce::Colours::white : juce::Colours::black;
}

void CustomMidiKeyboard::drawWhiteNote(int midiNoteNumber,
                                       juce::Graphics& g,
                                       juce::Rectangle<float> area,
                                       bool isDown,
                                       bool isOver,
                                       juce::Colour lineColour,
                                       juce::Colour textColour) {
  (void)textColour;
  auto noteColour = getNoteColour(midiNoteNumber, true);

  if (isDown)
    noteColour = noteColour.darker(0.3f);
  else if (isOver)
    noteColour = noteColour.brighter(0.1f);

  g.setColour(noteColour);
  g.fillRect(area);

  g.setColour(lineColour);
  g.drawRect(area, 1.0f);
}

void CustomMidiKeyboard::drawBlackNote(int midiNoteNumber,
                                       juce::Graphics& g,
                                       juce::Rectangle<float> area,
                                       bool isDown,
                                       bool isOver,
                                       juce::Colour noteFillColour) {
  (void)noteFillColour;
  auto noteColour = getNoteColour(midiNoteNumber, false);

  if (isDown)
    noteColour = noteColour.brighter(0.3f);
  else if (isOver)
    noteColour = noteColour.brighter(0.1f);

  g.setColour(noteColour);
  g.fillRect(area);

  g.setColour(noteColour.darker(0.5f));
  g.drawRect(area, 1.0f);
}
