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
    , mRedLine{this, "Red" }
    , mGreenLine{ this, "Green" }
    , mBlueLine{ this, "Blue" }
    , mWhiteLine{ this, "White" }
    , mBtnSaveConfig("Save Config")
    , mBtnLoadConfig("Load Config")
    , mBtnApply("Apply")
    , mBtnCancel("Cancel")
    , mBtnAdd("Add")
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

    for (LedLineEditor* editor : { &mRedLine, &mGreenLine, &mBlueLine, &mWhiteLine }) {
        auto row = midiContent.removeFromTop(midiRowHeight);
        editor->resized(row);
        midiContent.removeFromTop(spacing2);
    }
    // Actions group (bottom)
    propertiesArea.removeFromTop(10); // Spacing
    mActionsGroup.setBounds(propertiesArea);

    auto actionsContent = propertiesArea.reduced(10, 25);
    auto actionButtonHeight = 30;
    auto actionButtonWidth = 70;

    // Second row: Apply/Cancel (remplace Test LED/Test All)
    auto secondActionRow = actionsContent.removeFromTop(actionButtonHeight);
    mBtnApply.setBounds(secondActionRow.removeFromLeft(actionButtonWidth));
    secondActionRow.removeFromLeft(spacing2);
    mBtnCancel.setBounds(secondActionRow.removeFromLeft(actionButtonWidth));
    secondActionRow.removeFromLeft(spacing2);
    mBtnAdd.setBounds(secondActionRow.removeFromLeft(actionButtonWidth));

    // First row: Save/Load
    auto firstActionRow = actionsContent.removeFromTop(actionButtonHeight);
    mBtnSaveConfig.setBounds(firstActionRow.removeFromLeft(actionButtonWidth));
    firstActionRow.removeFromLeft(spacing2);
    mBtnLoadConfig.setBounds(firstActionRow.removeFromLeft(actionButtonWidth));

    actionsContent.removeFromTop(spacing2);

}

void LedConfigurationPage::mouseDown(const juce::MouseEvent& event) {
    auto worldViewMousePos = event.getPosition() - mWorldView.getBounds().getTopLeft();

    switch (mCurrentEditMode) {
    case EditMode::None:
    case EditMode::EditingLed:
        // Check if click is in WorldView area
        if (mWorldView.getBounds().contains(event.getPosition())) {
            // Convert mouse position to WorldView coordinates

            // Get LED at click position
            LedId clickedLed = mWorldView.getLedAt(worldViewMousePos);
            mIsEditingLed = false;

            if (clickedLed != NO_LED) {
                // LED was clicked - select it and update interface
                mCurrentEditMode = EditMode::EditingLed;
                selectLed(clickedLed);
            }
            else
            {
                mCurrentEditMode = EditMode::None;
                selectLed(NO_LED);
            }
        }
        break;
    case EditMode::AddingLed:
        if (mAddingLedCtxt)
        {
            Point worldPoint(mWorldView.getPosAt(worldViewMousePos));
            mAddingLedCtxt->pos = LedPosition(worldPoint, Point(0,100));
            DBG("Adde led at  " << mAddingLedCtxt->pos.topLeft.toString()
                << ", " << mAddingLedCtxt->pos.size.toString()
                << ", " << mAddingLedCtxt->pos.center.toString());
            mAddingLedCtxt->name = mLedNameEditor.getText();
            mAddingLedCtxt->ctrl.mr = mRedLine.getNumericValue();
            mAddingLedCtxt->ctrl.mg = mGreenLine.getNumericValue();
            mAddingLedCtxt->ctrl.mb = mBlueLine.getNumericValue();
            bool isRGBW = mLedTypeCombo.getSelectedId() == 2;
            if (isRGBW)
            {
                mAddingLedCtxt->ctrl.hasWhite = 1;
                mAddingLedCtxt->ctrl.mw = mWhiteLine.getNumericValue();
            }
            else
            {
                mAddingLedCtxt->ctrl.hasWhite = 0;
                mAddingLedCtxt->ctrl.mw = 0;
            }

        }
        break;
    case EditMode::MovingLed:
        break;
    case EditMode::ResizingLed:
        break;
    }
    refreshBtns();
    updateSelectedLedInfo();
}

void LedConfigurationPage::mouseDrag(const juce::MouseEvent& event) {
    // Handle drag to move/resize LEDs
    (void)event;
}

