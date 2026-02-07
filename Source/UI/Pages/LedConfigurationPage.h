// =============================================================================
// UI/Pages/LedConfigurationPage.h
// =============================================================================
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

#include "LumiMIDIProcessor.h"
#include "UI/Components/CcSender.h"
#include "UI/Components/LedLineEditor.h"
#include "UI/Components/NumericTextEditor.h"
#include "UI/Components/WorldView.h"
#include "UI/Pages/PageBase.h"


class LedConfigurationPage : public PageBase {
 public:
  LedConfigurationPage(LumiMIDIProcessor& processor,
                       juce::AudioProcessorValueTreeState& apvts,
                       UI_WorldView& worldView);
  ~LedConfigurationPage() override = default;

  // Component overrides
  void paint(juce::Graphics& g) override;
  void resized() override;
  void mouseDown(const juce::MouseEvent& event) override;
  void mouseDrag(const juce::MouseEvent& event) override;
  void mouseMove(const juce::MouseEvent& event) override;
  void mouseUp(const juce::MouseEvent& event) override;

  // PageBase overrides
  void activate() override;
  void deactivate() override;

  void onMidiMappingChanged(bool manual = true);

 private:
  void setupComponents();
  void setupLayout();

  void refreshBtns();

  // LED management
  void handleWorldViewClick(const juce::MouseEvent& event);
  void selectLed(LedId ledId);
  void updateSelectedLedInfo();
  void addNewLed();
  void removeLed();
  void moveLed();
  void duplicateLed();

  LedContext* getEditingLed();

  // Component callbacks
  void onLedNameChanged();
  void onLedLengthChanged();
  void onLedTypeChanged();

  // Action callbacks
  void handleApplyButtonClicked();
  void handleCancelButtonClicked();

 private:
  LumiMIDIProcessor& mProcessor;
  juce::AudioProcessorValueTreeState& mApvts;
  UI_WorldView& mWorldView;

  // === LEFT SECTION: WorldView and tools ===
  juce::GroupComponent mWorldViewGroup;

  // Creation/editing tools
  juce::TextButton mBtnAddLed;
  juce::TextButton mBtnRemoveLed;
  juce::TextButton mBtnMoveLed;

  // === RIGHT SECTION: Selected LED properties ===
  juce::GroupComponent mPropertiesGroup;

  // General information
  juce::Label mLedNameLabel;
  juce::TextEditor mLedNameEditor;

  juce::Label mLedLengthLabel;
  juce::Slider mLedLengthSlider;
  juce::Label mLedLengthValue;

  juce::Label mLedTypeLabel;
  juce::ComboBox mLedTypeCombo;

  // Position (read-only for now)
  juce::Label mPositionLabel;
  juce::Label mPositionValue;

  // === MIDI MAPPING SECTION ===
  juce::GroupComponent mMidiMappingGroup;

  // R, G, B components (and W if RGBW)
  LedLineEditor mRedLine;
  LedLineEditor mGreenLine;
  LedLineEditor mBlueLine;
  LedLineEditor mWhiteLine;

  // === BOTTOM SECTION: Actions ===
  juce::GroupComponent mActionsGroup;

  juce::TextButton mBtnSaveConfig;
  juce::TextButton mBtnLoadConfig;
  juce::TextButton mBtnApply;
  juce::TextButton mBtnCancel;

  // Internal state
  std::unique_ptr<LedId> mSelectedLed{nullptr};  // Currently selected LED
  bool mIsEditingLed = false;                    // Edit mode active

  enum class EditMode {
    None,
    EditingLed,   // Adding a new LED
    RemovingLed,  // ReMoving a LED
    AddingLed,    // Adding a new LED
    MovingLed,    // Moving an LED
    ResizingLed   // Resizing an LED
  };
  EditMode mCurrentEditMode = EditMode::None;
  std::unique_ptr<LedContext> mAddingLedCtxt;

  struct MoveCtxt {
    MoveCtxt(LedContext* ctxtP, const Point& pOffset)
        : ctxt(ctxtP), dragOffset(pOffset), pPosInit(ctxt->pos.topLeft) {}
    LedContext* ctxt{nullptr};
    Point dragOffset;
    Point pPosInit;
  };
  std::unique_ptr<MoveCtxt> mMovingLedCtxt;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LedConfigurationPage)
};