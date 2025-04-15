#include "pch.h"
#include "SRWLock.h"
#include <synchapi.h>

CSRWLockBase::CSRWLockBase()
{
	InitializeSRWLock(&m_srwl);
}

CSRWLockBase::~CSRWLockBase()
{

}

void CSRWLockBase::ReadLock()
{
	AcquireSRWLockShared(&m_srwl);
}

void CSRWLockBase::ReadUnlock()
{
	ReleaseSRWLockShared(&m_srwl);
}

void CSRWLockBase::WriteLock()
{
	AcquireSRWLockExclusive(&m_srwl);
}

void CSRWLockBase::WriteUnlock()
{
	ReleaseSRWLockExclusive(&m_srwl);
}

CSRLock::CSRLock(CSRWLockBase *pBase)
{
	m_pBase = pBase;

	m_pBase->ReadLock();
}

CSRLock::CSRLock(const CSRWLockBase *pBase)
{
	m_pBase = const_cast<CSRWLockBase *>(pBase);

	m_pBase->ReadLock();
}

CSRLock::~CSRLock()
{
	m_pBase->ReadUnlock();
}

CSWLock::CSWLock(CSRWLockBase *pBase)
{
	m_pBase = pBase;
	m_pBase->WriteLock();
}

CSWLock::CSWLock(const CSRWLockBase *pBase)
{
	m_pBase = const_cast<CSRWLockBase *>(pBase);
	m_pBase->WriteLock();
}

CSWLock::~CSWLock()
{
	m_pBase->WriteUnlock();
}