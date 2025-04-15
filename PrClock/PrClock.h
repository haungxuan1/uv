#pragma once
#include <string>

class CPrClock
{
public:
	CPrClock(std::wstring strFunc);
	~CPrClock();

private:
	clock_t m_nStart;
	std::wstring m_strFunc;
	LARGE_INTEGER m_tFreq;
	LARGE_INTEGER m_tStart;
};