
// ============================================================================
// Source/DSP/AudioEngine.cpp
// ============================================================================
#include "AudioEngine.h"
#include "../Parameters/ParameterManager.h"
#include "../UI/Resources/ColourPalette.h"

#include <vector>

namespace
{
    // TODO : move this somewhere smart
    class Base_Led
    {
    public:

    };
    class Led_RGBW : public Base_Led
    {
    public:
        Led_RGBW(unsigned int r, unsigned int g, unsigned int b, unsigned int w) : mr(r), mg(g), mb(b), mw(w) {}
        Led_RGBW(unsigned int i0, unsigned int delta) : mr(i0), mg(mr + delta), mb(mg + delta), mw(mb + delta) {}
        const unsigned int mr, mg, mb, mw;
    };

    const Led_RGBW Led_G(9, 2);
    const Led_RGBW Led_M(17, 2);
    const Led_RGBW Led_Sh1(25, 2);
    const Led_RGBW Led_Sh2(33, 2);
    const Led_RGBW Led_SQ(41, 2);

    const Led_RGBW Led_Gs(105, 2);
    const Led_RGBW Led_Ms(113, 2);
    const Led_RGBW Led_Sh1s(97, 2);
    const Led_RGBW Led_Sh2s(89, 2);
    const Led_RGBW Led_SQs(81, 2);

    static const std::vector<Led_RGBW> demoLeds{
        Led_G,Led_M,Led_Sh1,Led_Sh2,Led_SQ, // Side 1
        Led_Gs,Led_Ms,Led_Sh1s,Led_Sh2s,Led_SQs, // Side 2
    };

    juce::Colour normalizeRgbw(unsigned char r, unsigned char g, unsigned char b, unsigned char w)
    {
        int R((r + w / 3) *2);
        if (R > 0xFF) R = 0xFF;
        int G((g + w / 3) * 2);
        if (G > 0xFF) G = 0xFF;
        int B((b + w / 3) * 2);
        if (B > 0xFF) B = 0xFF;
        return juce::Colour(static_cast<unsigned char>(R), static_cast<unsigned char>(G), static_cast<unsigned char>(B));
    }
}

AudioEngine::AudioEngine(ParameterManager& paramManager)
    : parameterManager(paramManager)
{
    int note{ ColourPalette::colorPaletteFirstNote };

    for (const juce::Colour& col : ColourPalette::getBalancedSatColors())
    {
        noteColours[note] = col;
        note = ColourPalette::getNextWhiteKey(note);
    }
    for (const juce::Colour& col : ColourPalette::getBalancedHueColors())
    {
        noteColours[note] = col;
        note = ColourPalette::getNextWhiteKey(note);
    }
}

void AudioEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    memset(&mLedMapping[0], 0, sizeof(mLedMapping));

    // Setup LED mapping
    unsigned int ledId(0);
    for (const Led_RGBW& led : demoLeds)
    {
        if (ledId >= NB_MAX_CMDS) break;
        LedMapping& m(mLedMapping[ledId]);
        m.channel = static_cast<unsigned char>(1 + (led.mr >> 7));
        m.ccR = static_cast<unsigned char>(led.mr);
        m.ccG = static_cast<unsigned char>(led.mg);
        m.ccB = static_cast<unsigned char>(led.mb);
        m.ccW = static_cast<unsigned char>(led.mw);
        DBG("Create Led #" << ledId << " on CC:" << m.ccR << ", " << m.ccG << ", " << m.ccB << ", ");
        ledId++;
    }
}

void AudioEngine::releaseResources()
{
    // Nettoyer les ressources si nécessaire
}

void AudioEngine::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Effacer le buffer audio (pas de génération d'audio)
    buffer.clear();

    // Traiter les messages MIDI entrants
    processMidiMessages(midiMessages);
}

void AudioEngine::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    // Effacer le buffer audio (pas de génération d'audio)
    buffer.clear();

    // Traiter les messages MIDI entrants
    processMidiMessages(midiMessages);
}

void AudioEngine::learn(const juce::MidiMessage& message)
{
    mLearning = false;
    mMessage = message.getDescription();
}

void AudioEngine::setGlobalWhiteLevel(double level)
{
    mWhiteLevel = static_cast<float>(level) * 0.33f;
}


