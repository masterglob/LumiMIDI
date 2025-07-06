
// ============================================================================
// Source/DSP/AudioEngine.cpp
// ============================================================================
#include "AudioEngine.h"
#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"
#include "UI/Resources/ColourPalette.h"

#include <vector>

namespace {
using LedVect = std::vector<const LedContext*>;
const LedCtrlLine Led_Line_G(9, 2);
const LedCtrlLine Led_Line_M(17, 2);
const LedCtrlLine Led_Line_Sh1(25, 2);
const LedCtrlLine Led_Line_Sh2(33, 2);
const LedCtrlLine Led_Line_SQ(41, 2);
const LedCtrlLine Led_Line_Gs(105, 2);
const LedCtrlLine Led_Line_Ms(113, 2);
const LedCtrlLine Led_Line_Sh1s(97, 2);
const LedCtrlLine Led_Line_Sh2s(89, 2);
const LedCtrlLine Led_Line_SQs(81, 2);

static const int LedWDemo(550);
const LedContext Led_G{"G", Led_Line_G, Rect(100, 100, 100, -50)};
const LedContext Led_M{"M", Led_Line_M, Rect(100, 150, 150, 100)};
const LedContext Led_Sh1{"Sh1", Led_Line_Sh1, Rect(100, 120, 0, 130)};
const LedContext Led_Sh2{"Sh2", Led_Line_Sh2, Rect(200, 70, 0, 130)};
const LedContext Led_SQ{"SQ", Led_Line_SQ, Rect(280, 200, 0, 200)};

const LedContext Led_Gs{"Gs", Led_Line_Gs,
                        Rect(LedWDemo - 100, 100, -100, -50)};
const LedContext Led_Ms{"Ms", Led_Line_Ms,
                        Rect(LedWDemo - 100, 150, -150, 100)};
const LedContext Led_Sh1s{"Sh1s", Led_Line_Sh1s,
                          Rect(LedWDemo - 100, 120, 0, 130)};
const LedContext Led_Sh2s{"Sh2s", Led_Line_Sh2s,
                          Rect(LedWDemo - 200, 70, 0, 130)};
const LedContext Led_SQs{"SQs", Led_Line_SQs,
                         Rect(LedWDemo - 280, 200, 0, 200)};

const LedVect demoLeds{
    &Led_G,  &Led_M,  &Led_Sh1,  &Led_Sh2,  &Led_SQ,   // Side 1
    &Led_Gs, &Led_Ms, &Led_Sh1s, &Led_Sh2s, &Led_SQs,  // Side 2
};

juce::Colour normalizeRgbw(LineValue r, LineValue g, LineValue b) {
  int R(r * 2);
  if (R > 0xFF)
    R = 0xFF;
  int G(g * 2);
  if (G > 0xFF)
    G = 0xFF;
  int B(b * 2);
  if (B > 0xFF)
    B = 0xFF;
  return juce::Colour(static_cast<LineValue>(R), static_cast<LineValue>(G),
                      static_cast<LineValue>(B));
}

static PROGS::SimpleStroboscope simpleStroboscope;
static PROGS::SimpleWave simpleWave;
static PROGS::RandomSparkle randomSparkle;
}  // namespace

AudioEngine::AudioEngine(ParameterManager& paramManager)
    : parameterManager(paramManager), mProgramManager(*this) {
  int note{ColourPalette::colorPaletteFirstNote};

  for (const juce::Colour& col : ColourPalette::getBalancedSatColors()) {
    noteColours[note] = col;
    note = ColourPalette::getNextWhiteKey(note);
  }
  for (const juce::Colour& col : ColourPalette::getBalancedHueColors()) {
    noteColours[note] = col;
    note = ColourPalette::getNextWhiteKey(note);
  }
}

void AudioEngine::prepareToPlay(double sampleRate, int samplesPerBlock) {
  currentSampleRate = sampleRate;
  currentBlockSize = samplesPerBlock;

  // Setup LED mapping

  juce::SpinLock::ScopedTryLockType lock(mColorLock);
  mLeds.clear();
  for (const LedContext* it : demoLeds) {
    mLeds.push_back(it);
  }
}

void AudioEngine::releaseResources() {
  // Nettoyer les ressources si nécessaire
}

