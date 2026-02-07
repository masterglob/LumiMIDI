// ============================================================================
// Source/DSP/BaseProgram.cpp
// ============================================================================

#include "DSP/ParamController.h"

/**********************************************************************************/
ParamManager::ParamManager() {
}

/**********************************************************************************/
void ParamManager::applyCc(int ccNum, int ccVal) {
  auto it = mCcMap.find(ccNum);
  if (it != mCcMap.end() && it->second) {
    it->second(ccVal);
  }
}

/**********************************************************************************/
void ParamManager::addParam(int ccNum, ParamManager::CcChangeEvent cc) {
  mCcMap.emplace(ccNum, std::move(cc));
}
