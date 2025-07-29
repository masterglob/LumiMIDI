
// ============================================================================
// UI/Components/WorldView.cpp
// ============================================================================

#include "WorldView.h"
#include "DSP/AudioEngine.h"
#include "DSP/BaseProgram.h"

UI_WorldView::UI_WorldView(juce::AudioProcessorValueTreeState& apvts, const AudioEngine& engine)
    : mApvts(apvts)
    , mEngine(engine) {
    startTimerHz(30); // 30 FPS par défaut
}

UI_WorldView::~UI_WorldView() {
    stopTimer();
}

void UI_WorldView::setViewMode(ViewMode mode) {
    if (mViewMode != mode) {
        mViewMode = mode;
        mModified = true;
        repaint();
    }
}

void UI_WorldView::timerCallback() {
    // Mise à jour temps réel - plus efficace que dans paint()
    refreshLeds();
    repaint();
}


LedContext* UI_WorldView::getLedAt(const juce::Point<int>& p) {
    // Convert int point to float for precise comparison
    juce::Point<float> mousePos(static_cast<float>(p.x), static_cast<float>(p.y));

    // Iterate through all LEDs and check bounds
    for (auto& it : mLedsMap) {
        const LedContext& led = it.second;
        juce::Rectangle<float> ledBounds = getLedBounds(led);

        // Expand bounds slightly to account for LED width/stroke
        float scale = getScaleFactor();
        float ledWidth = static_cast<float>(led.width) * scale;
        float expansion = ledWidth * 0.5f; // Half stroke width on each side
        ledBounds = ledBounds.expanded(expansion);

        if (ledBounds.contains(mousePos)) {
            return const_cast<LedContext*>(&led);
        }
    }

    return nullptr; // No LED found at this position
}

juce::Rectangle<int> UI_WorldView::getDisplayArea() const {
    auto bounds = getLocalBounds().reduced(MARGIN);

    switch (mViewMode) {
    case ViewMode::Compact: {
        // Centrer dans l'espace disponible
        int width = std::min(bounds.getWidth(), COMPACT_WIDTH);
        int height = std::min(bounds.getHeight(), COMPACT_HEIGHT);
        return bounds.withSizeKeepingCentre(width, height);
    }
    case ViewMode::Full: {
        // Utiliser tout l'espace disponible en gardant les proportions
        float aspectRatio = static_cast<float>(FULL_WIDTH) / FULL_HEIGHT;
        int width = bounds.getWidth();
        int height = static_cast<int>(width / aspectRatio);

        if (height > bounds.getHeight()) {
            height = bounds.getHeight();
            width = static_cast<int>(height * aspectRatio);
        }

        return bounds.withSizeKeepingCentre(width, height);
    }
    }
    return bounds;
}

float UI_WorldView::getScaleFactor() const {
    auto displayArea = getDisplayArea();
    float scaleX = static_cast<float>(displayArea.getWidth()) / FULL_WIDTH;
    float scaleY = static_cast<float>(displayArea.getHeight()) / FULL_HEIGHT;
    return std::min(scaleX, scaleY);
}

void UI_WorldView::refreshLeds() {
    mLedsMap.clear();
    LedId ledId(0);
    for (const LedContext* it : mEngine.getLeds()) {
        mLedsMap.emplace(ledId, *it);
        ledId++;
    }
    mModified = false;
}

void UI_WorldView::paint(juce::Graphics& g) {

    paintBackground(g);
    paintLeds(g);

    if (mShowLedNames) {
        paintLedNames(g);
    }
}

void UI_WorldView::paintBackground(juce::Graphics& g) {
    auto displayArea = getDisplayArea();

    // Bordure
    g.setColour(juce::Colours::white);
    g.drawRect(displayArea, 1);

    // Gradient de fond
    auto innerArea = displayArea.reduced(1);
    juce::ColourGradient gradient(
        juce::Colour(0xff0a0a0e),
        innerArea.getTopLeft().toFloat(),
        juce::Colour(0xff060110),
        innerArea.getBottomRight().toFloat(),
        false
    );

    g.reduceClipRegion(innerArea);
    g.setGradientFill(gradient);
    g.fillRect(innerArea);
}

