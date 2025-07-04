// ============================================================================
// Source/DSP/BaseProgram.h
// ============================================================================
#pragma once

#include <list>
#include <map>
#include <vector>

#include <juce_graphics/juce_graphics.h>

class ParameterManager;

using LedId = unsigned char;
using LineId = unsigned short;
using LineValue = unsigned char;
using Point = juce::Point<int>;
using Rect = juce::Rectangle<int>;

#define TO_LINE_VALUE(x) static_cast<LineValue>(x)

/**********************************************************************************/
struct LedCtrlLine {
  LedCtrlLine(LineId r, LineId g, LineId b, LineId w)
      : mr(r), mg(g), mb(b), mw(w) {}
  LedCtrlLine(LineId i0, LineId delta)
      : mr(i0), mg(mr + delta), mb(mg + delta), mw(mb + delta) {}
  const LineId mr, mg, mb, mw;
};

/**********************************************************************************/
struct LedPosition {
  LedPosition(const Rect& r)
      : center{r.getCentre()},
        topLeft(r.getTopLeft()),
        size(r.getWidth(), r.getHeight()) {}
  LedPosition(const Point& topLeft, const Point& size)
      : center{(topLeft.getX() + size.getX()) / 2,
               (topLeft.getY() + size.getY()) / 2},
        topLeft(topLeft),
        size(size) {}
  Point center;
  Point topLeft;
  Point size;
};

/**********************************************************************************/
struct LedContext {
  LedContext(const std::string& nameRef,
             const LedCtrlLine& lineRef,
             const LedPosition& ref)
      : name(nameRef), ctrl(lineRef), pos(ref) {}

  std::string name;
  LedCtrlLine ctrl;
  LedPosition pos;
  int width{4};
};

using LedVect = std::vector<const LedContext*>;

/**********************************************************************************/
class BaseProgram {
 public:
  BaseProgram(void) = default;
  virtual ~BaseProgram(void) = default;

  struct Event {
    Event(LineId alineIdx, LineValue avalue)
        : lineIdx(alineIdx), value(avalue) {}
    LineId lineIdx;
    LineValue value;
  };
  using Events = std::vector<Event>;

  virtual void execute(const LedVect& leds,
                       const ParameterManager& parameterManager,
                       Events&) = 0;
  virtual bool done(void) const { return false; }

 protected:
 private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseProgram)
};

/**********************************************************************************/
class DefaultProgram : public BaseProgram {
 public:
  static DefaultProgram& instance(void);

 private:
  DefaultProgram(void) = default;
  ~DefaultProgram(void) override = default;
  void execute(const LedVect& leds,
               const ParameterManager& parameterManager,
               Events& events) override;
};