void AudioEngine::processBlock(juce::AudioBuffer<float>& buffer,
                               juce::MidiBuffer& midiMessages) {
  // Effacer le buffer audio (pas de génération d'audio)
  buffer.clear();

  // Traiter les messages MIDI entrants
  processMidiMessages(midiMessages);
}

void AudioEngine::processBlock(juce::AudioBuffer<double>& buffer,
                               juce::MidiBuffer& midiMessages) {
  // Effacer le buffer audio (pas de génération d'audio)
  buffer.clear();

  // Traiter les messages MIDI entrants
  processMidiMessages(midiMessages);
}

/**********************************************************************************/
void AudioEngine::learn(const juce::MidiMessage& message) {
  mLearning = false;
  mMessage = message.getDescription();
}

/**********************************************************************************/
void AudioEngine::setGlobalWhiteLevel(double level) {
  if (level < 0.0)
    mWhiteLevel = 0.0f;
  else if (level > 1.0)
    mWhiteLevel = 1.0f;
  else
    mWhiteLevel = static_cast<float>(level) * 0.33f;
}

/**********************************************************************************/
void AudioEngine::setGlobalHueLevel(double level) {
  if (level < 0.0)
    mHueLevel = 0.0f;
  else if (level > 1.0)
    mHueLevel = 1.0f;
  else
    mHueLevel = static_cast<float>(level);
}

/**********************************************************************************/
void AudioEngine::setGlobalSpeedLevel(double level) {
  if (level < 0.0)
    mSpeedLevel = 0.0f;
  else if (level > 1.0)
    mSpeedLevel = 1.0f;
  else
    mSpeedLevel = static_cast<float>(level);
}

/**********************************************************************************/
juce::Colour AudioEngine::getLedColor(LedId ledId) const {
  static const juce::Colour unknown(0);
  juce::SpinLock::ScopedTryLockType lock(mColorLock);

  if (lock.isLocked() && ledId < NB_MAX_LEDS) {
    const LedContext* led{mLeds[ledId]};
    const LedCtrlLine& m(led->ctrl);

    const LineValue& r(mOutMidiCtxt.mOutputContext[m.mr].lastSent);
    const LineValue& g(mOutMidiCtxt.mOutputContext[m.mg].lastSent);
    const LineValue& b(mOutMidiCtxt.mOutputContext[m.mb].lastSent);
    return normalizeRgbw(r, g, b);
  }
  return unknown;
}

/**********************************************************************************/
juce::Colour AudioEngine::getLedWhite(LedId ledId) const {
  static const juce::Colour unknown(0);
  juce::SpinLock::ScopedTryLockType lock(mColorLock);

  if (lock.isLocked() && ledId < NB_MAX_LEDS) {
    const LedContext* led{mLeds[ledId]};
    const LedCtrlLine& m(led->ctrl);

    const LineValue& w(mOutMidiCtxt.mOutputContext[m.mw].lastSent);
    return normalizeRgbw(w, w, w);
  }
  return unknown;
}

