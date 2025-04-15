#include "pch.h"
#include "ImPadding.h"

CImPadding::CImPadding():
	m_bThread(false),
	m_bTermination(false),
	m_uWorkType(eWT_Foreground),
	m_lpWork(nullptr)
{

}

CImPadding::~CImPadding()
{

}

void CImPadding::SetDebugDir(string strDir)
{
	m_strDebugDir = strDir;
}

void CImPadding::SetWorkType(uint8 uType)
{
	m_uWorkType = uType;
}

void CImPadding::SetThParam(CCondition* lpWork, function<void()> fWork)
{
	m_lpWork = lpWork; m_fWork = fWork;
}

bool CImPadding::Reg()
{
	if (!CIm2Mat::Reg()) return false;

	m_bTermination = false;

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)this, 0, NULL);

	CloseHandle(lpHandle);

	return (m_bThread = (!!lpHandle));
}

void CImPadding::Unreg()
{
	if (m_bThread)
	{
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)this, 0, NULL));

		m_cTermination.wait(); m_bThread = false;
	}
	CIm2Mat::Unreg();
}

unsigned int __stdcall CImPadding::OnWorkThread(void* lpUesrParam)
{
	CImPadding* lpThis = (CImPadding*)lpUesrParam;

	while (!lpThis->IsTermination()) lpThis->m_fWork();

	lpThis->m_cTermination.signal();

	return 0;
}

unsigned int __stdcall CImPadding::OnExitThread(void* lpUesrParam)
{
	CImPadding* lpThis = (CImPadding*)lpUesrParam;

	lpThis->Termination();

	lpThis->m_lpWork->signal();

	return 0;
}

void CImPadding::Termination()
{
	CSWLock lock(&m_lLock);

	m_bTermination = true;
}

bool CImPadding::IsTermination()
{
	CSRLock lock(&m_lLock);

	return m_bTermination;
}