std::function<float(int)> UI_WorldView::getToXTransform() {
    auto displayArea = getDisplayArea();
    float scale = getScaleFactor();
    return [displayArea, scale](int x) {
        return displayArea.getX() + static_cast<float>(x) * scale;
        };
}

std::function<float(int)> UI_WorldView::getToYTransform() {
    auto displayArea = getDisplayArea();
    float scale = getScaleFactor();
    return [displayArea, scale](int y) {
        return displayArea.getBottom() - static_cast<float>(y) * scale;
        };
}

juce::Rectangle<float> UI_WorldView::getLedBounds(const LedContext& led) {
    auto toX = getToXTransform();
    auto toY = getToYTransform();

    const LedPosition& pos = led.pos;
    float x0 = toX(pos.topLeft.getX());
    float y0 = toY(pos.topLeft.getY());
    float x1 = x0 + pos.size.getX() * getScaleFactor();
    float y1 = y0 - pos.size.getY() * getScaleFactor();

    // Create rectangle (ensure proper top-left, width, height)
    float left = std::min(x0, x1);
    float top = std::min(y0, y1);
    float width = std::abs(x1 - x0);
    float height = std::abs(y1 - y0);

    return juce::Rectangle<float>(left, top, width, height);
}
void UI_WorldView::paintLeds(juce::Graphics& g) {
    auto toX = getToXTransform();
    auto toY = getToYTransform();
    float scale = getScaleFactor();

    for (const auto& it : mLedsMap) {
        const LedContext& led = it.second;
        const LedPosition& pos = led.pos;
        const juce::Colour col = mEngine.getLedColor(it.first);
        const juce::Colour colW = mEngine.getLedWhite(it.first);

        float x0 = toX(pos.topLeft.getX());
        float y0 = toY(pos.topLeft.getY());
        float x1 = x0 + pos.size.getX() * scale;
        float y1 = y0 - pos.size.getY() * scale;

        const float ledWidth = static_cast<float>(led.width) * scale;
        const float bw = 2.0f * scale;
        const float bl = bw + 1.5f * scale;

        // Ombre
        g.setColour(juce::Colours::dimgrey);
        g.drawLine(x0 + bl, y0 + bl, x1 + bl, y1 + bl, ledWidth);
        g.drawLine(x0 - bl, y0 + bl, x1 - bl, y1 + bl, ledWidth);
        g.drawLine(x0 - bl, y0 - bl, x1 - bl, y1 - bl, ledWidth);
        g.drawLine(x0 + bl, y0 - bl, x1 + bl, y1 - bl, ledWidth);

        // Blanc
        g.setColour(colW);
        g.drawLine(x0 + bw, y0 + bw, x1 + bw, y1 + bw, ledWidth);
        g.drawLine(x0 - bw, y0 + bw, x1 - bw, y1 + bw, ledWidth);
        g.drawLine(x0 - bw, y0 - bw, x1 - bw, y1 - bw, ledWidth);
        g.drawLine(x0 + bw, y0 - bw, x1 + bw, y1 - bw, ledWidth);

        // Couleur principale
        g.setColour(col);
        g.drawLine(x0, y0, x1, y1, ledWidth);
    }
}

void UI_WorldView::paintLedNames(juce::Graphics& g) {
    if (mViewMode == ViewMode::Compact) return; // Pas de noms en mode compact

    auto displayArea = getDisplayArea();
    float scale = getScaleFactor();

    g.setFont(juce::FontOptions()
        .withName("Arial")
        .withPointHeight(15.0f * scale)
        .withStyle("Bold"));
    g.setColour(juce::Colours::white);

    auto toX = [&](int x) {
        return displayArea.getX() + static_cast<int>(x * scale);
        };
    auto toY = [&](int y) {
        return displayArea.getBottom() - static_cast<int>(y * scale);
        };

    for (const auto& it : mLedsMap) {
        const LedContext& led = it.second;
        const LedPosition& pos = led.pos;

        int wt = static_cast<int>(100 * scale);
        int ht = static_cast<int>(20 * scale);
        int xt = toX(pos.center.getX()) - wt / 3;
        int yt = toY(pos.center.getY()) - ht / 2;

        g.drawText(led.name, xt, yt, wt, ht, juce::Justification::centred);
    }
}

void UI_WorldView::resized() {
    // Forcer un repaint quand la taille change
    mModified = true;
    repaint();
}