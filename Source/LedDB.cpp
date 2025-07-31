// =============================================================================
// LedDB.cpp
// =============================================================================

#include "LedDB.h"

using ConstLedVect = std::vector<const LedContext*>;
namespace {
	const LedCtrlLine Led_Line_G(9, 2);
	const LedCtrlLine Led_Line_M(17, 2);
	const LedCtrlLine Led_Line_Sh1(25, 2);
	const LedCtrlLine Led_Line_Sh2(33, 2);
	const LedCtrlLine Led_Line_SQ(41, 2);
	const LedCtrlLine Led_Line_Gs(105, 2);
	const LedCtrlLine Led_Line_Ms(113, 2);
	const LedCtrlLine Led_Line_Sh1s(97, 2);
	const LedCtrlLine Led_Line_Sh2s(89, 2);
	const LedCtrlLine Led_Line_SQs(81, 2);

	static const int LedWDemo(550);
	const LedContext Led_G{ "G", Led_Line_G, Rect(100, 100, 100, -50) };
	const LedContext Led_M{ "M", Led_Line_M, Rect(100, 150, 150, 100) };
	const LedContext Led_Sh1{ "Sh1", Led_Line_Sh1, Rect(100, 120, 0, 130) };
	const LedContext Led_Sh2{ "Sh2", Led_Line_Sh2, Rect(200, 70, 0, 130) };
	const LedContext Led_SQ{ "SQ", Led_Line_SQ, Rect(280, 200, 0, 200) };

	const LedContext Led_Gs{ "Gs", Led_Line_Gs,
							Rect(LedWDemo - 100, 100, -100, -50) };
	const LedContext Led_Ms{ "Ms", Led_Line_Ms,
							Rect(LedWDemo - 100, 150, -150, 100) };
	const LedContext Led_Sh1s{ "Sh1s", Led_Line_Sh1s,
							  Rect(LedWDemo - 100, 120, 0, 130) };
	const LedContext Led_Sh2s{ "Sh2s", Led_Line_Sh2s,
							  Rect(LedWDemo - 200, 70, 0, 130) };
	const LedContext Led_SQs{ "SQs", Led_Line_SQs,
							 Rect(LedWDemo - 280, 200, 0, 200) };

	const ConstLedVect demoLeds{
		&Led_G,  &Led_M,  &Led_Sh1,  &Led_Sh2,  &Led_SQ,   // Side 1
		&Led_Gs, &Led_Ms, &Led_Sh1s, &Led_Sh2s, &Led_SQs,  // Side 2
	};

}

/*************************************************/
LedDB::LedDB(void)
{
	for (const LedContext* it : demoLeds) {
		addLed(*it);
	}
	doneEditing();
}


/*************************************************/
void LedDB::cancelEditing(void)
{
	juce::ScopedLock lock(mutex);
	mLedsMapEdit = mLedsMap;
}

/*************************************************/
LedId LedDB::addLed(const LedContext& ctxt)
{
	// working on non active element does not require mutex lock
	// as long as "addLed" is not called simult. with doneEditing
	for (LedId id(0); id < 255; id++)
	{
		if (mLedsMapEdit.find(id) == mLedsMapEdit.end())
		{
			juce::ScopedLock lock(mutex);
			mContexts.emplace_back(new LedContext(ctxt));
			mLedsMapEdit[id] = mContexts.back().get();
			return id;
		}
	}
	return NO_LED;
}

/*************************************************/
void LedDB::removeLed(const LedId ledId)
{
	if (ledId != NO_LED)
	{
		juce::ScopedLock lock(mutex);
		mLedsMapEdit.erase(ledId);
	}
}

/*************************************************/
LedContext* LedDB::getLed(const LedId ledId)
{
	auto it(mLedsMapEdit.find(ledId));

	if (it == mLedsMapEdit.end()) return nullptr;
	return it->second;

}
/*************************************************/
const LedContext* LedDB::getLed(const LedId ledId)const
{
	auto it(mLedsMapEdit.find(ledId));

	if (it == mLedsMapEdit.end()) return nullptr;
	return it->second;

}

/*************************************************/
LedVectId LedDB::getAll(void)
{
	LedVectId result;
	juce::ScopedLock lock(mutex);
	result.reserve(mLedsMap.size());
	{
		for (const auto& it : mLedsMap)
		{
			result.emplace_back(it.first, it.second);
		}
	}
	return std::move(result);
}

/*************************************************/
void LedDB::doneEditing(void)
{
	juce::ScopedLock lock(mutex);
	mLedsMap = mLedsMapEdit;
}


