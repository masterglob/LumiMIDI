
// ============================================================================
// UI/Components/CcSender.cpp
// ============================================================================

#include <map>
#include "CcSender.h"

#define LED_CONTOUR 1

using namespace std;
namespace
{
}

UI_CcSender::UI_CcSender(const AudioEngine& engine):
    mEngine(engine)
{
    // CC Sender editor
    // Set up the label
    mSend_CC_Label.setText("CC To send:", juce::dontSendNotification);
    mSend_CC_Label.attachToComponent(&mSend_CC_TextEditor, true);
    addAndMakeVisible(mSend_CC_Label);

    // SEND CC AREA
    mSend_CC_TextEditor.setMultiLine(false);
    mSend_CC_TextEditor.setReturnKeyStartsNewLine(false);
    mSend_CC_TextEditor.setReadOnly(false);
    mSend_CC_TextEditor.setScrollbarsShown(false);
    mSend_CC_TextEditor.setCaretVisible(true);
    mSend_CC_TextEditor.setPopupMenuEnabled(true);
    mSend_CC_TextEditor.setText("0"); // Default value

    mSend_CC_Button.setButtonText("Apply");
    mSend_CC_Button.onClick = [this]() { send_CC_Clicked(); };
    addAndMakeVisible(mSend_CC_Button);

    // Apply integer-only filter
    integerFilter = std::make_unique<IntegerTextEditorFilter>();
    mSend_CC_TextEditor.setInputFilter(integerFilter.get(), false);

    // Add listener for when text changes
    mSend_CC_TextEditor.onTextChange = [this]() { send_CC_TextChanged(); };

    addAndMakeVisible(mSend_CC_TextEditor);

}

void UI_CcSender::send_CC_TextChanged()
{
    juce::String text = mSend_CC_TextEditor.getText();

    if (text.isNotEmpty())
    {
        int value = text.getIntValue();

        // Validate range if needed
        if (value < 0)
        {
            mSend_CC_TextEditor.setText(juce::String("0"), false);
        }
        if (value > 127)
        {
            mSend_CC_TextEditor.setText(juce::String("127"), false);
        }
    }
}

void UI_CcSender::send_CC_Clicked()
{
    int currentValue = mSend_CC_TextEditor.getText().getIntValue();
    if (currentValue < 0) return;
    DBG("Button clicked! Current value: " + juce::String(static_cast<unsigned int>(currentValue)));
}

void UI_CcSender::paint(juce::Graphics& g)
{
    auto area = getLocalBounds();
    int w(area.getWidth());
    if (w > 250)
        area.removeFromRight(w - 250);

    g.setColour(juce::Colours::white);
    g.drawRect(area);
    area.reduce(1, 1);

    g.setColour(juce::Colours::green);
    g.fillRect(area);
}

void UI_CcSender::resized()
{
    auto area = getLocalBounds();
    area.reduce(5, 5);
    int w(area.getWidth());
    if (w > 250)
        area.removeFromRight(w-250);
    mSend_CC_TextEditor.setBounds(area.removeFromLeft(60));
    area.removeFromLeft(10);
    mSend_CC_Label.setBounds(area.removeFromLeft(80));
    area.removeFromLeft(10);
    mSend_CC_Button.setBounds(area.removeFromLeft(80));
}
