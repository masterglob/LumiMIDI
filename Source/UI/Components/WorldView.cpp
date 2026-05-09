
// ============================================================================
// UI/Components/WorldView.cpp
// ============================================================================

#include "WorldView.h"

#include "DSP/AudioEngine.h"
#include "DSP/BaseProgram.h"

namespace {
float distancePointToLineSegment(const juce::Point<float>& point,
                                 const juce::Point<float>& lineStart,
                                 const juce::Point<float>& lineEnd) {
  // Vector from line start to end
  float dx = lineEnd.getX() - lineStart.getX();
  float dy = lineEnd.getY() - lineStart.getY();

  // Handle degenerate case (line has zero length)
  float lineLength = std::sqrt(dx * dx + dy * dy);
  if (lineLength < 1e-6f) {
    // Line is essentially a point, return distance to that point
    return point.getDistanceFrom(lineStart);
  }

  // Vector from line start to point
  float px = point.getX() - lineStart.getX();
  float py = point.getY() - lineStart.getY();

  // Project point onto line (parameter t)
  float t = (px * dx + py * dy) / (lineLength * lineLength);

  // Clamp t to [0,1] to stay within line segment
  t = std::max(0.0f, std::min(1.0f, t));

  // Find closest point on line segment
  juce::Point<float> closestPoint(lineStart.getX() + t * dx,
                                  lineStart.getY() + t * dy);

  // Return distance from original point to closest point on line
  return point.getDistanceFrom(closestPoint);
}

// ============================================================
// Helpers
// ============================================================
// > 1.0 makes low intensity values more visible
// 2.2 is close to standard display gamma
constexpr float gamma = 2.2f;

// Minimum visible brightness in the UI
// Helps very low DMX values remain visible on screen
constexpr float minVisible = 0.015f;

// Additional boost applied to low intensity levels
// 0.0 disables the effect
constexpr float lowEndBoost = 0.20f;

// Highlight compression factor
// 1.0 = no compression
// > 1.0 prevents LEDs from appearing fully bright too early
constexpr float highlightCompression = 1.3f;

// Global UI brightness multiplier
constexpr float masterIntensity = 1.0f;

float adjustChannel(float v) {
  // Safety clamp
  v = juce::jlimit(0.0f, 1.0f, v);

  // --------------------------------------------------------
  // Low-end boost
  // --------------------------------------------------------

  // Slightly boosts low values without affecting highlights too much
  v += (1.0f - v) * lowEndBoost * v;

  // --------------------------------------------------------
  // Inverse gamma correction
  // --------------------------------------------------------

  // Makes low intensities much more visible on screen
  v = std::pow(v, 1.0f / gamma);

  // --------------------------------------------------------
  // Minimum visible brightness
  // --------------------------------------------------------

  // Prevents LEDs from becoming visually invisible at low levels
  if (v > 0.0f)
    v = juce::jmax(v, minVisible);

  // --------------------------------------------------------
  // Highlight compression
  // --------------------------------------------------------
  v = v / (v + highlightCompression * (1.0f - v));

  // --------------------------------------------------------
  // Global intensity gain
  // --------------------------------------------------------
  v *= masterIntensity;

  return juce::jlimit(0.0f, 1.0f, v);
};

/* Adjust graphical color to a realistic match of real LEDs*/
juce::Colour colorAdjust(juce::Colour dmxNormalized) {
  // ============================================================
  // RGB
  // ============================================================

  float r = adjustChannel(dmxNormalized.getFloatRed());
  float g = adjustChannel(dmxNormalized.getFloatGreen());
  float b = adjustChannel(dmxNormalized.getFloatBlue());

  return juce::Colour::fromFloatRGBA(r, g, b, dmxNormalized.getFloatAlpha());
}

}  // namespace
UI_WorldView::UI_WorldView(juce::AudioProcessorValueTreeState& apvts,
                           AudioEngine& engine)
    : mApvts(apvts), mEngine(engine) {
  startTimerHz(30);  // 30 FPS par d�faut
}

UI_WorldView::~UI_WorldView() {
  stopTimer();
}

void UI_WorldView::setViewMode(ViewMode mode) {
  if (mViewMode != mode) {
    mViewMode = mode;
    mModified = true;
    mZoomFactor = -1.0;
    repaint();
  }
}

void UI_WorldView::timerCallback() {
  // Mise � jour temps r�el - plus efficace que dans paint()
  refreshLeds();
  repaint();
}

LedContext* UI_WorldView::getLed(LedId ledId) {
  return mEngine.getLeds().getLed(ledId);
}

Point UI_WorldView::getPosAt(const Point& p) {
  // float scale = getScaleFactor();
  auto xt = getFromXTransform();
  auto yt = getFromYTransform();

  return Point(static_cast<int>(xt(p.getX())), static_cast<int>(yt(p.getY())));
}
Point UI_WorldView::getPosTo(const Point& p) {
  // float scale = getScaleFactor();
  auto xt = getToXTransform();
  auto yt = getToYTransform();

  return Point(static_cast<int>(xt(p.getX())), static_cast<int>(yt(p.getY())));
}