void LedConfigurationPage::mouseMove(const juce::MouseEvent& event) {
    // Handle mouse move for hover effects, cursor changes, etc.
    // Check if mouse is over WorldView using the correct coordinate system
    if (mWorldView.getBounds().contains(event.getPosition())) {
        // Convert mouse position to WorldView coordinates
        auto worldViewMousePos = event.getPosition() - mWorldView.getBounds().getTopLeft();

        // Get LED at cursor position
        const  LedContext* ledUnderCursor = mWorldView.getLed(mWorldView.getLedAt(worldViewMousePos));

        if (ledUnderCursor != nullptr) {
            // Mouse is over a LED
            juce::String ledName = juce::String(ledUnderCursor->name);

            // Update cursor for LED interaction
            switch (mCurrentEditMode) {
            case EditMode::None:
                setMouseCursor(juce::MouseCursor::PointingHandCursor); // Indicate clickable LED
                break;
            case EditMode::EditingLed:
                setMouseCursor(juce::MouseCursor::PointingHandCursor); // Indicate clickable LED
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

            // TODO: Show LED tooltip or highlight
            // setTooltip("LED: " + ledName + " (" + ledUnderCursor->ctrl.toString() + ")");
        }
        else {
            // Mouse over WorldView but no LED

            switch (mCurrentEditMode) {
            case EditMode::None:
                setMouseCursor(juce::MouseCursor::NormalCursor);
                break;
            case EditMode::AddingLed:
                setMouseCursor(juce::MouseCursor::CrosshairCursor);
                break;
            case EditMode::MovingLed:
            case EditMode::ResizingLed:
                setMouseCursor(juce::MouseCursor::NormalCursor);
                break;
            }

            // TODO: Clear tooltip
            // setTooltip("");
        }
    }
    else {
        // Mouse outside WorldView
        setMouseCursor(juce::MouseCursor::NormalCursor);

        // TODO: Clear tooltip
        // setTooltip("");
    }
}

void LedConfigurationPage::mouseUp(const juce::MouseEvent& event) {
    // End interaction
    (void)event;
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
    for (LedLineEditor* editor : { &mRedLine, &mGreenLine, &mBlueLine, &mWhiteLine })
    {
        editor->setupComponents(*this);
    }

    // === Actions ===
    addAndMakeVisible(mBtnSaveConfig);
    addAndMakeVisible(mBtnLoadConfig);
    addAndMakeVisible(mBtnApply);
    addAndMakeVisible(mBtnCancel);
    addAndMakeVisible(mBtnAdd);

    // Personnalisation des nouveaux boutons
    refreshBtns();

    // Component configuration
    mLedTypeCombo.addItem("RGB", 1);
    mLedTypeCombo.addItem("RGBW", 2);
    mLedTypeCombo.setSelectedId(1);

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

    // Nouveaux callbacks pour Apply/Cancel
    mBtnApply.onClick = [this]() { handleApplyButtonClicked(); };
    mBtnCancel.onClick = [this]() { handleCancelButtonClicked(); };
    mBtnAdd.onClick = [this]() { handleAddButtonClicked(); };

    mLedNameEditor.onTextChange = [this]() { mIsEditingLed = true; onLedNameChanged(); };
    mLedLengthSlider.onValueChange = [this]() { mIsEditingLed = true; onLedLengthChanged(); };
    mLedTypeCombo.onChange = [this]() {mIsEditingLed = true;  onLedTypeChanged();  };

}

// Methods to implement (for now, just stubs)
void LedConfigurationPage::handleWorldViewClick(const juce::MouseEvent& event) {
    // TODO: Implement LED selection/creation
    (void)event;
}

void LedConfigurationPage::selectLed(LedId ledId) {
    mSelectedLed.reset(new LedId(ledId));
    updateSelectedLedInfo();
}

void LedConfigurationPage::updateSelectedLedInfo() {

    const LedContext* led(getEditingLed());

    if (led == nullptr) {
        // Clear selection - reset all fields to default/empty
        mLedNameEditor.setText("", false);
        mLedLengthSlider.setValue(100, juce::dontSendNotification);
        mLedLengthValue.setText("100", juce::dontSendNotification);
        mLedTypeCombo.setSelectedId(1, juce::dontSendNotification); // RGB
        mPositionValue.setText("", juce::dontSendNotification);

        // Reset MIDI mapping to defaults
        int ccInit{ 1 };
        // Reset all MIDI types to None
        for (LedLineEditor* editor : { &mRedLine, &mGreenLine, &mBlueLine, &mWhiteLine }) {
            editor->setSelectedId(1);
            editor->setNumericValue(ccInit);
            ccInit++;
        }
    }
    else
    {
        mLedNameEditor.setText(led->name, false);

        const int ledLength(led->pos.getLength());
        mLedLengthSlider.setValue(ledLength, juce::dontSendNotification);
        mLedLengthValue.setText(juce::String(ledLength), juce::dontSendNotification);

        bool isRGBW = led->ctrl.hasWhite;
        mLedTypeCombo.setSelectedId(isRGBW ? 2 : 1, juce::dontSendNotification);

        juce::String positionText = juce::String::formatted("(%d, %d) - (%d, %d)",
            led->pos.topLeft.getX(),
            led->pos.topLeft.getY(),
            led->pos.topLeft.getX() + led->pos.size.getX(),
            led->pos.topLeft.getY() + led->pos.size.getY());
        mPositionValue.setText(positionText, juce::dontSendNotification);

        // Red component
        mRedLine.setCcNumber(led->ctrl.mr);

        // Green component
        mGreenLine.setCcNumber(led->ctrl.mg);

        // Blue component
        mBlueLine.setCcNumber(led->ctrl.mb);

        // White component (if RGBW)
        mWhiteLine.setCcNumber(led->ctrl.mw);
    }

    // Trigger MIDI mapping update to hide prefix/editors
    onMidiMappingChanged(false);
    onLedTypeChanged(); // Update White component visibility
}

void LedConfigurationPage::addNewLed() {
    // TODO: Add a new LED
    juce::Logger::writeToLog("Add LED button clicked");

    mProcessor.getAudioEngine().updateLeds();

}

void LedConfigurationPage::removeLed() {
    // TODO: Remove selected LED
    juce::Logger::writeToLog("Remove LED button clicked");
}

void LedConfigurationPage::duplicateLed() {
    // TODO: Duplicate selected LED
    juce::Logger::writeToLog("Duplicate LED button clicked");
}

void LedConfigurationPage::onLedNameChanged() {
    // TODO: Update LED name

    LedContext* led(getEditingLed());
    if (led)
    {
        led->name = mLedNameEditor.getText();
    }

    refreshBtns();
    juce::Logger::writeToLog("LED name changed to: " + mLedNameEditor.getText());
}

void LedConfigurationPage::onLedLengthChanged() {
    // TODO: Update LED length
    mLedLengthValue.setText(juce::String(static_cast<int>(mLedLengthSlider.getValue())), juce::dontSendNotification);
    
    refreshBtns();
    juce::Logger::writeToLog("LED length changed to: " + juce::String(static_cast<int>(mLedLengthSlider.getValue())));
}

void LedConfigurationPage::onLedTypeChanged() {
    // TODO: Update LED type and show/hide White component
    bool isRGBW = mLedTypeCombo.getSelectedId() == 2;
    mWhiteLine.setVisible(isRGBW);

    if (isRGBW)
    {
        mWhiteLine.setSelectedId(2);
    }
    refreshBtns();

    juce::Logger::writeToLog("LED type changed to: " + (isRGBW ? juce::String("RGBW") : juce::String("RGB")));
}

void LedConfigurationPage::onMidiMappingChanged(bool manual) {
    if (manual)
    {
        mIsEditingLed = true;
    }
    // Structure to iterate through all MIDI components
    struct MidiComponentSet {
        juce::ComboBox* typeCombo;
        juce::Label* prefixLabel;
        NumericTextEditor* valueEditor;
    };

    for (LedLineEditor* editor : { &mRedLine, &mGreenLine, &mBlueLine, &mWhiteLine }) {
        editor->refresh();
    }
    refreshBtns();
}

void LedConfigurationPage::refreshBtns()
{
    bool btnApplyEnabled(false);
    bool btnAddEnabled(false);
    bool btnCancelEnabled(false);
    DBG("refreshBtns: Mode=" << (int)mCurrentEditMode << ", mIsEditingLed="<< (int) mIsEditingLed);

    switch (mCurrentEditMode)
    {
    case LedConfigurationPage::EditMode::None:
        btnAddEnabled = true;
        break;
    case LedConfigurationPage::EditMode::EditingLed:
        btnApplyEnabled = mIsEditingLed;
        btnCancelEnabled = true;
        break;
    case LedConfigurationPage::EditMode::AddingLed:
        btnCancelEnabled = true;
        btnApplyEnabled = mAddingLedCtxt.get() && mAddingLedCtxt->name != "";
        break;
    case LedConfigurationPage::EditMode::MovingLed:
        break;
    case LedConfigurationPage::EditMode::ResizingLed:
        break;
    default:
        break;
    }

    mBtnApply.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    mBtnApply.setColour(juce::TextButton::buttonColourId,
        (btnApplyEnabled ? juce::Colours::green.darker() : juce::Colours::grey));
    mBtnApply.setEnabled(btnApplyEnabled);
    mBtnCancel.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    mBtnCancel.setColour(juce::TextButton::buttonColourId,
        (btnCancelEnabled ? juce::Colours::red : juce::Colours::grey));
    mBtnCancel.setEnabled(btnCancelEnabled);

    mBtnAdd.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    mBtnAdd.setColour(juce::TextButton::buttonColourId,
        (btnAddEnabled ? juce::Colours::green.darker() : juce::Colours::grey));
    mBtnAdd.setEnabled(btnAddEnabled);

}

void LedConfigurationPage::handleApplyButtonClicked() {

    LedContext* led(getEditingLed());

    if (led != nullptr) {
        DBG("Applying changes for LED: " << juce::String(led->name));

        try
        {
            // read new values
            juce::String newName = mLedNameEditor.getText();
            int newLength = static_cast<int>(mLedLengthSlider.getValue());
            bool hasWhite = mLedTypeCombo.getSelectedId() == 2;

            struct Update { LedLineEditor& ed; LineId& line; };
            const std::vector< Update> updates{
                {mRedLine, led->ctrl.mr} ,
                {mGreenLine, led->ctrl.mg} ,
                {mBlueLine, led->ctrl.mb} ,
                {mWhiteLine, led->ctrl.mw} };

            for (const Update& update : updates) {
                int ccValue{ 0 };

                if (update.ed.getSelectedId() == 2)
                {
                    update.line = update.ed.getNumericValue();
                }
            }

            // Apply only if all is correct
            led->name = newName;
            led->ctrl.hasWhite = hasWhite;
            mIsEditingLed = false;
            DBG("Applyed changes for LED: " << juce::String(led->name));
        }
        catch (...)
        {

        }
        if (mAddingLedCtxt && mCurrentEditMode == EditMode::AddingLed)
        {
            DBG("Add led " << led->name << " with R ="<< (int) mAddingLedCtxt->ctrl.mr);
            LedDB& db = mProcessor.getAudioEngine().getLeds();
            db.addLed(*led);
            db.doneEditing();
            mCurrentEditMode = EditMode::None;
            mSelectedLed.reset();
            mAddingLedCtxt.reset();
            mIsEditingLed = false;
            mProcessor.getAudioEngine().updateLeds();

        }

    }

    // TODO: Sauvegarder globalement si nécessaire
    // mProcessor.applyLedConfiguration();
    refreshBtns();
}

void LedConfigurationPage::handleAddButtonClicked() {
    mCurrentEditMode = EditMode::AddingLed;
    mAddingLedCtxt.reset(new LedContext);
    mAddingLedCtxt->pos.topLeft = juce::Point(-1, -1);
    mAddingLedCtxt->pos.size = juce::Point(-1, -1);
    mAddingLedCtxt->pos.center = juce::Point(-1, -1);
}

void LedConfigurationPage::handleCancelButtonClicked() {
    juce::Logger::writeToLog("Cancel button clicked - Canceling LED configuration changes");

    mCurrentEditMode = EditMode::None;
    mSelectedLed.reset();
    mAddingLedCtxt.reset();
    mIsEditingLed = false;

    const LedContext* led(getEditingLed());

    if (led != nullptr) {
        juce::Logger::writeToLog("Canceling changes for LED: " + juce::String(led->name));

        // Restore initial values
        updateSelectedLedInfo();

        juce::Logger::writeToLog("LED properties restored to original values");
    }
    else {
        // No led selected
        selectLed(NO_LED); // Will empty all fields
        juce::Logger::writeToLog("Interface reset to default values");
    }

    refreshBtns();
}

LedContext* LedConfigurationPage::getEditingLed()
{
    LedContext* led(mSelectedLed ? mWorldView.getLed(*mSelectedLed) : nullptr);
    if (mAddingLedCtxt)
    {
        led = mAddingLedCtxt.get();
    }
    return led;
}