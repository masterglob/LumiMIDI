#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

class CustomMidiKeyboard : public juce::MidiKeyboardComponent {
public:
    CustomMidiKeyboard(juce::MidiKeyboardState& state, Orientation orientation);

    // Méthodes à override pour les couleurs personnalisées
    void drawWhiteNote(int midiNoteNumber,
        juce::Graphics& g,
        juce::Rectangle<float> area,
        bool isDown,
        bool isOver,
        juce::Colour lineColour,
        juce::Colour textColour) override;

    void drawBlackNote(int midiNoteNumber,
        juce::Graphics& g,
        juce::Rectangle<float> area,
        bool isDown,
        bool isOver,
        juce::Colour noteFillColour) override;

    // Méthodes pour définir les couleurs spécifiques
    void setNoteColour(int midiNote, juce::Colour colour);
    void clearNoteColour(int midiNote);

private:
    std::map<int, juce::Colour> noteColours;
    juce::Colour getNoteColour(int midiNote, bool isWhiteNote);
};
