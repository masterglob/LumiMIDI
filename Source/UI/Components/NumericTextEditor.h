// =============================================================================
// UI/Components/NumericTextEditor.h
// =============================================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

/**
 * Custom TextEditor with mouse wheel support for numeric values.
 * Supports mouse wheel scrolling with increment/decrement functionality.
 * Values are automatically constrained between min and max bounds.
 */
class NumericTextEditor : public juce::TextEditor {
public:
    NumericTextEditor();
    ~NumericTextEditor() override = default;

    // Set the valid range for numeric values
    void setRange(int minValue, int maxValue);

    // Set the increment step for mouse wheel (default = 1)
    void setWheelIncrement(int increment);

    // Set the increment multiplier when Shift is held (default = 10)
    void setShiftMultiplier(int multiplier);

    // Set the increment step for arrow keys (default = 1)
    void setArrowIncrement(int increment);

    // Component overrides
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
    bool keyPressed(const juce::KeyPress& key) override;

    // Get current numeric value
    int getNumericValue() const;

    // Set numeric value (with range validation)
    void setNumericValue(int value, bool sendNotification = true);

private:
    void validateAndUpdateValue();

private:
    int mMinValue = 0;
    int mMaxValue = 127;
    int mWheelIncrement = 1;
    int mArrowIncrement = 1;
    int mShiftMultiplier = 10;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NumericTextEditor)
};