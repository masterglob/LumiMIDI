// =============================================================================
// UI/Pages/LedConfigurationPage.h
// =============================================================================
#pragma once
#include "UI/Pages/PageBase.h"
#include "LumiMIDIProcessor.h"
#include "UI/Components/WorldView.h"
#include "UI/Components/CcSender.h"
#include "UI/Components/NumericTextEditor.h"
#include <juce_audio_processors/juce_audio_processors.h>

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

private:
    void setupComponents();
    void setupLayout();

    // LED management
    void handleWorldViewClick(const juce::MouseEvent& event);
    void selectLed(const LedContext* pLedCtxt);
    void updateSelectedLedInfo();
    void addNewLed();
    void removeLed();
    void duplicateLed();

    // Component callbacks
    void onLedNameChanged();
    void onLedLengthChanged();
    void onLedTypeChanged();
    void onMidiMappingChanged();

private:
    LumiMIDIProcessor& mProcessor;
    juce::AudioProcessorValueTreeState& mApvts;
    UI_WorldView& mWorldView;

    // === LEFT SECTION: WorldView and tools ===
    juce::GroupComponent mWorldViewGroup;

    // Creation/editing tools
    juce::TextButton mBtnAddLed;
    juce::TextButton mBtnRemoveLed;
    juce::TextButton mBtnDuplicateLed;
    juce::ToggleButton mToggleGridSnap;
    juce::Label mGridSnapLabel;

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
    juce::Label mRedLabel;
    juce::ComboBox mRedMidiTypeCombo;    // CC / Note On
    juce::Label mRedMidiPrefix;          // "CC#" or "Note#"
    NumericTextEditor mRedMidiValueEditor;// Numeric input 0-127 with wheel support

    juce::Label mGreenLabel;
    juce::ComboBox mGreenMidiTypeCombo;
    juce::Label mGreenMidiPrefix;
    NumericTextEditor mGreenMidiValueEditor;

    juce::Label mBlueLabel;
    juce::ComboBox mBlueMidiTypeCombo;
    juce::Label mBlueMidiPrefix;
    NumericTextEditor mBlueMidiValueEditor;

    juce::Label mWhiteLabel;             // Visible only if RGBW
    juce::ComboBox mWhiteMidiTypeCombo;
    juce::Label mWhiteMidiPrefix;
    NumericTextEditor mWhiteMidiValueEditor;

    // === BOTTOM SECTION: Actions and test ===
    juce::GroupComponent mActionsGroup;

    juce::TextButton mBtnSaveConfig;
    juce::TextButton mBtnLoadConfig;
    juce::TextButton mBtnTestLed;        // Test selected LED
    juce::TextButton mBtnTestAll;        // Test all LEDs

    // Internal state
    const LedContext* mSelectedLed;      // Currently selected LED
    bool mIsEditingLed = false;          // Edit mode active

    enum class EditMode {
        None,
        AddingLed,          // Adding a new LED
        MovingLed,          // Moving an LED
        ResizingLed         // Resizing an LED
    };
    EditMode mCurrentEditMode = EditMode::None;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LedConfigurationPage)
};