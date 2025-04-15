#pragma once
typedef void *HANDLE;

class CCondition
{
public:
	CCondition();
	~CCondition();

public:
	bool wait(DWORD dwMilliseconds = INFINITE);
	void signal();

private:
	HANDLE m_hEvent;
};
