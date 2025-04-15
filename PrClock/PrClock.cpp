#include "pch.h"
#include "PrClock.h"

CPrClock::CPrClock(std::wstring strFunc)
	:m_nStart(0), m_strFunc(strFunc)
{
	QueryPerformanceFrequency(&m_tFreq);

	QueryPerformanceCounter(&m_tStart);
}

CPrClock::~CPrClock()
{
	LARGE_INTEGER tEnd;

	QueryPerformanceCounter(&tEnd);

	double dTime = (double)(tEnd.QuadPart - m_tStart.QuadPart) / m_tFreq.QuadPart /** 1000000.0*/;

	_cwprintf(L"%s: %0.2fs\r\n", m_strFunc.c_str(), dTime);
}