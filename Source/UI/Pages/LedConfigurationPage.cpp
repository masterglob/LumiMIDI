// =============================================================================
// UI/Pages/LedConfigurationPage.cpp
// =============================================================================
#include "UI/Pages/LedConfigurationPage.h"

LedConfigurationPage::LedConfigurationPage(LumiMIDIProcessor& processor,
    juce::AudioProcessorValueTreeState& apvts,
    UI_WorldView& worldView)
    : mProcessor(processor)
    , mApvts(apvts)
    , mWorldView(worldView)
    , mBtnAddLed("Add LED")
    , mBtnRemoveLed("Remove")
    , mBtnDuplicateLed("Duplicate")
    , mToggleGridSnap("Grid Snap")
    , mGridSnapLabel("Grid Snap", "Snap to grid")
    , mLedNameLabel("Name", "LED Name")
    , mLedLengthLabel("Length", "LED Length")
    , mLedLengthValue("100", "Length value")
    , mLedTypeLabel("Type", "LED Type")
    , mPositionLabel("Position", "LED Position")
    , mPositionValue("(0, 0) - (100, 0)", "Position coordinates")
    , mRedLabel("Red", "Red")
    , mGreenLabel("Green", "Green")
    , mBlueLabel("Blue", "Blue")
    , mWhiteLabel("White", "White")
    , mBtnSaveConfig("Save Config")
    , mBtnLoadConfig("Load Config")
    , mBtnTestLed("Test LED")
    , mBtnTestAll("Test All")
{
    setupComponents();
    setupLayout();
}

void LedConfigurationPage::paint(juce::Graphics& g) {
    // Page background
    g.fillAll(juce::Colours::darkgrey.darker());

    // Page title
    auto titleArea = getLocalBounds().removeFromTop(30);
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("LED Configuration", titleArea, juce::Justification::centred);
}

