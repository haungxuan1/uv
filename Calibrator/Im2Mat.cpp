#include "pch.h"
#include "Im2Mat.h"
#include <io.h>

CIm2Mat::CIm2Mat():
	m_bThread(false),
	m_bTermination(false)
{

}

CIm2Mat::~CIm2Mat()
{

}

bool CIm2Mat::Reg()
{
	m_bTermination = false;

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnReadThread, (void*)this, 0, NULL);

	CloseHandle(lpHandle);

	return (m_bThread = (!!lpHandle));
}

void CIm2Mat::Unreg()
{
	if (m_bThread)
	{
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)this, 0, NULL));

		m_cTermination.wait(); m_bThread = false; Clear();
	}
}

void CIm2Mat::SetFiles(vector<vector<array<string,2>>>& vFile)
{
	m_vFile = vFile;
}

void CIm2Mat::SetSignal(function<void(lpIm2Mat&)> fSignal, function<bool()> fIsIdle)
{
	m_fIsIdle = fIsIdle;

	m_fSignal = fSignal;
}

void CIm2Mat::Termination()
{
	CSWLock lock(&m_lLock);

	m_bTermination = true;
}

bool CIm2Mat::IsTermination()
{
	CSRLock lock(&m_lLock);

	return m_bTermination;
}

unsigned int __stdcall CIm2Mat::OnExitThread(void* lpUesrParam)
{
	CIm2Mat* lpThis = (CIm2Mat*)lpUesrParam;

	lpThis->Termination();

	return 0;
}

unsigned int __stdcall CIm2Mat::OnReadThread(void* lpUesrParam)
{
	CIm2Mat* lpThis = (CIm2Mat*)lpUesrParam;

	int nRemaining = 0;	vector<array<string, 2>> vFile;

	while (!lpThis->IsTermination())
	{
		if (!lpThis->m_fIsIdle()) { Sleep(1000 * 3); continue; }

		if (!lpThis->PopFile(vFile, nRemaining)) { Sleep(300); continue; }

		lpIm2Mat lpCache = new tagIm2Mat();

		lpThis->Im2Mat(vFile, *lpCache);

		lpCache->nRemaining = nRemaining;

		lpThis->m_fSignal(lpCache);
	}

	lpThis->m_cTermination.signal();

	return 0;
}

void CIm2Mat::Clear()
{
	m_vFile.clear();
}

bool CIm2Mat::PopFile(vector<array<string,2>>& vFile, int& nRemaining)
{
	if (m_vFile.empty()) return false;

	auto iFront = m_vFile.begin();

	vFile = (*iFront);

	iFront = m_vFile.erase(iFront);

	nRemaining = m_vFile.size();

	return true;
}

void CIm2Mat::Im2Mat(vector<array<string, 2>>& vFile, tagIm2Mat& vIm2Mat)
{
	for (auto& iFile : vFile)
	{
		if (_access(iFile[1].c_str(), 0) != 0) continue;

		Mat* lpMat = new Mat;

		*lpMat = imread(iFile[1].c_str(), IMREAD_GRAYSCALE);

		vIm2Mat.vIm2Mat.push_back(lpMat);
	}
	vIm2Mat.strFirst = vFile[0][0];
}