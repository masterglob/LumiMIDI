// ============================================================================
// Source/DSP/BaseProgram.h
// ============================================================================
#pragma once

#include <list>
#include <vector>

class ParameterManager;

using LineId = unsigned short;
using LineValue = unsigned char;

#define TO_LINE_VALUE(x) static_cast<LineValue>(x)

/**********************************************************************************/
class Led_RGBW
{
public:
    Led_RGBW(LineId r, LineId g, LineId b, LineId w) : mr(r), mg(g), mb(b), mw(w) {}
    Led_RGBW(LineId i0, LineId delta) : mr(i0), mg(mr + delta), mb(mg + delta), mw(mb + delta) {}
    const LineId mr, mg, mb, mw;
};
using LedVect = std::vector<const Led_RGBW*>;


/**********************************************************************************/
class BaseProgram
{
public:
    BaseProgram(void) = default;
    virtual ~BaseProgram(void) = default;

    struct Event
    {
        Event(LineId alineIdx, LineValue avalue) :lineIdx(alineIdx), value(avalue) {}
        LineId lineIdx;
        LineValue value;
    };
    using Events = std::vector<Event>;

    virtual Events execute(const LedVect& leds, const ParameterManager& parameterManager) = 0;
    virtual bool done(void)const { return false; }

protected:

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseProgram)
};

/**********************************************************************************/
class DefaultProgram : public BaseProgram
{
public:
    static DefaultProgram& instance(void);

private:
    DefaultProgram(void) = default;
    ~DefaultProgram(void) override = default;
    Events execute(const LedVect& leds, const ParameterManager& parameterManager) override;
};