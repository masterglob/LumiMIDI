// ============================================================================
// Source/DSP/BaseProgram.h
// ============================================================================
#pragma once

#include <list>
#include <map>
#include <memory>
#include <vector>

#include <juce_graphics/juce_graphics.h>

class ParameterManager;

using LedId = unsigned char;
using LineId = unsigned short;
using LineValue = unsigned char;
using CCValue = unsigned char;
using Point = juce::Point<int>;
using Rect = juce::Rectangle<int>;

#define TO_LINE_VALUE(x) static_cast<LineValue>(x)
#define MAX_CC_VALUE 127
#define MAX_CC_VALUE_F 127.0f

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
struct ProgramContext {};
/**********************************************************************************/
class BaseProgram {
 public:
  BaseProgram(const std::string& name);
  virtual ~BaseProgram(void) = default;

  struct Event {
    Event(LineId alineIdx, LineValue avalue)
        : lineIdx(alineIdx), value(avalue) {}
    LineId lineIdx;
    LineValue value;
  };
  using Events = std::vector<Event>;

  void reset(const CCValue velocity);

  virtual void execute(const LedVect& leds,
                       const ParameterManager& parameterManager,
                       Events&) = 0;
  virtual bool done(void) const { return mDone; }

  const std::string name;

 protected:
  virtual void reset(void) {};
  static juce::uint32 floatToPeriod(float f); /* Input Range : [0..1] */
  LineValue floatToCcValue(float f);          /* Input Range : [0..127] */

  std::unique_ptr<ProgramContext> mContext{nullptr};
  juce::uint32 elapsedMs(void) const;
  bool mDone{false};
  CCValue mVelocity{0};

 private:
  BaseProgram(void) = delete;
  juce::uint32 startMillis{0};
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseProgram)
};

/**********************************************************************************/
#define DECLARE_PROGRAM_CLASS(ClassName)                    \
  class ClassName : public BaseProgram {                    \
   public:                                                  \
    ClassName();                                            \
    void execute(const LedVect& leds,                       \
                 const ParameterManager& parameterManager,  \
                 BaseProgram::Events& events);              \
                                                            \
   private:                                                 \
    void reset(void) override;                              \
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassName) \
  }

namespace PROGS {
DECLARE_PROGRAM_CLASS(DefaultProgram);
DECLARE_PROGRAM_CLASS(SimpleStroboscope);
DECLARE_PROGRAM_CLASS(SimpleWave);
DECLARE_PROGRAM_CLASS(RandomSparkle);

}  // namespace PROGS
