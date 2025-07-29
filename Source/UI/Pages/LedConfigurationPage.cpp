// =============================================================================
// LedConfigurationPage.cpp
// =============================================================================
#include "UI/Pages/LedConfigurationPage.h"

LedConfigurationPage::LedConfigurationPage(LumiMIDIProcessor& processor,
    juce::AudioProcessorValueTreeState& apvts,
    UI_WorldView& worldView)
    : mProcessor(processor)
    , mApvts(apvts)
    , mWorldView(worldView)
    , mBtnLearn("Learn")
    , mCcSender(processor.getAudioEngine())
{
    setupComponents();
    setupLayout();
}

void LedConfigurationPage::paint(juce::Graphics& g) {

    // Debug: Dessiner un fond coloré pour vérifier que la page se dessine
    g.fillAll(juce::Colours::darkgreen.withAlpha(0.1f));

    auto titleArea = getLocalBounds().removeFromTop(30);

    // Titre de la section
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("LED Configuration", titleArea,
        juce::Justification::centred);
}

void LedConfigurationPage::resized() {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(35); // Espace pour le titre

    // WorldView occupe la majorité de l'espace (85%)
    auto worldViewArea = bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.85f));
    mWorldView.setBounds(worldViewArea.reduced(5));
    auto* parent = mWorldView.getParentComponent();

    // Contrôles en bas (15% restant) - arrangés horizontalement
    bounds.removeFromTop(10); // Espacement
    auto controlArea = bounds.reduced(10);

    auto buttonWidth = 100;
    auto buttonArea = controlArea.removeFromTop(40);
    mBtnLearn.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));

    // CcSender dans le reste de l'espace
    mCcSender.setBounds(controlArea.reduced(5));

}

void LedConfigurationPage::activate() {
    mIsActive = true;
    setVisible(true);
    // CORRECTION: Retirer le WorldView de son parent actuel avant de l'ajouter
    auto* currentParent = mWorldView.getParentComponent();
    if (currentParent && currentParent != this) {
        currentParent->removeChildComponent(&mWorldView);
        currentParent->repaint();
    }
    
    // Ajouter le WorldView à cette page
    addAndMakeVisible(mWorldView);
    mWorldView.setShowLedNames(true);
    mWorldView.setRefreshRate(10);
    mWorldView.setViewMode(UI_WorldView::ViewMode::Full);

     
    // S'assurer que tous les composants sont visibles
    mWorldView.setVisible(true);

    repaint();
}

void LedConfigurationPage::deactivate() {
    mIsActive = false;
    setVisible(false);
}

void LedConfigurationPage::setupComponents() {
    // Ne PAS ajouter le WorldView ici - il sera ajouté dans activate()
    // addAndMakeVisible(mWorldView); 

    addAndMakeVisible(mBtnLearn);
    addAndMakeVisible(mCcSender);
    addAndMakeVisible(mWorldView);

    // Configuration des callbacks
    mBtnLearn.onClick = [this]() {
        // Logique du bouton Learn
        // Par exemple : activer/désactiver le mode apprentissage
        };
}

void LedConfigurationPage::setupLayout() {
    // Configuration des knobs, liaisons callbacks, etc.
    // Ajout de toute logique de setup spécifique
}