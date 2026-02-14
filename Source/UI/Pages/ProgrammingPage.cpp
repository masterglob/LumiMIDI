
// =============================================================================
// ProgrammingPage.cpp
// =============================================================================
#include "UI/Pages/ProgrammingPage.h"

#include <juce_graphics/juce_graphics.h>

namespace {
// === Mod�les de listes ===
}  // namespace

ProgramList::ProgramList(const AudioEngine::ProgramsVect& itemsRef) : items(itemsRef) {
  int raw{0};
  for (const BaseProgram* prg : itemsRef) {
    mProgramToRaw[prg] = raw;
    raw++;
  }
}

bool ProgramList::selectProgram(const BaseProgram* prg) {
  auto it(mProgramToRaw.find(prg));
  if (it != mProgramToRaw.end()) {
    mList.selectRow(it->second);
    return true;
  }
  return false;
}

void ProgramList::setupComponents(juce::Component& comp) {
  mList.setRowHeight(22);
  mList.setModel(this);
  comp.addAndMakeVisible(mList);
}

void ProgramList::resized(const juce::Rectangle<int>& area) {
  mList.setBounds(area.reduced(2));
}

void ProgramList::paintListBoxItem(int rowNumber,
                                   juce::Graphics& g,
                                   int width,
                                   int height,
                                   bool rowIsSelected) {
  if (rowIsSelected) {
    g.fillAll(juce::Colours::lightblue);
    g.setColour(juce::Colours::black);
  } else {
    g.setColour(juce::Colours::white);
  }

  if (rowNumber >= 0 && rowNumber < static_cast<int>(items.size())) {
    const BaseProgram* prg{items[static_cast<size_t>(rowNumber)]};
    if (!prg)
      g.drawText("...", 2, 0, width - 4, height, juce::Justification::centredLeft);
    else
      g.drawText(
          prg->triggerName() + "  " + prg->name, 2, 0, width - 4, height, juce::Justification::centredLeft);
  }
}

void ProgramList::listBoxItemClicked(int row, const juce::MouseEvent&) {
  if (onItemClicked && row >= 0 && row < static_cast<int>(items.size())) {
    onItemClicked(items[static_cast<size_t>(row)]);
  }
}

ProgrammingPage::ProgrammingPage(LumiMIDIProcessor& processor,
                                 juce::AudioProcessorValueTreeState& apvts,
                                 juce::MidiKeyboardState& keyboardState,
                                 UI_WorldView& worldView)
    : mProcessor(processor),
      mApvts(apvts),
      mKeyboardState(keyboardState),
      mWorldView(worldView),
      mProgramName(),
      mWhiteGlobalKnob("White",
                       apvts,
                       ParameterIDs::mainW,
                       [this](double val) { mProcessor.getAudioEngine().setGlobalWhiteLevel(val); }),
      mHueGlobalKnob("Hue",
                     apvts,
                     ParameterIDs::mainHue,
                     [this](double val) { mProcessor.getAudioEngine().setMainHueLevel(val); }),
      mFx1HueGlobalKnob("Fx1-Hue",
                        apvts,
                        ParameterIDs::fx1Hue,
                        [this](double val) { mProcessor.getAudioEngine().setFx1HueLevel(val); }),
      mFx2HueGlobalKnob("Fx2-Hue",
                        apvts,
                        ParameterIDs::fx2Hue,
                        [this](double val) { mProcessor.getAudioEngine().setFx2HueLevel(val); }),
      mSpeedKnob("Speed", apvts, ParameterIDs::speed, nullptr),
      mPhaseKnob("Phase",
                 apvts,
                 ParameterIDs::phase,
                 [this](double val) { mProcessor.getAudioEngine().setGlobalPhaseLevel(val); }),
      mMidiKeyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
      mMainProgramList(mProcessor.getAudioEngine().getMainPrograms()),
      mFxList(mProcessor.getAudioEngine().getFxPrograms()) {
  setupComponents();
  setupLayout();

  // �couter les �v�nements MIDI
  mKeyboardState.addListener(this);
}

ProgrammingPage::~ProgrammingPage() {
  mKeyboardState.removeListener(this);
}

void ProgrammingPage::paint(juce::Graphics& g) {
  g.fillAll(juce::Colours::darkblue.darker());

  g.setColour(juce::Colours::white);
  g.setFont(18.0f);
  g.drawText("Programming & Automation", getLocalBounds().removeFromTop(30), juce::Justification::centred);
}

