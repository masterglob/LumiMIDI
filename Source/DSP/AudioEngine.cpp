
// ============================================================================
// Source/DSP/AudioEngine.cpp
// ============================================================================
#include "AudioEngine.h"
#include "Parameters/ParameterManager.h"
#include "UI/Resources/ColourPalette.h"
#include "DSP/BaseProgram.h"

#include "DSP/Programs/SimpleStroboscope.h"

#include <vector>

namespace
{
    using Led_RGBW = Led_RGBW;
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

    const LedVect demoLeds{
        &Led_G   ,&Led_M,  &Led_Sh1,  &Led_Sh2,  &Led_SQ, // Side 1
        &Led_Gs,  &Led_Ms, &Led_Sh1s, &Led_Sh2s, &Led_SQs, // Side 2
    };

    juce::Colour normalizeRgbw(LineValue r, LineValue g, LineValue b, LineValue w)
    {
        int R((r + w / 3) *2);
        if (R > 0xFF) R = 0xFF;
        int G((g + w / 3) * 2);
        if (G > 0xFF) G = 0xFF;
        int B((b + w / 3) * 2);
        if (B > 0xFF) B = 0xFF;
        return juce::Colour(static_cast<LineValue>(R), static_cast<LineValue>(G), static_cast<LineValue>(B));
    }

    static DefaultProgram& defaultProgram(DefaultProgram::instance());
    static PROGS::SimpleStroboscope simpleStroboscope;
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
        m.channel = TO_LINE_VALUE(1 + (led.mr >> 7));
        m.ccR = TO_LINE_VALUE(led.mr);
        m.ccG = TO_LINE_VALUE(led.mg);
        m.ccB = TO_LINE_VALUE(led.mb);
        m.ccW = TO_LINE_VALUE(led.mw);
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

void AudioEngine::setGlobalSpeedLevel(double level)
{
    if (level < 0.0) mSpeedLevel = 0.0f;
    else if (level > 1.0) mSpeedLevel = 1.0f;
    else  mSpeedLevel = static_cast<float>(level);
}


juce::Colour AudioEngine::getLedColor(LineId ledId) const
{
    static const juce::Colour unknown(0);
    juce::SpinLock::ScopedTryLockType lock(mColorLock);

    if (lock.isLocked() && ledId < NB_MAX_CMDS)
    {
        const LedMapping& m(mLedMapping[ledId]);
        const LineValue& r(mOutMidiCtxt.mOutputContext[m.ccR].lastSent);
        const LineValue& g(mOutMidiCtxt.mOutputContext[m.ccG].lastSent);
        const LineValue& b(mOutMidiCtxt.mOutputContext[m.ccB].lastSent);
        const LineValue& w(mOutMidiCtxt.mOutputContext[m.ccW].lastSent);
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

            if (noteNumber == 35)
            {
                mProgramManager.push(&simpleStroboscope, 5000);
                continue;
            }

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

            if (noteNumber == 35)
            {
                mProgramManager.pop(&simpleStroboscope);
                continue;
            }
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


void AudioEngine::OutputMidiContext::insertEvent(juce::MidiBuffer& midiMessages, LineId lineId, LineValue value)
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

void AudioEngine::ProgramManager::set(BaseProgram* program)
{
    juce::ScopedLock lock(mLock);
    mPrograms.clear();
    mPrograms.push_back({ program, 0 });
}

void AudioEngine::ProgramManager::push(BaseProgram* program, juce::uint32 duration)
{
    juce::ScopedLock lock(mLock);
    if (duration > 0)
    {
        auto endMs = juce::Time::getMillisecondCounter() + duration;
        mPrograms.push_back({ program, endMs });
    }
    else
    {
        mPrograms.push_back({ program, 0 });
    }
}


void AudioEngine::ProgramManager::pop(BaseProgram* program)
{
    juce::ScopedLock lock(mLock);
    if (mPrograms.empty()) return;
    auto item = mPrograms.back();
    if (item.first == program)
        mPrograms.pop_back();
}

void AudioEngine::ProgramManager::operator()( juce::MidiBuffer& newEvents)
{

    BaseProgram* prg{ nullptr };
    while (prg == nullptr)
    {
        juce::ScopedLock lock(mLock);
        if (mPrograms.empty()) break;

        auto item = mPrograms.back();
        if (item.second >0 && item.second <= juce::Time::getMillisecondCounter() || !item.first || item.first->done())
        {
            mPrograms.pop_back();
            continue;
        }
        prg = item.first;
    }

    if (prg == nullptr)
    {
        prg = &defaultProgram;
    }

    BaseProgram::Events events;
    events.reserve(256);

     prg->execute(demoLeds, mEngine.parameterManager, events);
    OutputMidiContext& midiCtx(mEngine.mOutMidiCtxt);

    for (const BaseProgram::Event& evt : events)
    {
        midiCtx.insertEvent(newEvents, evt.lineIdx, evt.value);
    }
}