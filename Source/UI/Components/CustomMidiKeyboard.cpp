
#include "UI/Components/CustomMidiKeyboard.h"
#include "../Resources/ColourPalette.h"

CustomMidiKeyboard::CustomMidiKeyboard(juce::MidiKeyboardState& state,
    Orientation orientation)
    : MidiKeyboardComponent(state, orientation) {
    setKeyWidth(12.0f);
    setLowestVisibleKey(36);  // C2
    setKeyPressBaseOctave(4);

    int note{ 12 * 3 };

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