void ProgrammingPage::resized() {
  auto bounds = getLocalBounds();
  bounds.removeFromTop(35);  // Title

  mProgramName.setBounds(bounds.removeFromTop(30).reduced(5));

  // Bottom keyboard
  auto keyboardHeight = 80;
  mMidiKeyboard.setBounds(bounds.removeFromBottom(keyboardHeight));
  bounds.removeFromBottom(10);  // Spacing

  // Programs (Left)
  auto prgArea = bounds.removeFromLeft(320);
  {
    auto topArea = prgArea.removeFromTop(prgArea.getHeight() / 2);
    auto bottomArea = prgArea;

    mMainProgramLabel.setBounds(topArea.removeFromTop(25).reduced(2));
    mMainProgramList.resized(topArea.reduced(2));

    mFxLabel.setBounds(bottomArea.removeFromTop(25).reduced(2));
    mFxList.resized(bottomArea.reduced(2));
  }

  // Controls (top)
  {
    auto controlArea = bounds.removeFromTop(120);
    auto btnW = controlArea.getWidth() / 4;

    mWhiteGlobalKnob.setBounds(controlArea.removeFromLeft(btnW).reduced(5));
    mHueGlobalKnob.setBounds(controlArea.removeFromLeft(btnW).reduced(5));
    mSpeedKnob.setBounds(controlArea.removeFromLeft(btnW).reduced(5));
    mPhaseKnob.setBounds(controlArea.removeFromLeft(btnW).reduced(5));
  }
  // Worldview (remaining in center)
  auto worldViewArea = bounds;

  if (mIsActive) mWorldView.setBounds(worldViewArea.reduced(5));
}

void ProgrammingPage::activate() {
  mIsActive = true;
  setVisible(true);
  // CORRECTION: Retirer le WorldView de son parent actuel avant de l'ajouter
  auto* currentParent = mWorldView.getParentComponent();
  if (currentParent && currentParent != this) {
    currentParent->removeChildComponent(&mWorldView);
    currentParent->repaint();
  }

  // Ajouter le WorldView � cette page
  addAndMakeVisible(mWorldView);
  mWorldView.setViewMode(UI_WorldView::ViewMode::Compact);
  mWorldView.setShowLedNames(false);
  mWorldView.setRefreshRate(15);

  mWhiteGlobalKnob.setVisible(true);
  mHueGlobalKnob.setVisible(true);
  mSpeedKnob.setVisible(true);
  mPhaseKnob.setVisible(true);

  repaint();
}

void ProgrammingPage::deactivate() {
  mIsActive = false;
  setVisible(false);
}

void ProgrammingPage::handleNoteOn(juce::MidiKeyboardState* source,
                                   int midiChannel,
                                   int midiNoteNumber,
                                   float velocity) {
  (void) source;
  // Cr�er le message MIDI
  auto message = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);

  // Envoyer au processeur via une m�thode s�curis�e
  mProcessor.addMidiEvent(message);
}

void ProgrammingPage::handleNoteOff(juce::MidiKeyboardState* source,
                                    int midiChannel,
                                    int midiNoteNumber,
                                    float velocity) {
  (void) source;
  auto message = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
  mProcessor.addMidiEvent(message);
}

void ProgrammingPage::onSend_CC_Clicked(unsigned int cc) {
  mProcessor.sendDirectMidiEvent(juce::MidiMessage::controllerEvent(1, cc, 63));
}

void ProgrammingPage::setupComponents() {
  mWorldView.setViewMode(UI_WorldView::ViewMode::Compact);
  mWorldView.setShowLedNames(false);
  mWorldView.setRefreshRate(15);

  mProgramName.setText("Current program :", juce::dontSendNotification);
  mProgramName.setJustificationType(juce::Justification::centred);

  {
    juce::Font f(juce::FontOptions().withHeight(20.0f));
    f.setTypefaceStyle("Bold");
    mProgramName.setFont(f);
  }

  addAndMakeVisible(mProgramName);

  addAndMakeVisible(mWorldView);
  addAndMakeVisible(mMidiKeyboard);

  addAndMakeVisible(mWhiteGlobalKnob);
  addAndMakeVisible(mHueGlobalKnob);
  addAndMakeVisible(mSpeedKnob);
  addAndMakeVisible(mPhaseKnob);

  // Main Program
  mMainProgramLabel.setText("Main Program", juce::dontSendNotification);
  mMainProgramLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(mMainProgramLabel);

  mMainProgramList.setupComponents(*this);

  // Fx
  mFxLabel.setText("Effect", juce::dontSendNotification);
  mFxLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(mFxLabel);

  mFxList.setupComponents(*this);

  // === Callbacks ===
  mMainProgramList.onItemClicked = [this](const BaseProgram* prg) {
    if (prg) {
      juce::Logger::outputDebugString("Program clicked: " + prg->name);
      AudioEngine& audio(mProcessor.getAudioEngine());
      auto trg{prg->trigger()};
      if (trg) {
        audio.receiveMidiMsg(juce::MidiMessage::programChange(1, trg->pId));
      }
    }
  };

  mFxList.onItemClicked = [this](const BaseProgram* prg) {
    if (prg) {
      juce::Logger::outputDebugString("Fx clicked: " + prg->name);
      AudioEngine& audio(mProcessor.getAudioEngine());
      auto trg{prg->trigger()};
      if (trg) {
        audio.receiveMidiMsg(juce::MidiMessage::noteOn(1, trg->pId, MAX_CC_VALUE));
      }
    }
  };
}

void ProgrammingPage::setupLayout() {
}

void ProgrammingPage::setProgram(const BaseProgram* pPrg) {
  if (pPrg && mCurrPrg != pPrg) {
    if (mMainProgramList.selectProgram(pPrg)) {
      mCurrPrg = pPrg;
      mProgramName.setText("Current program :" + mCurrPrg->name, juce::dontSendNotification);
    }
  }
}