/**********************************************************************************/
void AudioEngine::processMidiMessages(juce::MidiBuffer& midiMessages) {
  juce::MidiBuffer newEvents;
  // Parcourir tous les messages MIDI du buffer
  for (const auto metadata : midiMessages) {
    juce::MidiMessage message = metadata.getMessage();
    if (mLearning) {
      learn(message);
    }

    // Exemple de traitement des messages MIDI
    if (message.isNoteOn()) {
      // Message Note On reçu
      auto noteNumber = message.getNoteNumber();
      auto velocity = message.getVelocity();
      (void)noteNumber;
      (void)velocity;

      if (noteNumber == 35) {
        mProgramManager.pushFx(&simpleStroboscope, velocity, 5000);
        continue;
      }
      if (noteNumber == 34) {
        mProgramManager.pushFx(&simpleWave, velocity);
        continue;
      }
      if (noteNumber == 33) {
        mProgramManager.pushFx(&randomSparkle, velocity);
        continue;
      }

      auto it(noteColours.find(noteNumber));
      if (it != noteColours.end()) {
        static const float coef(1 / 256.0);
        const juce::Colour col(it->second);
        // Apply color mode
        DBG("Note ON: " << noteNumber << " Color = " << col.toString());
        const float fRed(coef * col.getRed());
        const float fGreen(coef * col.getGreen());
        const float fBlue(coef * col.getBlue());
        DBG("R=" << col.getRed() << ", G=" << col.getGreen()
                 << ", B=" << col.getBlue());
        DBG("R=" << fRed << ", G=" << fGreen << ", B=" << fBlue);
        parameterManager.setParameterValue(ParameterIDs::mainR, fRed);
        parameterManager.setParameterValue(ParameterIDs::mainG, fGreen);
        parameterManager.setParameterValue(ParameterIDs::mainB, fBlue);

      } else {
        DBG("Note ON: " << noteNumber << " Velocity: " << velocity);
      }
    } else if (message.isNoteOff()) {
      // Message Note Off reçu
      auto noteNumber = message.getNoteNumber();
      (void)noteNumber;

      DBG("Note OFF: " << noteNumber);

      if (noteNumber == 35) {
        mProgramManager.popFx(&simpleStroboscope);
        continue;
      }
      if (noteNumber == 34) {
        mProgramManager.popFx(&simpleWave);
        continue;
      }
      if (noteNumber == 33) {
        mProgramManager.popFx(&randomSparkle);
        continue;
      }
    } else if (message.isController()) {
      // Message Control Change reçu
      auto controllerNumber = message.getControllerNumber();
      auto controllerValue = message.getControllerValue();
      (void)controllerNumber;
      (void)controllerValue;

      DBG("CC: " << controllerNumber << " Value: " << controllerValue);
    } else if (message.isPitchWheel()) {
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

/**********************************************************************************/
void AudioEngine::OutputMidiContext::insertEvent(juce::MidiBuffer& midiMessages,
                                                 LineId lineId,
                                                 LineValue value) {
  if (lineId >= NB_MAX_CMDS)
    return;
  OutputMidiMsg& line(mOutputContext[lineId]);

  if (line.lastSent != value) {
    midiMessages.addEvent(
        juce::MidiMessage::controllerEvent(line.channel + 1, lineId, value), 0);
    line.lastSent = value;
    if (lineId == 9) {
      DBG("Sent CH= " << static_cast<int>(line.channel + 1) << ", lineId="
                      << std::to_string(lineId) << ", val=" << value);
    }
  }
}

/**********************************************************************************/
AudioEngine::ProgramManager::ProgramManager(AudioEngine& engine)
    : mEngine(engine) {}

/**********************************************************************************/
void AudioEngine::ProgramManager::set(BaseProgram* program) {
  juce::ScopedLock lock(mLock);
  mMainProgram = program;
  mOverlayProgram = {nullptr, 0};
}

/**********************************************************************************/
void AudioEngine::ProgramManager::pushFx(BaseProgram* program,
                                         CCValue velocity,
                                         juce::uint32 duration) {
  if (!program)
    return;

  program->reset(velocity);

  juce::ScopedLock lock(mLock);
  if (duration > 0) {
    auto endMs = juce::Time::getMillisecondCounter() + duration;
    mOverlayProgram = {program, endMs};
  } else {
    mOverlayProgram = {program, 0};
  }
}

/**********************************************************************************/
void AudioEngine::ProgramManager::popFx(const BaseProgram* program) {
  juce::ScopedLock lock(mLock);
  if (mOverlayProgram.first == program)
    mOverlayProgram = {nullptr, 0};
}

/**********************************************************************************/
void AudioEngine::ProgramManager::operator()(juce::MidiBuffer& newEvents) {
  if (mMainProgram == nullptr) {
    static PROGS::DefaultProgram defaultProgram;
    mMainProgram = &defaultProgram;
  }

  BaseProgram::Events events;
  events.reserve(256);

  mMainProgram->execute(demoLeds, mEngine.parameterManager, events);

  {
    juce::ScopedLock lock(mLock);
    if (mOverlayProgram.first) {
      if ((mOverlayProgram.second > 0 &&
           mOverlayProgram.second <= juce::Time::getMillisecondCounter()) ||
          mOverlayProgram.first->done()) {
        DBG("Stopping program: " << mOverlayProgram.first->name);
        mOverlayProgram = {nullptr, 0};
      } else
        mOverlayProgram.first->execute(demoLeds, mEngine.parameterManager,
                                       events);
    }
  }
  OutputMidiContext& midiCtx(mEngine.mOutMidiCtxt);

  for (const BaseProgram::Event& evt : events) {
    midiCtx.insertEvent(newEvents, evt.lineIdx, evt.value);
  }
}
