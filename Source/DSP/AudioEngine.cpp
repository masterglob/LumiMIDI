
// ============================================================================
// Source/DSP/AudioEngine.cpp
// ============================================================================
#include "AudioEngine.h"

#include <vector>

#include "DSP/BaseProgram.h"
#include "Parameters/ParameterManager.h"
#include "UI/Resources/ColourPalette.h"

namespace {

const int CC_HUE_NUM{20};
const int CC_SAT_NUM{21};
const int CC_FX1_HUE_NUM{22};
const int CC_FX2_HUE_NUM{23};
const int CC_WHITE_NUM{24};
const int CC_SPEED_NUM{25};
const int CC_PHASE_NUM{26};

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

static PROGS::DefaultProgram defaultProgram;
static PROGS::SimpleStroboscope progSimpleStroboscope;
static PROGS::SimpleWave progSimpleWave;
static PROGS::SingleFlashFxC progSingleFlashFxC;
static PROGS::SingleFlashFxL progSingleFlashFxL;
static PROGS::SingleFlashFxR progSingleFlashFxR;
static PROGS::SingleFlashFxTop progSingleFlashFxTop;
static PROGS::SingleFlashFxBot progSingleFlashFxBot;
static PROGS::RandomSparkle progRandomSparkle;
static PROGS::Breathing progBreathing;
static PROGS::WarmCoolCycle progWarmCoolCycle;
static PROGS::RandomFill progRandomFill;
static PROGS::ZoneFlash sZoneFlash;
static PROGS::OppColors sOppColors;

const float thresholdLow = 0.05f;
const float thresholdHigh = 0.1f;
const int holdLowTimeSamples = 44100;  // 1 seconde @ 44.1 kHz
const float alphaLow = 0.05f;

template <typename T>
static inline float toFloat01(T v) noexcept {
  if (v <= static_cast<T>(0))
    return 0.f;
  if (v >= static_cast<T>(1))
    return 1.f;
  return static_cast<float>(v);
}
}  // namespace

AudioEngine::AudioEngine(ParameterManager& paramManager)
    : parameterManager(paramManager),
      mProgramManager(*this),
      mParamCtrl{},
      mLowFilter(75.0f, 1.0f),
      mLowTrigger(thresholdLow, thresholdHigh, holdLowTimeSamples, alphaLow) {
  mPendingUiMidiMsg.reserve(128);
  int note{ColourPalette::colorPaletteFirstNote};

  mProgramManager.updateLeds(mLeds.getAll());
  for (const juce::Colour& col : ColourPalette::getBalancedSatColors()) {
    noteColours[note] = col;
    note = ColourPalette::getNextWhiteKey(note);
  }
  for (const juce::Colour& col : ColourPalette::getBalancedHueColors()) {
    noteColours[note] = col;
    note = ColourPalette::getNextWhiteKey(note);
  }

  // Add controllers
  mParamCtrl.addParam(CC_HUE_NUM, [this](int ccVal) {
    parameterManager.setParameterValue(ParameterIDs::mainHue,
                                       juce::jlimit(0, 127, ccVal) / 127.0f);
  });
  mParamCtrl.addParam(CC_SAT_NUM, [this](int ccVal) {
    parameterManager.setParameterValue(ParameterIDs::mainSat,
                                       juce::jlimit(0, 127, ccVal) / 127.0f);
  });
  mParamCtrl.addParam(CC_FX1_HUE_NUM, [this](int ccVal) {
    parameterManager.setParameterValue(ParameterIDs::fx1Hue,
                                       juce::jlimit(0, 127, ccVal) / 127.0f);
  });
  mParamCtrl.addParam(CC_FX2_HUE_NUM, [this](int ccVal) {
    parameterManager.setParameterValue(ParameterIDs::fx2Hue,
                                       juce::jlimit(0, 127, ccVal) / 127.0f);
  });
  mParamCtrl.addParam(CC_WHITE_NUM, [this](int ccVal) {
    parameterManager.setParameterValue(ParameterIDs::mainW,
                                       juce::jlimit(0, 127, ccVal) / 127.0f);
  });
  mParamCtrl.addParam(CC_SPEED_NUM, [this](int ccVal) {
    parameterManager.setParameterValue(ParameterIDs::speed,
                                       juce::jlimit(0, 127, ccVal) / 127.0f);
  });
  mParamCtrl.addParam(CC_PHASE_NUM, [this](int ccVal) {
    parameterManager.setParameterValue(
        ParameterIDs::phase, juce::jlimit(0, 127, ccVal) / 127.0f - 0.5f);
  });
}

