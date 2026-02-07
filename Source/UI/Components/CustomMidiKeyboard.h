#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>


class CustomMidiKeyboard : public juce::MidiKeyboardComponent {
 public:
  CustomMidiKeyboard(juce::MidiKeyboardState& state, Orientation orientation);

  // M�thodes � override pour les couleurs personnalis�es
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

  // M�thodes pour d�finir les couleurs sp�cifiques
  void setNoteColour(int midiNote, juce::Colour colour);
  void clearNoteColour(int midiNote);

 private:
  std::map<int, juce::Colour> noteColours;
  juce::Colour getNoteColour(int midiNote, bool isWhiteNote);
};