void LedConfigurationPage::resized() {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(35); // Space for title

    // Main division: 65% left (WorldView), 35% right (Properties)
    auto leftArea = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.65f));
    auto rightArea = bounds.reduced(5, 0); // Left margin for right section

    // === LEFT SECTION: WorldView and tools ===
    auto worldViewArea = leftArea.reduced(5);

    // WorldView group with title
    mWorldViewGroup.setBounds(worldViewArea);

    // Inner group area (without border and title)
    auto worldViewContent = worldViewArea.reduced(10, 25); // Margin for border and title

    // Tools at top (action buttons)
    auto toolsHeight = 35;
    auto toolsArea = worldViewContent.removeFromTop(toolsHeight);

    auto buttonWidth = 80;
    auto spacing = 5;
    mBtnAddLed.setBounds(toolsArea.removeFromLeft(buttonWidth));
    toolsArea.removeFromLeft(spacing);
    mBtnRemoveLed.setBounds(toolsArea.removeFromLeft(buttonWidth));
    toolsArea.removeFromLeft(spacing);
    mBtnDuplicateLed.setBounds(toolsArea.removeFromLeft(buttonWidth));

    // Grid snap on the right
    toolsArea.removeFromLeft(20); // Spacing
    mToggleGridSnap.setBounds(toolsArea.removeFromLeft(20)); // Checkbox
    mGridSnapLabel.setBounds(toolsArea.removeFromLeft(80));  // Label

    // WorldView takes the rest
    worldViewContent.removeFromTop(10); // Spacing
    mWorldView.setBounds(worldViewContent);

    // === RIGHT SECTION: Properties ===
    auto propertiesArea = rightArea;

    // Properties group (1/3 of top)
    auto propsHeight = propertiesArea.getHeight() / 3;
    auto propsArea = propertiesArea.removeFromTop(propsHeight);
    mPropertiesGroup.setBounds(propsArea);

    auto propsContent = propsArea.reduced(10, 25);
    auto rowHeight = 25;
    auto labelWidth = 60;
    auto spacing2 = 5;

    // LED name
    auto nameRow = propsContent.removeFromTop(rowHeight);
    mLedNameLabel.setBounds(nameRow.removeFromLeft(labelWidth));
    mLedNameEditor.setBounds(nameRow.reduced(spacing2, 0));

    propsContent.removeFromTop(spacing2);

    // LED length
    auto lengthRow = propsContent.removeFromTop(rowHeight);
    mLedLengthLabel.setBounds(lengthRow.removeFromLeft(labelWidth));
    auto lengthControls = lengthRow.reduced(spacing2, 0);
    mLedLengthSlider.setBounds(lengthControls.removeFromLeft(lengthControls.getWidth() - 50));
    mLedLengthValue.setBounds(lengthControls);

    propsContent.removeFromTop(spacing2);

    // LED type
    auto typeRow = propsContent.removeFromTop(rowHeight);
    mLedTypeLabel.setBounds(typeRow.removeFromLeft(labelWidth));
    mLedTypeCombo.setBounds(typeRow.reduced(spacing2, 0));

    propsContent.removeFromTop(spacing2);

    // Position (read-only)
    auto posRow = propsContent.removeFromTop(rowHeight);
    mPositionLabel.setBounds(posRow.removeFromLeft(labelWidth));
    mPositionValue.setBounds(posRow.reduced(spacing2, 0));

    // MIDI Mapping group (1/2 of middle)
    propertiesArea.removeFromTop(10); // Spacing
    auto midiHeight = propertiesArea.getHeight() * 2 / 3;
    auto midiArea = propertiesArea.removeFromTop(midiHeight);
    mMidiMappingGroup.setBounds(midiArea);

    auto midiContent = midiArea.reduced(10, 25);
    auto midiRowHeight = 30;
    auto colorLabelWidth = 50;
    auto comboWidth = 70;
    auto prefixWidth = 40;      // "CC#" or "Note#"
    auto editorWidth = 50;      // TextEditor for value

    // Red component
    auto redRow = midiContent.removeFromTop(midiRowHeight);
    mRedLabel.setBounds(redRow.removeFromLeft(colorLabelWidth));
    redRow.removeFromLeft(spacing2);
    mRedMidiTypeCombo.setBounds(redRow.removeFromLeft(comboWidth));
    redRow.removeFromLeft(spacing2);
    mRedMidiPrefix.setBounds(redRow.removeFromLeft(prefixWidth));
    mRedMidiValueEditor.setBounds(redRow.removeFromLeft(editorWidth));

    midiContent.removeFromTop(spacing2);

    // Green component
    auto greenRow = midiContent.removeFromTop(midiRowHeight);
    mGreenLabel.setBounds(greenRow.removeFromLeft(colorLabelWidth));
    greenRow.removeFromLeft(spacing2);
    mGreenMidiTypeCombo.setBounds(greenRow.removeFromLeft(comboWidth));
    greenRow.removeFromLeft(spacing2);
    mGreenMidiPrefix.setBounds(greenRow.removeFromLeft(prefixWidth));
    mGreenMidiValueEditor.setBounds(greenRow.removeFromLeft(editorWidth));

    midiContent.removeFromTop(spacing2);

    // Blue component
    auto blueRow = midiContent.removeFromTop(midiRowHeight);
    mBlueLabel.setBounds(blueRow.removeFromLeft(colorLabelWidth));
    blueRow.removeFromLeft(spacing2);
    mBlueMidiTypeCombo.setBounds(blueRow.removeFromLeft(comboWidth));
    blueRow.removeFromLeft(spacing2);
    mBlueMidiPrefix.setBounds(blueRow.removeFromLeft(prefixWidth));
    mBlueMidiValueEditor.setBounds(blueRow.removeFromLeft(editorWidth));

    midiContent.removeFromTop(spacing2);

    // White component (visible only if RGBW)
    auto whiteRow = midiContent.removeFromTop(midiRowHeight);
    mWhiteLabel.setBounds(whiteRow.removeFromLeft(colorLabelWidth));
    whiteRow.removeFromLeft(spacing2);
    mWhiteMidiTypeCombo.setBounds(whiteRow.removeFromLeft(comboWidth));
    whiteRow.removeFromLeft(spacing2);
    mWhiteMidiPrefix.setBounds(whiteRow.removeFromLeft(prefixWidth));
    mWhiteMidiValueEditor.setBounds(whiteRow.removeFromLeft(editorWidth));

    // Actions group (bottom)
    propertiesArea.removeFromTop(10); // Spacing
    mActionsGroup.setBounds(propertiesArea);

    auto actionsContent = propertiesArea.reduced(10, 25);
    auto actionButtonHeight = 30;
    auto actionButtonWidth = 80;

    // First row: Save/Load
    auto firstActionRow = actionsContent.removeFromTop(actionButtonHeight);
    mBtnSaveConfig.setBounds(firstActionRow.removeFromLeft(actionButtonWidth));
    firstActionRow.removeFromLeft(spacing2);
    mBtnLoadConfig.setBounds(firstActionRow.removeFromLeft(actionButtonWidth));

    actionsContent.removeFromTop(spacing2);

    // Second row: Test
    auto secondActionRow = actionsContent.removeFromTop(actionButtonHeight);
    mBtnTestLed.setBounds(secondActionRow.removeFromLeft(actionButtonWidth));
    secondActionRow.removeFromLeft(spacing2);
    mBtnTestAll.setBounds(secondActionRow.removeFromLeft(actionButtonWidth));
}