void AudioEngine::prepareToPlay(double sampleRate,
                                int samplesPerBlock,
                                int numChannels) {
  currentSampleRate = sampleRate;
  currentBlockSize = samplesPerBlock;
  mNumChannels = numChannels;

  // Setup LED mapping

  juce::SpinLock::ScopedTryLockType lock(mColorLock);
  mLowFilter.prepareToPlay(sampleRate, samplesPerBlock, mNumChannels);
  mLowTrigger.reset();
}

void AudioEngine::releaseResources() {}

void AudioEngine::processBlock(juce::AudioBuffer<float>& buffer,
                               juce::MidiBuffer& midiMessages,
                               double blockDurationSeconds) {
  mLowFilter.processBlock(buffer);
  // (TODO)mLowFreqLevel= mLowTrigger.process(mLowFilter.getRms(),
  // buffer.getNumSamples());
  mLowFreqLevel = mLowFilter.getRms() *
                  50;  // TOOD : normailse and make a param for this "50"
  parameterManager.setLowRms(mLowFreqLevel);

  // Clear audio buffer
  buffer.clear();

  // process MIDI messages
  processMidiMessages(midiMessages, blockDurationSeconds);
}

void AudioEngine::processBlock(juce::AudioBuffer<double>& buffer,
                               juce::MidiBuffer& midiMessages,
                               double blockDurationSeconds) {
  // Clear audio buffer (no audio output)
  buffer.clear();

  // process MIDI messages
  processMidiMessages(midiMessages, blockDurationSeconds);
}

/**********************************************************************************/
void AudioEngine::learn(const juce::MidiMessage& message) {
  mLearning = false;
  mMessage = message.getDescription();
}

/**********************************************************************************/
void AudioEngine::setGlobalWhiteLevel(double level) {
  mWhiteLevel = toFloat01(level);
}

/**********************************************************************************/
void AudioEngine::setGlobalPhaseLevel(double level) {
  mPhaseLevel = toFloat01(level);
}

/**********************************************************************************/
juce::Colour AudioEngine::getLedColor(LedId ledId) const {
  static const juce::Colour unknown(0);
  juce::SpinLock::ScopedTryLockType lock(mColorLock);

  if (lock.isLocked() && ledId < NB_MAX_LEDS) {
    const LedContext* led{mLeds.getLed(ledId)};
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
    const LedContext* led{mLeds.getLed(ledId)};
    const LedCtrlLine& m(led->ctrl);

    const LineValue& w(mOutMidiCtxt.mOutputContext[m.mw].lastSent);
    return normalizeRgbw(w, w, w);
  }
  return unknown;
}

/**********************************************************************************/
void AudioEngine::receiveMidiMsg(const juce::MidiMessage& msg) {
  const juce::SpinLock::ScopedLockType lock(mSpinLock);
  mPendingUiMidiMsg.push_back(MidiEvent(msg, 0));
}

