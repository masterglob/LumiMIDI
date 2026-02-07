// =============================================================================
// PageManager.h
// =============================================================================
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include "LedConfigurationPage.h"
#include "ProgrammingPage.h"


class PageManager : public juce::Component {
 public:
  enum class PageType { LedConfiguration, Programming };

  PageManager(LumiMIDIProcessor& processor,
              juce::AudioProcessorValueTreeState& apvts,
              UI_WorldView& worldView,
              juce::MidiKeyboardState& keyboardState);
  ~PageManager() override = default;

  // Component overrides
  void resized() override;

  // Page management
  void switchToPage(PageType pageType);
  PageType getCurrentPageType() const { return mCurrentPage; }

  // Page access
  LedConfigurationPage& getLedConfigPage() { return mLedConfigPage; }
  ProgrammingPage& getProgrammingPage() { return mProgrammingPage; }

 private:
  void setupPages();
  PageBase* getCurrentPage();
  void updateTabAppearance();

 private:
  LumiMIDIProcessor& mProcessor;
  juce::AudioProcessorValueTreeState& mApvts;

  LedConfigurationPage mLedConfigPage;
  ProgrammingPage mProgrammingPage;

  juce::TextButton mTabLedConfig;
  juce::TextButton mTabProgramming;

  PageType mCurrentPage;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PageManager)
};
