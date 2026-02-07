
// =============================================================================
// PageManager.cpp
// =============================================================================
#include "UI/Pages/PageManager.h"

PageManager::PageManager(LumiMIDIProcessor& processor,
                         juce::AudioProcessorValueTreeState& apvts,
                         UI_WorldView& worldView,
                         juce::MidiKeyboardState& keyboardState)
    : mProcessor(processor),
      mApvts(apvts),
      mLedConfigPage(processor, apvts, worldView),
      mProgrammingPage(processor, apvts, keyboardState, worldView),
      mCurrentPage(PageType::Programming) {
  setupPages();
  switchToPage(PageType::LedConfiguration);
}

void PageManager::resized() {
  auto bounds = getLocalBounds();

  // Barre d'onglets en haut
  auto tabBarHeight = 40;
  auto tabBar = bounds.removeFromTop(tabBarHeight);

  auto tabWidth = tabBar.getWidth() / 2;
  mTabLedConfig.setBounds(tabBar.removeFromLeft(tabWidth));
  mTabProgramming.setBounds(tabBar);

  // Zone des pages
  mLedConfigPage.setBounds(bounds);
  mProgrammingPage.setBounds(bounds);
}

void PageManager::switchToPage(PageType pageType) {
  if (mCurrentPage == pageType) return;

  // D�sactiver la page actuelle
  getCurrentPage()->deactivate();

  // Changer de page
  mCurrentPage = pageType;

  // Activer la nouvelle page
  getCurrentPage()->activate();
  getCurrentPage()->resized();

  // Mettre � jour l'apparence des onglets
  updateTabAppearance();
}

void PageManager::setupPages() {
  addAndMakeVisible(mLedConfigPage);
  addAndMakeVisible(mProgrammingPage);

  // Configuration des onglets
  addAndMakeVisible(mTabLedConfig);
  addAndMakeVisible(mTabProgramming);

  mTabLedConfig.setButtonText("LED Config");
  mTabProgramming.setButtonText("Programming");

  mTabLedConfig.onClick = [this]() { switchToPage(PageType::LedConfiguration); };
  mTabProgramming.onClick = [this]() { switchToPage(PageType::Programming); };
}

PageBase* PageManager::getCurrentPage() {
  switch (mCurrentPage) {
    case PageType::LedConfiguration:
      return &mLedConfigPage;
    case PageType::Programming:
      return &mProgrammingPage;
    default:
      return &mLedConfigPage;
  }
}

void PageManager::updateTabAppearance() {
  // Mettre � jour l'apparence des onglets selon la page active
  mTabLedConfig.setToggleState(mCurrentPage == PageType::LedConfiguration, juce::dontSendNotification);
  mTabProgramming.setToggleState(mCurrentPage == PageType::Programming, juce::dontSendNotification);
}