/**********************************************************************************/
void AudioEngine::processMidiMessages(juce::MidiBuffer& midiMessages,
                                      double blockDurationSeconds) {
  // What will be sent to DMX!
  juce::MidiBuffer newEvents;

  // UI-injected events (from mPendingUiMidiMsg)
  juce::MidiBuffer injected;

  if (mSpinLock.tryEnter()) {
    for (auto& e : mPendingUiMidiMsg) {
      injected.addEvent(e.msg, 0);
    }

    mPendingUiMidiMsg.clear();
    mSpinLock.exit();
  }

  midiMessages.addEvents(injected, 0, currentBlockSize, 0);

  // Process all MIDI messages
  for (const auto metadata : midiMessages) {
    juce::MidiMessage message = metadata.getMessage();
    if (mLearning) {
      learn(message);
    }

    // Search for matching program
    {
      int param;
      BaseProgram* prg = mProgramManager.getByTrigger(message, param);
      if (prg) {
        DBG("MIDI " << prg->triggerName() << " => " << prg->name
                    << ", param=" << param);

        const CCValue cc(param > MAX_CC_VALUE ? MAX_CC_VALUE
                                              : static_cast<CCValue>(param));

        if (prg->isFx()) {
          if (cc == 0) {
            // Stop Effect
            mProgramManager.popFx(prg);
          } else {
            mProgramManager.pushFx(prg, cc);
          }
        } else {
          mProgramManager.set(prg, cc);
        }
      } else {
        // Search for matching Controls
        if (message.isController()) {
          // Control Change Mesage
          int ccNum = message.getControllerNumber();
          int ccVal = message.getControllerValue();
          mParamCtrl.applyCc(ccNum, ccVal);
          DBG("Cc " << juce::String(ccNum) << " applied");
        } else {
          DBG("MIDI " << message.getDescription() << " => No Effect");
        }
      }
    }

    // TODO :clean and report this code...
    continue;
#if 0
    if (message.isNoteOn()) {
      // Note On Message
      auto noteNumber = message.getNoteNumber();
      auto velocity = message.getVelocity();

      auto it(noteColours.find(noteNumber));
      if (it != noteColours.end()) {
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

      } else {
        DBG("Note ON: " << noteNumber << " Velocity: " << velocity);
      }
    } else if (message.isNoteOff()) {
      // Message Note Off
      auto noteNumber = message.getNoteNumber();
      (void) noteNumber;

      DBG("Note OFF: " << noteNumber);

      if (noteNumber == 35) {
        mProgramManager.popFx(&progSimpleStroboscope);
        continue;
      }
      if (noteNumber == 34) {
        mProgramManager.popFx(&progSimpleWave);
        continue;
      }
      if (noteNumber == 33) {
        mProgramManager.popFx(&progRandomSparkle);
        continue;
      }
    } else if (message.isController()) {
      // Control Change Mesage
      auto controllerNumber = message.getControllerNumber();
      auto controllerValue = message.getControllerValue();
      (void) controllerNumber;
      (void) controllerValue;

      DBG("CC: " << controllerNumber << " Value: " << controllerValue);
    } else if (message.isProgramChange()) {
      // Program change message
      const int prg = message.getProgramChangeNumber();
      DBG("PC: " << prg);
    } else if (message.isPitchWheel()) {
      // Pitch message
      auto pitchWheelValue = message.getPitchWheelValue();
      (void) pitchWheelValue;

      DBG("Pitch Wheel: " << pitchWheelValue);
    }
#endif
  }

  mProgramManager.blockUpdate();

  {
    /* Apply controls via MIDI to DMX
     * It is mandatory to anticipate all events within current frame buffer
     */
    mCurrentTimeS += blockDurationSeconds;

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
    /* if (lineId == 9) {
       DBG("Sent CH= " << static_cast<int>(line.channel + 1) << ", lineId="
                       << std::to_string(lineId) << ", val=" << value);
     }*/
  }
}

/**********************************************************************************/
void AudioEngine::updateLeds(void) {
  mProgramManager.updateLeds(mLeds.getAll());
}

/**********************************************************************************/
juce::MidiMessage AudioEngine::programToMidi(const BaseProgram* prg) const {
  const auto it = mProgramManager.mProgramToMidi.find(prg);
  if (it == mProgramManager.mProgramToMidi.end())
    return juce::MidiMessage();
  return it->second;
}
/**********************************************************************************/
BaseProgram* AudioEngine::noteToProgram(int note) const {
  const auto it = mProgramManager.mNoteToProgram.find(note);
  if (it == mProgramManager.mNoteToProgram.end())
    return nullptr;
  return it->second;
}

/**********************************************************************************/
AudioEngine::ProgramManager::ProgramManager(AudioEngine& engine)
    : mEngine(engine),
      mainPrograms{&defaultProgram, &progBreathing, &progWarmCoolCycle,
                   &progRandomFill, &sZoneFlash,    &sOppColors},
      fxPrograms{&progSimpleStroboscope, &progSimpleWave,
                 &progSingleFlashFxL,    &progSingleFlashFxC,
                 &progSingleFlashFxR,    &progSingleFlashFxTop,
                 &progSingleFlashFxBot,  &progRandomSparkle} {
  {
    uint8_t pc = 20;
    for (BaseProgram* pPrg : mainPrograms) {
      mProgramToMidi[pPrg] = juce::MidiMessage::programChange(1, pc);
      // mNoteToProgram[pc] = pPrg; // TODO remove
      pPrg->setTrigger(new ProgramTriggerPC(pc));
      pc++;
      mTriggers[pPrg->triggerName()] = pPrg;
    }
  }

  {
    int note = 20;
    for (BaseProgram* pPrg : fxPrograms) {
      static const uint8_t param(0);
      mProgramToMidi[pPrg] = juce::MidiMessage::noteOn(1, note, param);
      // mNoteToProgram[note] = pPrg;
      pPrg->setTrigger(new ProgramTriggerNote(note));
      note++;
      mTriggers[pPrg->triggerName()] = pPrg;
    }
  }
}

