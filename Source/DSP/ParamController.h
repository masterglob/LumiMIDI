// ============================================================================
// Source/DSP/BaseProgram.h
// ============================================================================
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_graphics/juce_graphics.h>

#include <algorithm>
#include <map>

/**********************************************************************************/
class ParamManager {
 public:
  ParamManager();

  using CcChangeEvent = std::function<void(int ccVal)>;

  void addParam(int ccNum, CcChangeEvent handler);
  void applyCc(int ccNum, int ccVal);

 protected:
 private:
  // A map CC => Controller
  using CcMap = std::map<int, CcChangeEvent>;
  CcMap mCcMap;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParamManager)
};