LedId UI_WorldView::getLedAt(const juce::Point<int>& p) {
  // Convert int point to float for precise comparison
  juce::Point<float> mousePos(static_cast<float>(p.x), static_cast<float>(p.y));

  float scale = getScaleFactor();

  // Cache les transformations pour �viter de les recr�er � chaque LED
  auto toXTransform = getToXTransform();
  auto toYTransform = getToYTransform();

  // Iterate through all LEDs and check distance to line
  const LedVectId m{mEngine.getLeds().getAll()};
  for (auto& it : m) {
    LedContext& led = *it.context;

    // Get LED line endpoints in world coordinates
    juce::Point<float> lineStart(toXTransform(led.pos.topLeft.getX()),
                                 toYTransform(led.pos.topLeft.getY()));

    juce::Point<float> lineEnd(
        toXTransform(led.pos.topLeft.getX() + led.pos.size.getX()),
        toYTransform(led.pos.topLeft.getY() + led.pos.size.getY()));

    // Optimisation : v�rification rapide avec bounding box �largie
    float ledWidth = static_cast<float>(led.width) * scale;
    float tolerance =
        std::max(ledWidth * 2.0f, 5.0f * scale);  // Tol�rance minimale

    // Bounding box de la ligne avec tol�rance pour early exit
    float minX = std::min(lineStart.getX(), lineEnd.getX()) - tolerance;
    float maxX = std::max(lineStart.getX(), lineEnd.getX()) + tolerance;
    float minY = std::min(lineStart.getY(), lineEnd.getY()) - tolerance;
    float maxY = std::max(lineStart.getY(), lineEnd.getY()) + tolerance;

    // Skip si le point n'est m�me pas dans la bounding box (optimisation
    // majeure)
    if (mousePos.getX() < minX || mousePos.getX() > maxX ||
        mousePos.getY() < minY || mousePos.getY() > maxY) {
      continue;
    }

    // Calculate distance from mouse to line segment (seulement si dans la
    // bounding box)
    float distance = distancePointToLineSegment(mousePos, lineStart, lineEnd);

    if (distance <= tolerance) {
      return it.id;  // First found
    }
  }

  return NO_LED;  // No LED found at this position
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

float UI_WorldView::getScaleFactor() {
  auto displayArea = getDisplayArea();
  if (mZoomFactor < 0.0f) {
    int xMin{FULL_WIDTH};
    int xMax{-FULL_WIDTH};
    int yMin{FULL_HEIGHT};
    int yMax{-FULL_HEIGHT};
    for (auto& it : mEngine.getLeds().getAll()) {
      LedContext& led = *it.context;
      int xL = led.pos.topLeft.getX();
      int xR = xL + led.pos.size.getX();
      if (xL < xMin)
        xMin = xL;
      if (xR > xMax)
        xMax = xR;
      int yT = led.pos.topLeft.getY();
      int yB = yT + led.pos.size.getY();
      if (yT < yMin)
        yMin = yT;
      if (yB > yMax)
        yMax = yB;
    }

    float scaleX = static_cast<float>(xMax) / displayArea.getWidth();
    float scaleY = static_cast<float>(yMax) / displayArea.getHeight();
    mZoomFactor = std::min(scaleX, scaleY);
  }
  return mZoomFactor;
}

void UI_WorldView::refreshLeds() {
  mModified = false;
}

void UI_WorldView::paint(juce::Graphics& g) {
  const LedVectId m{mEngine.getLeds().getAll()};
  paintBackground(g);
  paintLeds(g, m);

  if (mShowLedNames) {
    paintLedNames(g, m);
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
      juce::Colour(0xff0a0a0e), innerArea.getTopLeft().toFloat(),
      juce::Colour(0xff060110), innerArea.getBottomRight().toFloat(), false);

  g.reduceClipRegion(innerArea);
  g.setGradientFill(gradient);
  g.fillRect(innerArea);
}

std::function<float(int)> UI_WorldView::getFromXTransform() {
  auto displayArea = getDisplayArea();
  float scale = getScaleFactor();
  return [displayArea, scale](int x) {
    return (static_cast<float>(x) - displayArea.getX()) / scale;
  };
}

std::function<float(int)> UI_WorldView::getFromYTransform() {
  auto displayArea = getDisplayArea();
  float scale = getScaleFactor();
  return [displayArea, scale](int y) {
    return (displayArea.getBottom() - static_cast<float>(y)) / scale;
  };
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

void UI_WorldView::paintLeds(juce::Graphics& g, const LedVectId& m) {
  auto toX = getToXTransform();
  auto toY = getToYTransform();
  float scale = getScaleFactor();

  for (const auto& it : m) {
    const LedContext& led = *it.context;
    const LedPosition& pos = led.pos;
    const juce::Colour col = colorAdjust(mEngine.getLedColor(it.id));
    const juce::Colour colW = colorAdjust(mEngine.getLedWhite(it.id));

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

void UI_WorldView::paintLedNames(juce::Graphics& g, const LedVectId& m) {
  if (mViewMode == ViewMode::Compact)
    return;  // Pas de noms en mode compact

  auto displayArea = getDisplayArea();
  float scale = getScaleFactor();

  g.setFont(juce::FontOptions()
                .withName("Arial")
                .withPointHeight(25.0f * scale)
                .withStyle("Bold"));
  g.setColour(juce::Colours::white);

  auto toX = [&](int x) {
    return displayArea.getX() + static_cast<int>(x * scale);
  };
  auto toY = [&](int y) {
    return displayArea.getBottom() - static_cast<int>(y * scale);
  };

  for (const auto& it : m) {
    const LedContext& led = *it.context;
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
  mZoomFactor = -1.0;
  repaint();
}