juce::Colour AudioEngine::getLedColor(unsigned int ledId) const
{
    static const juce::Colour unknown(0);
    juce::SpinLock::ScopedTryLockType lock(mColorLock);

    if (lock.isLocked() && ledId < NB_MAX_CMDS)
    {
        const LedMapping& m(mLedMapping[ledId]);
        const unsigned char& r(mOutMidiCtxt.mOutputContext[m.ccR].lastSent);
        const unsigned char& g(mOutMidiCtxt.mOutputContext[m.ccG].lastSent);
        const unsigned char& b(mOutMidiCtxt.mOutputContext[m.ccB].lastSent);
        const unsigned char& w(mOutMidiCtxt.mOutputContext[m.ccW].lastSent);
        // Todo : add W component
        return normalizeRgbw(r,g,b,w);
    }
    return unknown;
}

void AudioEngine::processMidiMessages(juce::MidiBuffer& midiMessages)
{
    juce::MidiBuffer newEvents;
    // Parcourir tous les messages MIDI du buffer
    for (const auto metadata : midiMessages)
    {
        juce::MidiMessage message = metadata.getMessage();
        if (mLearning)
        {
            learn(message);
        }

        // Exemple de traitement des messages MIDI
        if (message.isNoteOn())
        {
            // Message Note On reçu
            auto noteNumber = message.getNoteNumber();
            auto velocity = message.getVelocity();
            (void)noteNumber;
            (void)velocity;

            auto it(noteColours.find(noteNumber));
            if (it != noteColours.end())
            {
                static const float coef(1 / 256.0);
                const juce::Colour col(it->second);
                // Apply color mode
                DBG("Note ON: " << noteNumber << " Color = " << col.toString());
                const float fRed(coef * col.getRed());
                const float fGreen(coef * col.getGreen());
                const float fBlue(coef * col.getBlue());
                DBG("R=" << col.getRed() << ", G=" << col.getGreen() << ", B=" << col.getBlue());
                DBG("R=" << fRed << ", G=" << fGreen << ", B=" << fBlue);
                parameterManager.setParameterValue(ParameterIDs::mainR, fRed);
                parameterManager.setParameterValue(ParameterIDs::mainG, fGreen);
                parameterManager.setParameterValue(ParameterIDs::mainB, fBlue);

            }
            else
            {
                DBG("Note ON: " << noteNumber << " Velocity: " << velocity);
            }
        }
        else if (message.isNoteOff())
        {
            // Message Note Off reçu
            auto noteNumber = message.getNoteNumber();
            (void)noteNumber;

            DBG("Note OFF: " << noteNumber);
        }
        else if (message.isController())
        {
            // Message Control Change reçu
            auto controllerNumber = message.getControllerNumber();
            auto controllerValue = message.getControllerValue();
            (void)controllerNumber;
            (void)controllerValue;

            DBG("CC: " << controllerNumber << " Value: " << controllerValue);
        }
        else if (message.isPitchWheel())
        {
            // Message Pitch Bend reçu
            auto pitchWheelValue = message.getPitchWheelValue();
            (void)pitchWheelValue;

            DBG("Pitch Wheel: " << pitchWheelValue);
        }
    }

    {
        // Apply controls via MIDI to DMX

        static const float coef(127);
        const float mRed(parameterManager.getMainRed());
        const float mGreen(parameterManager.getMainGreen());
        const float mBlue(parameterManager.getMainBlue());

        // Demo : apply simple color to all Leds
        for (const Led_RGBW &led : demoLeds)
        {
            mOutMidiCtxt.insertEvent(newEvents, led.mr, static_cast<unsigned char> (mRed * coef));
            mOutMidiCtxt.insertEvent(newEvents, led.mg, static_cast<unsigned char> (mGreen * coef));
            mOutMidiCtxt.insertEvent(newEvents, led.mb, static_cast<unsigned char> (mBlue * coef));
            const float fw = (mBlue + mGreen + mRed) * mWhiteLevel * coef;
            mOutMidiCtxt.insertEvent(newEvents, led.mw, static_cast<unsigned char> (fw));
        }
    }
    newEvents.swapWith(midiMessages);
}


void AudioEngine::OutputMidiContext::insertEvent(juce::MidiBuffer& midiMessages, unsigned int lineId, unsigned char value)
{
    if (lineId >= NB_MAX_CMDS) return;
    OutputMidiMsg& line(mOutputContext[lineId]);

    if (line.lastSent != value)
    {
        midiMessages.addEvent(juce::MidiMessage::controllerEvent(line.channel + 1, lineId, value), 0);
        line.lastSent = value;
        if (lineId == 9) {
        DBG("Sent CH= " << static_cast<int>(line.channel + 1) << ", lineId=" << std::to_string(lineId) << ", val=" << value);
        }
    }
}