// =============================================================================
// UI/Components/NumericTextEditor.cpp
// =============================================================================
#include "UI/Components/NumericTextEditor.h"

NumericTextEditor::NumericTextEditor() {
    // Configure for numeric input by default
    setInputRestrictions(3, "0123456789");
    setFont(juce::Font(14.0f));
    setTextToShowWhenEmpty("0", juce::Colours::grey);

    // Set up text change callback to validate input
    onTextChange = [this]() {
        validateAndUpdateValue();
        };
}

void NumericTextEditor::setRange(int minValue, int maxValue) {
    mMinValue = minValue;
    mMaxValue = maxValue;

    // Update input restrictions based on max value
    int maxDigits = juce::String(mMaxValue).length();
    setInputRestrictions(maxDigits, "0123456789");

    // Validate current value against new range
    validateAndUpdateValue();
}

void NumericTextEditor::setWheelIncrement(int increment) {
    mWheelIncrement = juce::jmax(1, increment);
}

void NumericTextEditor::setShiftMultiplier(int multiplier) {
    mShiftMultiplier = juce::jmax(1, multiplier);
}

void NumericTextEditor::setArrowIncrement(int increment) {
    mArrowIncrement = juce::jmax(1, increment);
}

void NumericTextEditor::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) {
    // Get current value
    int currentValue = getNumericValue();

    // Calculate increment (wheel.deltaY > 0 = scroll up = increment)
    int increment = wheel.deltaY > 0 ? mWheelIncrement : -mWheelIncrement;

    // Apply modifier keys for faster scrolling
    if (event.mods.isShiftDown()) {
        increment *= mShiftMultiplier;
    }

    // Calculate new value
    int newValue = currentValue + increment;

    // Set the new value (automatically constrained)
    setNumericValue(newValue, true);
}

bool NumericTextEditor::keyPressed(const juce::KeyPress& key) {
    // Handle arrow keys for increment/decrement
    if (key == juce::KeyPress::upKey || key == juce::KeyPress::downKey) {
        // Get current value
        int currentValue = getNumericValue();

        // Calculate increment (up = increment, down = decrement)
        int increment = key == juce::KeyPress::upKey ? mArrowIncrement : -mArrowIncrement;

        // Apply modifier keys for faster adjustment
        if (key.getModifiers().isShiftDown()) {
            increment *= mShiftMultiplier;
        }

        // Calculate and set new value
        int newValue = currentValue + increment;
        setNumericValue(newValue, true);

        return true; // Key was handled
    }

    // Let the base class handle other keys (typing, navigation, etc.)
    return TextEditor::keyPressed(key);
}

int NumericTextEditor::getNumericValue() const {
    juce::String text = getText();
    if (text.isEmpty()) {
        return mMinValue;
    }
    return text.getIntValue();
}

void NumericTextEditor::setNumericValue(int value, bool sendNotification) {
    // Constrain value to valid range
    int constrainedValue = juce::jlimit(mMinValue, mMaxValue, value);

    // Update text if value changed
    if (constrainedValue != getNumericValue()) {
        setText(juce::String(constrainedValue), sendNotification);
    }
}

void NumericTextEditor::validateAndUpdateValue() {
    // Get current text value
    juce::String currentText = getText();

    // Skip validation if empty (will show placeholder)
    if (currentText.isEmpty()) {
        return;
    }

    // Get numeric value and constrain
    int value = currentText.getIntValue();
    int constrainedValue = juce::jlimit(mMinValue, mMaxValue, value);

    // Update text if value was out of range
    if (constrainedValue != value) {
        // Don't send notification to avoid recursion
        setText(juce::String(constrainedValue), false);
    }
}