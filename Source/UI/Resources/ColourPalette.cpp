
// ============================================================================
// UI/Resources/ColourPalette.cpp
// ============================================================================

#include "ColourPalette.h"

namespace ColourPalette
{

    const ColorVect& getBalancedSatColors(void)
    {
        static const ColorVect cols{
    juce::Colour(0,   0, 0 ),    // Index 0
    juce::Colour(0,   0, 127),   
    juce::Colour(0,   0, 255),   
    juce::Colour(0, 127,   0),   
    juce::Colour(0, 127, 127),   
    juce::Colour(0, 127, 255),   
    juce::Colour(0, 255,   0),   
    juce::Colour(0, 255, 127),   
    juce::Colour(0, 255, 255),   
    juce::Colour(127,   0,   0), 
    juce::Colour(127,   0, 127), 
    juce::Colour(127,   0, 255), 
    juce::Colour(127, 127,   0), 
    juce::Colour(127, 127, 127), 
    juce::Colour(127, 127, 255), 
    juce::Colour(127, 255,   0), 
    juce::Colour(127, 255, 127), 
    juce::Colour(127, 255, 255), 
    juce::Colour(255,   0,   0), 
    juce::Colour(255,   0, 127), 
    juce::Colour(255,   0, 255), 
    juce::Colour(255, 127,   0), 
    juce::Colour(255, 127, 127), 
    juce::Colour(255, 127, 255), 
    juce::Colour(255, 255,   0), 
    juce::Colour(255, 255, 127), 
    juce::Colour(255, 255, 255)  
        };
        return cols;
    }
    const ColorVect& getBalancedHueColors(void)
    {
        static const ColorVect cols{
        juce::Colour(235, 70, 70),    // Hue 0°
        juce::Colour(207, 128, 76),   // Hue 24°
        juce::Colour(179, 160, 86),   // Hue 48°
        juce::Colour(160, 179, 86),   // Hue 72°
        juce::Colour(128, 207, 76),   // Hue 96°
        juce::Colour(70, 235, 70),    // Hue 120°
        juce::Colour(76, 207, 128),   // Hue 144°
        juce::Colour(86, 179, 160),   // Hue 168°
        juce::Colour(86, 160, 179),   // Hue 192°
        juce::Colour(76, 128, 207),   // Hue 216°
        juce::Colour(70, 70, 235),    // Hue 240°
        juce::Colour(128, 76, 207),   // Hue 264°
        juce::Colour(160, 86, 179),   // Hue 288°
        juce::Colour(179, 86, 160),   // Hue 312°
        juce::Colour(207, 76, 128)    // Hue 336°
        };
        return cols;
    }

    int getNextWhiteKey(int midiNote) {
        // Pattern of semitones between white keys: 2,2,1,2,2,2,1
        // C->D(+2), D->E(+2), E->F(+1), F->G(+2), G->A(+2), A->B(+2), B->C(+1)

        int noteInOctave = midiNote % 12;

        switch (noteInOctave) {
        case 0:  // C -> D
            return midiNote + 2;
        case 2:  // D -> E
            return midiNote + 2;
        case 4:  // E -> F
            return midiNote + 1;
        case 5:  // F -> G
            return midiNote + 2;
        case 7:  // G -> A
            return midiNote + 2;
        case 9:  // A -> B
            return midiNote + 2;
        case 11: // B -> C
            return midiNote + 1;
        default:
            // If input is a black key, find the next white key
            return getNextWhiteKey(midiNote + 1);
        }
    }
}