/**********************************************************************************/
BaseProgram* AudioEngine::ProgramManager::getByTrigger(
    const juce::MidiMessage& message,
    int& param) {
  juce::String s;
  param = MAX_CC_VALUE;

  if (message.isNoteOn()) {
    s = ProgramTriggerNote(message.getNoteNumber()).name();
    param = message.getVelocity();
  } else if (message.isNoteOff()) {
    s = ProgramTriggerNote(message.getNoteNumber()).name();
    param = 0;
  } else if (message.isProgramChange()) {
    s = ProgramTriggerPC(message.getProgramChangeNumber()).name();
  } else if (message.isController()) {
    s = ProgramTriggerCC(message.getControllerNumber()).name();
    param = message.getControllerValue();
  } else {
    s = "Unsupported MIDI msg";
  }
  auto it = mTriggers.find(s);

  return (it != mTriggers.end()) ? it->second : nullptr;
}

/**********************************************************************************/
void AudioEngine::ProgramManager::set(BaseProgram* program, CCValue velocity) {
  mMainProgram = program;
  if (mMainProgram) {
    mMainProgram->reset(velocity);
  }
}

/**********************************************************************************/
void AudioEngine::ProgramManager::pushFx(BaseProgram* program,
                                         CCValue velocity,
                                         juce::uint32 duration) {
  if (!program)
    return;

  program->reset(velocity);

  const juce::SpinLock::ScopedLockType lock(mEngine.mSpinLock);
  if (duration > 0) {
    auto endMs = juce::Time::getMillisecondCounter() + duration;
    mOverlayProgramsToAdd[program] = endMs;
  } else {
    mOverlayProgramsToAdd[program] = 0;
  }
}

/**********************************************************************************/
void AudioEngine::ProgramManager::popFx(BaseProgram* program) {
  const juce::SpinLock::ScopedLockType lock(mEngine.mSpinLock);
  mOverlayProgramsToDel.push_back(program);
}

/**********************************************************************************/
void AudioEngine::ProgramManager::updateLeds(const LedVectId& m) {
  mLedsVectUpdate.reset(new LedVectId(m));
}

/**********************************************************************************/
void AudioEngine::ProgramManager::blockUpdate() {
  if (mLedsVectUpdate != nullptr) {
    mLedsVect.clear();
    for (auto& it : *mLedsVectUpdate) {
      mLedsVect.emplace_back(it.context);
    }
  }
  mLedsVectUpdate.reset(nullptr);
}

/**********************************************************************************/
void AudioEngine::ProgramManager::operator()(juce::MidiBuffer& newEvents) {
  if (mMainProgram == nullptr) {
    mMainProgram = &defaultProgram;
  }

  BaseProgram::Events events;
  events.reserve(256);

  // Run main program
  mMainProgram->execute(mLedsVect, mEngine.parameterManager, events);

  // Update programs

  if (mEngine.mSpinLock.tryEnter()) {
    for (BaseProgram* p : mOverlayProgramsToDel) {
      const FxPrograms::iterator it = mOverlayPrograms.find(p);
      if (it == mOverlayPrograms.end())
        continue;

      mOverlayPrograms.erase(it);
    }
    mOverlayProgramsToDel.clear();

    for (auto itPrg : mOverlayProgramsToAdd) {
      mOverlayPrograms[itPrg.first] = itPrg.second;
    }
    mOverlayProgramsToAdd.clear();
    mEngine.mSpinLock.exit();
  }

  // Run all active Fx
  for (auto it = mOverlayPrograms.begin(); it != mOverlayPrograms.end();) {
    BaseProgram& prg = *it->first;
    juce::uint32 timeout = it->second;

    if ((timeout > 0 && timeout <= juce::Time::getMillisecondCounter()) ||
        prg.done()) {
      DBG("Stopping program: " << prg.name);
      it = mOverlayPrograms.erase(it);
    } else {
      prg.execute(mLedsVect, mEngine.parameterManager, events);
      ++it;
    }
  }
  OutputMidiContext& midiCtx(mEngine.mOutMidiCtxt);

  for (const BaseProgram::Event& evt : events) {
    midiCtx.insertEvent(newEvents, evt.lineIdx, evt.value);
  }
}
