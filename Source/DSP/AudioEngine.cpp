
// ============================================================================
// Source/DSP/AudioEngine.cpp
// ============================================================================
#include "AudioEngine.h"
#include "../Parameters/ParameterManager.h"
#include "../UI/Resources/ColourPalette.h"

#include <vector>

namespace
{
    using Led_RGBW = AudioEngine::Led_RGBW;
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

    const AudioEngine::LedVect demoLeds{
        &Led_G   ,&Led_M,  &Led_Sh1,  &Led_Sh2,  &Led_SQ, // Side 1
        &Led_Gs,  &Led_Ms, &Led_Sh1s, &Led_Sh2s, &Led_SQs, // Side 2
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

    class DefaultProgram : public AudioEngine::Program
    {
    public:
        DefaultProgram(void) = default;
        ~DefaultProgram(void) override = default;

    private:
        Events execute(const AudioEngine::LedVect& leds, const ParameterManager& parameterManager) override
        {
            Events result;
            result.reserve(256);

            static const float coef(127);
            const float mRed(parameterManager.getMainRed());
            const float mGreen(parameterManager.getMainGreen());
            const float mBlue(parameterManager.getMainBlue());
            const float mWhite(parameterManager.getMainWhite());
            // const float mHue(parameterManager.getMainHue());


            for (const Led_RGBW* pLed : leds)
            {
                if (!pLed) continue;
                const Led_RGBW& led(*pLed);
                result.emplace_back(led.mr, static_cast<unsigned char> (mRed * coef));
                result.emplace_back(led.mg, static_cast<unsigned char> (mGreen * coef));
                result.emplace_back(led.mb, static_cast<unsigned char> (mBlue * coef));
                const float fw = (mBlue + mGreen + mRed) * mWhite * coef;
                result.emplace_back(led.mw, static_cast<unsigned char> (fw));
            }
            return result;
        }

    };
    static DefaultProgram defaultProgram;
}

AudioEngine::AudioEngine(ParameterManager& paramManager)
    : parameterManager(paramManager),
      mProgramManager(*this)
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
    for (const Led_RGBW* pLed : demoLeds)
    {
        if (!pLed) continue;
        const Led_RGBW& led(*pLed);
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
    if (level < 0.0) mWhiteLevel = 0.0f;
    else if (level > 1.0) mWhiteLevel = 1.0f;
    else
    mWhiteLevel = static_cast<float>(level) * 0.33f;
}

void AudioEngine::setGlobalHueLevel(double level)
{
    if (level < 0.0) mHueLevel = 0.0f;
    else if (level > 1.0) mHueLevel = 1.0f;
    else  mHueLevel = static_cast<float>(level);
}


juce::Colour AudioEngine::getLedColor(unsigned short ledId) const
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

        mProgramManager(newEvents);
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

/**********************************************************************************/
AudioEngine::ProgramManager::ProgramManager(AudioEngine& engine):
    mEngine(engine)
{
}

void AudioEngine::ProgramManager::set(Program* program)
{
    juce::ScopedLock lock(mLock);
    mPrograms.clear();
    mPrograms.push_back(program);
}

void AudioEngine::ProgramManager::push(Program* program)
{
    juce::ScopedLock lock(mLock);
    mPrograms.push_back(program);
}

void AudioEngine::ProgramManager::operator()( juce::MidiBuffer& newEvents)
{

    Program* prg{ nullptr };
    while (prg == nullptr)
    {
        juce::ScopedLock lock(mLock);
        if (mPrograms.empty()) break;

        prg = mPrograms.back();
        if (prg->done())
        {
            mPrograms.pop_back();
            prg = nullptr;
        }
    }

    if (prg == nullptr)
    {
        prg = &defaultProgram;
    }

    Program::Events evts(prg->execute(demoLeds, mEngine.parameterManager));
    OutputMidiContext& midiCtx(mEngine.mOutMidiCtxt);

    for (const Program::Event& evt : evts)
    {
        midiCtx.insertEvent(newEvents, evt.lineIdx, evt.value);
    }
}