void LedConfigurationPage::mouseDown(const juce::MouseEvent& event) {
    // Check if click is in WorldView area
    if (mWorldView.getBounds().contains(event.getPosition())) {
        handleWorldViewClick(event);
    }
}

void LedConfigurationPage::mouseDrag(const juce::MouseEvent& event) {
    // Handle drag to move/resize LEDs
    // (to implement later)
}

void LedConfigurationPage::mouseMove(const juce::MouseEvent& event) {
    // Handle mouse move for hover effects, cursor changes, etc.
    // Check if mouse is over WorldView using the correct coordinate system
    if (mWorldView.getBounds().contains(event.getPosition())) {
        // TODO: Implement hover logic for LEDs
        // - Change cursor when over LED handles
        // - Show LED information on hover
        // - Highlight LED under cursor

        // Example: Change cursor based on edit mode
        switch (mCurrentEditMode) {
        case EditMode::None:
            setMouseCursor(juce::MouseCursor::NormalCursor);
            break;
        case EditMode::AddingLed:
            setMouseCursor(juce::MouseCursor::CrosshairCursor);
            break;
        case EditMode::MovingLed:
            setMouseCursor(juce::MouseCursor::DraggingHandCursor);
            break;
        case EditMode::ResizingLed:
            setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
            break;
        }

        // Debug: Print mouse position when over WorldView
        DBG("Mouse over WorldView at: " << event.getPosition().toString());
    }
    else {
        // Mouse outside WorldView
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
}

void LedConfigurationPage::mouseUp(const juce::MouseEvent& event) {
    // End interaction
    mCurrentEditMode = EditMode::None;
}

void LedConfigurationPage::activate() {
    mIsActive = true;
    setVisible(true);

    // Handle shared WorldView
    auto* currentParent = mWorldView.getParentComponent();
    if (currentParent && currentParent != this) {
        currentParent->removeChildComponent(&mWorldView);
        currentParent->repaint();
    }

    addAndMakeVisible(mWorldView);
    mWorldView.setShowLedNames(true);
    mWorldView.setRefreshRate(10);
    mWorldView.setViewMode(UI_WorldView::ViewMode::Full);

    // Allow parent to receive mouse events over WorldView
    mWorldView.setInterceptsMouseClicks(false, true);

    repaint();
}

void LedConfigurationPage::deactivate() {
    mIsActive = false;
    setVisible(false);
}

void LedConfigurationPage::setupComponents() {
    // === Groups ===
    addAndMakeVisible(mWorldViewGroup);
    mWorldViewGroup.setText("LED Layout");

    addAndMakeVisible(mPropertiesGroup);
    mPropertiesGroup.setText("LED Properties");

    addAndMakeVisible(mMidiMappingGroup);
    mMidiMappingGroup.setText("MIDI Mapping");

    addAndMakeVisible(mActionsGroup);
    mActionsGroup.setText("Actions");

    // === Tools ===
    addAndMakeVisible(mBtnAddLed);
    addAndMakeVisible(mBtnRemoveLed);
    addAndMakeVisible(mBtnDuplicateLed);
    addAndMakeVisible(mToggleGridSnap);
    addAndMakeVisible(mGridSnapLabel);

    // === Properties ===
    addAndMakeVisible(mLedNameLabel);
    addAndMakeVisible(mLedNameEditor);
    addAndMakeVisible(mLedLengthLabel);
    addAndMakeVisible(mLedLengthSlider);
    addAndMakeVisible(mLedLengthValue);
    addAndMakeVisible(mLedTypeLabel);
    addAndMakeVisible(mLedTypeCombo);
    addAndMakeVisible(mPositionLabel);
    addAndMakeVisible(mPositionValue);

    // === MIDI Mapping ===
    addAndMakeVisible(mRedLabel);
    addAndMakeVisible(mRedMidiTypeCombo);
    addAndMakeVisible(mRedMidiPrefix);
    addAndMakeVisible(mRedMidiValueEditor);

    addAndMakeVisible(mGreenLabel);
    addAndMakeVisible(mGreenMidiTypeCombo);
    addAndMakeVisible(mGreenMidiPrefix);
    addAndMakeVisible(mGreenMidiValueEditor);

    addAndMakeVisible(mBlueLabel);
    addAndMakeVisible(mBlueMidiTypeCombo);
    addAndMakeVisible(mBlueMidiPrefix);
    addAndMakeVisible(mBlueMidiValueEditor);

    addAndMakeVisible(mWhiteLabel);
    addAndMakeVisible(mWhiteMidiTypeCombo);
    addAndMakeVisible(mWhiteMidiPrefix);
    addAndMakeVisible(mWhiteMidiValueEditor);

    // === Actions ===
    addAndMakeVisible(mBtnSaveConfig);
    addAndMakeVisible(mBtnLoadConfig);
    addAndMakeVisible(mBtnTestLed);
    addAndMakeVisible(mBtnTestAll);

    // Component configuration
    mLedTypeCombo.addItem("RGB", 1);
    mLedTypeCombo.addItem("RGBW", 2);
    mLedTypeCombo.setSelectedId(1);

    // MIDI combo configuration
    for (auto* combo : { &mRedMidiTypeCombo, &mGreenMidiTypeCombo, &mBlueMidiTypeCombo, &mWhiteMidiTypeCombo }) {
        combo->addItem("None", 1);
        combo->addItem("CC", 2);
        combo->addItem("Note On", 3);
        combo->setSelectedId(1); // Default to None
    }

    // MIDI TextEditor configuration (0-127, numbers only)
    for (auto* editor : { &mRedMidiValueEditor, &mGreenMidiValueEditor, &mBlueMidiValueEditor, &mWhiteMidiValueEditor }) {
        editor->setRange(0, 127);           // Set valid range for MIDI values
        editor->setWheelIncrement(1);       // Default increment for mouse wheel
        editor->setArrowIncrement(1);       // Default increment for arrow keys
        editor->setShiftMultiplier(10);     // Shift+wheel/arrow = increment by 10
    }

    // Default values using setNumericValue
    mRedMidiValueEditor.setNumericValue(17, false);
    mGreenMidiValueEditor.setNumericValue(18, false);
    mBlueMidiValueEditor.setNumericValue(19, false);
    mWhiteMidiValueEditor.setNumericValue(20, false);

    // Default prefixes (hidden initially since None is selected)
    mRedMidiPrefix.setText("", juce::dontSendNotification);
    mGreenMidiPrefix.setText("", juce::dontSendNotification);
    mBlueMidiPrefix.setText("", juce::dontSendNotification);
    mWhiteMidiPrefix.setText("", juce::dontSendNotification);

    // Hide prefix and value editor initially (None is selected by default)
    for (auto* prefix : { &mRedMidiPrefix, &mGreenMidiPrefix, &mBlueMidiPrefix, &mWhiteMidiPrefix }) {
        prefix->setVisible(false);
    }
    for (auto* editor : { &mRedMidiValueEditor, &mGreenMidiValueEditor, &mBlueMidiValueEditor, &mWhiteMidiValueEditor }) {
        editor->setVisible(false);
    }

    mLedLengthSlider.setRange(10, 1000, 1);
    mLedLengthSlider.setValue(100);
    mLedLengthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mLedLengthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
}

void LedConfigurationPage::setupLayout() {
    // Component callbacks (to implement)
    mBtnAddLed.onClick = [this]() { addNewLed(); };
    mBtnRemoveLed.onClick = [this]() { removeLed(); };
    mBtnDuplicateLed.onClick = [this]() { duplicateLed(); };

    mLedNameEditor.onTextChange = [this]() { onLedNameChanged(); };
    mLedLengthSlider.onValueChange = [this]() { onLedLengthChanged(); };
    mLedTypeCombo.onChange = [this]() { onLedTypeChanged(); };

    // MIDI callbacks for all components
    struct MidiComponentSet {
        juce::ComboBox* typeCombo;
        juce::Label* prefixLabel;
        juce::TextEditor* valueEditor;
    };

    std::array<MidiComponentSet, 4> midiComponents = { {
        { &mRedMidiTypeCombo, &mRedMidiPrefix, &mRedMidiValueEditor },
        { &mGreenMidiTypeCombo, &mGreenMidiPrefix, &mGreenMidiValueEditor },
        { &mBlueMidiTypeCombo, &mBlueMidiPrefix, &mBlueMidiValueEditor },
        { &mWhiteMidiTypeCombo, &mWhiteMidiPrefix, &mWhiteMidiValueEditor }
    } };

    for (auto& component : midiComponents) {
        component.typeCombo->onChange = [this]() { onMidiMappingChanged(); };
        component.valueEditor->onTextChange = [this]() { onMidiMappingChanged(); };
    }
}

// Methods to implement (for now, just stubs)
void LedConfigurationPage::handleWorldViewClick(const juce::MouseEvent& event) {
    // TODO: Implement LED selection/creation
}

void LedConfigurationPage::selectLed(const juce::String& ledName) {
    mSelectedLedName = ledName;
    updateSelectedLedInfo();
}

void LedConfigurationPage::updateSelectedLedInfo() {
    // TODO: Update interface with selected LED info
}

void LedConfigurationPage::addNewLed() {
    // TODO: Add a new LED
}

void LedConfigurationPage::removeLed() {
    // TODO: Remove selected LED
}

void LedConfigurationPage::duplicateLed() {
    // TODO: Duplicate selected LED
}

void LedConfigurationPage::onLedNameChanged() {
    // TODO: Update LED name
}

void LedConfigurationPage::onLedLengthChanged() {
    // TODO: Update LED length
    mLedLengthValue.setText(juce::String(static_cast<int>(mLedLengthSlider.getValue())), juce::dontSendNotification);
}

void LedConfigurationPage::onLedTypeChanged() {
    // TODO: Update LED type and show/hide White component
    bool isRGBW = mLedTypeCombo.getSelectedId() == 2;
    mWhiteLabel.setVisible(isRGBW);
    mWhiteMidiTypeCombo.setVisible(isRGBW);

    // For White component, also check if MIDI type is None
    if (isRGBW) {
        bool whiteMidiEnabled = mWhiteMidiTypeCombo.getSelectedId() != 1; // Not None
        mWhiteMidiPrefix.setVisible(whiteMidiEnabled);
        mWhiteMidiValueEditor.setVisible(whiteMidiEnabled);
    }
    else {
        mWhiteMidiPrefix.setVisible(false);
        mWhiteMidiValueEditor.setVisible(false);
    }
}

void LedConfigurationPage::onMidiMappingChanged() {
    // Structure to iterate through all MIDI components
    struct MidiComponentSet {
        juce::ComboBox* typeCombo;
        juce::Label* prefixLabel;
        NumericTextEditor* valueEditor;
    };

    std::array<MidiComponentSet, 4> midiComponents = { {
        { &mRedMidiTypeCombo, &mRedMidiPrefix, &mRedMidiValueEditor },
        { &mGreenMidiTypeCombo, &mGreenMidiPrefix, &mGreenMidiValueEditor },
        { &mBlueMidiTypeCombo, &mBlueMidiPrefix, &mBlueMidiValueEditor },
        { &mWhiteMidiTypeCombo, &mWhiteMidiPrefix, &mWhiteMidiValueEditor }
    } };

    for (auto& component : midiComponents) {
        // Get selected type (1 = None, 2 = CC, 3 = Note On)
        int selectedType = component.typeCombo->getSelectedId();

        if (selectedType == 1) { // None selected
            // Hide prefix and value editor
            component.prefixLabel->setVisible(false);
            component.valueEditor->setVisible(false);
        }
        else {
            // Show prefix and value editor
            component.prefixLabel->setVisible(true);
            component.valueEditor->setVisible(true);

            // Set appropriate prefix
            juce::String typePrefix = (selectedType == 2) ? "CC#" : "Note#";
            component.prefixLabel->setText(typePrefix, juce::dontSendNotification);

            // Validate value in TextEditor (0-127) - now handled automatically by NumericTextEditor
            int value = component.valueEditor->getNumericValue();

            // Value is automatically constrained by NumericTextEditor
            // No need for manual validation
        }
    }
}