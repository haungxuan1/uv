#include "pch.h"
#include "Condition.h"
#include <winnt.h>

CCondition::CCondition()
{
	m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

CCondition::~CCondition()
{
	if (NULL != m_hEvent)
	{
		::CloseHandle((m_hEvent));
	}
}

bool CCondition::wait(DWORD dwMilliseconds)
{
	DWORD dwRes = WaitForSingleObject(m_hEvent, dwMilliseconds);

	ResetEvent(m_hEvent);

	return (dwRes == WAIT_OBJECT_0);
}

void CCondition::signal()
{
	SetEvent(m_hEvent);
}
