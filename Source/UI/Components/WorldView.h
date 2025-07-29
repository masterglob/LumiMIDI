
// ============================================================================
// UI/Components/WorldView.h
// ============================================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "DSP/AudioEngine.h"
class UI_WorldView : public juce::Component, public juce::Timer {
public:
    enum class ViewMode {
        Compact,    // Pour page Programming
        Full        // Pour page LED Config
    };

    UI_WorldView(juce::AudioProcessorValueTreeState& apvts, const AudioEngine& engine);
    ~UI_WorldView() override;

    // Configuration
    void setViewMode(ViewMode mode);
    void setShowLedNames(bool show) { mShowLedNames = show; repaint(); }
    void setRefreshRate(int hz) { startTimerHz(hz); }

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Timer override pour les mises à jour temps réel
    void timerCallback() override;

    // Get information of a LED at a given position
    // return nullptr if no led matches
    LedContext* getLedAt(const juce::Point<int>& p);

private:
    void refreshLeds();
    void paintBackground(juce::Graphics& g);
    void paintLeds(juce::Graphics& g);
    void paintLedNames(juce::Graphics& g);

    // Calcul des dimensions selon le mode
    juce::Rectangle<int> getDisplayArea() const;
    float getScaleFactor() const;

private:
    juce::AudioProcessorValueTreeState& mApvts;
    const AudioEngine& mEngine;

    using LedsMap = std::map<LedId, LedContext>;
    LedsMap mLedsMap;

    ViewMode mViewMode = ViewMode::Full;
    bool mModified = true;
    bool mShowLedNames = false;

    // Paramètres configurables
    static constexpr int FULL_WIDTH = 800;
    static constexpr int FULL_HEIGHT = 500;
    static constexpr int COMPACT_WIDTH = 400;
    static constexpr int COMPACT_HEIGHT = 250;
    static constexpr int MARGIN = 10;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_WorldView)
};