// =============================================================================
// PageBase.h
// =============================================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class PageBase : public juce::Component {
public:
    PageBase() = default;
    virtual ~PageBase() = default;

    virtual void activate() {}
    virtual void deactivate() {}
    virtual bool isActive() const { return mIsActive; }

protected:
    bool mIsActive = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PageBase)
};