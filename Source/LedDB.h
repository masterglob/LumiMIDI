// =============================================================================
// LedDB.h
// =============================================================================
#pragma once
#include <atomic>

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

using LedId = unsigned char;
static constexpr LedId NO_LED(0xFF);

using LineId = unsigned short;
using LineValue = unsigned char;
using CCValue = unsigned char;
using Point = juce::Point<int>;
using Rect = juce::Rectangle<int>;

#define MIN_CC_VALUE static_cast<LineValue>(0)
#define MAX_CC_VALUE static_cast<LineValue>(127)
#define MAX_CC_VALUE_F 127.0f
#define FLOAT_TO_LINE_VALUE(x) \
  static_cast<LineValue>(MAX_CC_VALUE_F * juce::jlimit(0.0f, 0.99f, (x)))
#define TO_LINE_VALUE(x) static_cast<LineValue>(x)

/**********************************************************************************/
struct LedCtrlLine {
  LedCtrlLine(LineId r, LineId g, LineId b, LineId w)
      : mr(r), mg(g), mb(b), mw(w), hasWhite(true) {}
  LedCtrlLine(LineId r, LineId g, LineId b)
      : mr(r), mg(g), mb(b), mw(0), hasWhite(false) {}
  LedCtrlLine(LineId i0, LineId delta)
      : mr(i0),
        mg(mr + delta),
        mb(mg + delta),
        mw(mb + delta),
        hasWhite(true) {}
  LedCtrlLine() = default;
  LineId mr{0}, mg{0}, mb{0}, mw{0};
  bool hasWhite{false};
};

/**********************************************************************************/
struct LedPosition {
  LedPosition(const Rect& r)
      : center{r.getCentre()},
        topLeft(r.getTopLeft()),
        size(r.getWidth(), r.getHeight()) {}
  LedPosition(const Point& topLeftP, const Point& sizeP)
      : center{topLeftP + sizeP / 2}, topLeft(topLeftP), size(sizeP) {}
  LedPosition() = default;

  Point center;
  Point topLeft;
  Point size;
  int getLength() const {
    return static_cast<int>(
        std::sqrt(size.getX() * size.getX() + size.getY() * size.getY()));
  }
  void move(const Point& p) {
    topLeft = topLeft + p;
    center = center + p;
  }
  void set(const Point& p) {
    topLeft = p;
    center = topLeft + size / 2;
  }
};

/**********************************************************************************/
struct LedContext {
  LedContext(const juce::String& nameRef,
             const LedCtrlLine& lineRef,
             const LedPosition& ref)
      : name(nameRef), ctrl(lineRef), pos(ref) {}
  LedContext() = default;

  juce::String name;
  LedCtrlLine ctrl;
  LedPosition pos;
  int width{4};
};

/**********************************************************************************/
struct LedContextId {
  LedContextId(LedId lid, LedContext* pCtxt) : id(lid), context(pCtxt) {}
  LedId id;
  LedContext* context;
};
using LedVect = std::vector<LedContext*>;
using LedsMap = std::map<LedId, LedContext*>;
using LedVectId = std::vector<LedContextId>;

/**********************************************************************************/
class LedDB {
 public:
  LedDB(void);

  // ReadOnly feature
  //
  // Editing feature:
  void cancelEditing(void);
  LedId addLed(const LedContext&);
  void removeLed(const LedId ledId);
  LedContext* getLed(const LedId);
  const LedContext* getLed(const LedId) const;
  LedVectId getAll(void);
  void doneEditing(void);

 private:
  LedDB(const LedDB&) = delete;
  LedDB& operator=(const LedDB&) = delete;

  mutable juce::CriticalSection mutex;

  LedsMap mLedsMap;
  LedsMap mLedsMapEdit;

  // Store all existing contexts to avoid invalid further reference
  std::vector<std::unique_ptr<LedContext>> mContexts;
};
