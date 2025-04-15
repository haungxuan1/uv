#pragma once
#include <Windows.h> // 头文件包含顺序， 为解决SRWLOCK 未知重写说明符
#include <vector>
using namespace std;

class CSRLock;
class CSWLock;

class CSRWLockBase
{
	friend class CSRLock;
	friend class CSWLock;
public:
	CSRWLockBase();
	virtual ~CSRWLockBase();

private:
	void ReadLock();
	void ReadUnlock();
	void WriteLock();
	void WriteUnlock();

private:
	SRWLOCK m_srwl;
};

class CSRLock
{
public:
	CSRLock(CSRWLockBase *pBase);
	CSRLock(const CSRWLockBase *pBase);
	~CSRLock();

private:
	CSRWLockBase *m_pBase;
};

class CSWLock
{
public:
	CSWLock(CSRWLockBase *pBase);
	CSWLock(const CSRWLockBase *pBase);
	~CSWLock();

private:
	CSRWLockBase *m_pBase;
};