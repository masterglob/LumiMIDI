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

#define TO_LINE_VALUE(x) static_cast<LineValue>(x)
#define MAX_CC_VALUE 127
#define MAX_CC_VALUE_F 127.0f

/**********************************************************************************/
struct LedCtrlLine {
    LedCtrlLine(LineId r, LineId g, LineId b, LineId w)
        : mr(r), mg(g), mb(b), mw(w) {
    }
    LedCtrlLine(LineId i0, LineId delta)
        : mr(i0), mg(mr + delta), mb(mg + delta), mw(mb + delta) {
    }
    LineId mr, mg, mb, mw;
    bool hasWhite()const { return mw > 0; }
};

/**********************************************************************************/
struct LedPosition {
    LedPosition(const Rect& r)
        : center{ r.getCentre() },
        topLeft(r.getTopLeft()),
        size(r.getWidth(), r.getHeight()) {
    }
    LedPosition(const Point& topLeft, const Point& size)
        : center{ (topLeft.getX() + size.getX()) / 2,
                 (topLeft.getY() + size.getY()) / 2 },
        topLeft(topLeft),
        size(size) {
    }
    Point center;
    Point topLeft;
    Point size;
    int getLength()const {
        return static_cast<int>(std::sqrt(size.getX() * size.getX() + size.getY() * size.getY()));
    }
};

/**********************************************************************************/
struct LedContext {
    LedContext(const juce::String& nameRef,
        const LedCtrlLine& lineRef,
        const LedPosition& ref)
        : name(nameRef), ctrl(lineRef), pos(ref) {
    }

    juce::String name;
    LedCtrlLine ctrl;
    LedPosition pos;
    int width{ 4 };
};

using LedVect = std::vector<LedContext*>;
using LedsMap = std::map<LedId, LedContext*>;

/**********************************************************************************/
class LedDB
{
public:
    LedDB(void);

    // ReadOnly feature
    // 
    // Editing feature:
    void cancelEditing(void);
    LedId addLed(const LedContext&);
    LedContext* getLed(const LedId);
    const LedContext* getLed(const LedId)const;
    LedsMap getAll(void);
    void doneEditing(void);

private:
    LedDB(const LedDB&) = delete;
    LedDB& operator=(const LedDB&) = delete;


    juce::CriticalSection mutex;

    LedsMap mLedsMap;
    LedsMap mLedsMapEdit;

    // Store all existing contexts to avoid invalid further reference
    std::deque<std::unique_ptr<LedContext>> mContexts;
};
