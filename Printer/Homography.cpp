#include "pch.h"
#include "Homography.h"
#include "PrClock/PrClock.h"

CHomography::CHomography() :
	m_bTermination(false), m_bThread(false)
{
	m_vThParam = make_tuple(
		__FUNCTION__,
		this,
		&m_lLock,
		&m_cImFrame,
		&m_cTermination,
		&m_bTermination,
		&m_vParams,
		bind(&CHomography::OnImHomography, this, placeholders::_1));
}

CHomography::~CHomography()
{

}

bool CHomography::Reg()
{
	if (!CImageLine::Reg()) return false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	Restart(m_bTermination, m_lLock);

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle);

	return (m_bThread = (!!lpHandle));
}

void CHomography::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CImageLine::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CHomography::AllocateMmemory()
{
	CImageLine::AllocateMmemory();

}

void CHomography::DeallocateMemory()
{
	for (auto& iParam : m_vParams)
	{ 
		vector<vParam>* lpImInLine = (vector<vParam>*)get<ePI_UserParam>(iParam); 
		
		lpImInLine->clear(); 
		
		delete lpImInLine;
	}

	m_vParams.clear();

	CImageLine::DeallocateMemory();
}

void CHomography::ImHomography(vParam iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cImFrame.signal();
}

void CHomography::OnImHomography(vParam& iParam)
{
	vector<vParam>* lpImInLine = (vector<vParam>*)get<ePI_UserParam>(iParam);

	for (auto& iImParam : *lpImInLine)
	{
		auto& iId = get<ePI_FrameId>(iImParam);

		auto& iFrame = get<ePI_ImPtr>(iImParam);

		cv::rotate(*iFrame.Get(), *iFrame.Get(), ROTATE_90_COUNTERCLOCKWISE);

		copyMakeBorder(*iFrame.Get(), *iFrame.Get(), m_vFill[1], m_vFill[3], m_vFill[0], m_vFill[2], BORDER_CONSTANT, Scalar(0));

		warpPerspective(*iFrame.Get(), *iFrame.Get(), m_cHomography, Size2f(m_vImWH2[0], m_vImWH2[1]), 1, 0, Scalar(0));
	}
	get<ePI_Timer>(iParam).push_back({ __FUNCTION__, clock() });

	CImageLine::ImLine(iParam);
}
