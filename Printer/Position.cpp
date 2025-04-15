#include "pch.h"
#include "def.h"
#include "Position.h"
#include "PQSetting.h"
#include "PrClock/PrClock.h"
#include "Helper.h"
//#include "MaskFastMatch.h"
#include "PQMsg.h"

#ifndef USE_HALCON
#include "FastMatch.h"
#else
#include "FastHalMatch.h"
#endif


CPosition::CPosition() :
	m_bThread(false), m_bTermination(false)
{
	m_vThParam = make_tuple(
		__FUNCTION__,
		this,
		&m_lLock,
		&m_cImView,
		&m_cTermination,
		&m_bTermination,
		&m_vParams,
		bind(&CPosition::OnPosition, this, placeholders::_1));
}

CPosition::~CPosition()
{

}

bool CPosition::Reg()
{
	if (!CPrtMatrix::Reg()) return false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	Restart(m_bTermination, m_lLock);

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle);

	return (m_bThread = (!!lpHandle));
}

void CPosition::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CPrtMatrix::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CPosition::AllocateMmemory()
{
	CPrtMatrix::AllocateMmemory();

	m_lpMold = TIFFOpen(m_strTIFF.c_str(), "rb");
}

void CPosition::DeallocateMemory()
{
	m_lpMold ? TIFFClose(m_lpMold) : []() {}(); m_lpMold = nullptr;

	for (auto& iParam : m_vParams) 
	{
		array<int64, 3>* lpMosaicParam = (array<int64, 3>*)get<ePI_UserParam>(iParam); 
		
		delete lpMosaicParam;
	}

	m_vParams.clear();

	CPrtMatrix::DeallocateMemory();
}

void CPosition::Position(vParam iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cImView.signal();
}

void CPosition::OnPosition(vParam iParam)
{
	auto& iFrameId = get<ePI_FrameId>(iParam);

	auto& iFleece = get<ePI_Fleece>(iParam);

	auto& iTime = get<ePI_Timer>(iParam);

	array<int64, 3>* lpMosaicParam = (array<int64, 3>*)get<ePI_UserParam>(iParam);

	auto& iTarget = get<ePI_ImPtr>(iParam);
	vector<tuple<Mat, Rect, double, double>> vMatch;

//	MatchAndCreatPrintMat(8, 2, *iTarget.Get(), m_cTemplate, m_lpMold, vMatch);

//	m_cMask = Mat();
	if(m_cMask.empty()) m_cMask = Mat();

	m_iTargetImage = (*iTarget.Get()).clone();

	//int ret = MatchAndCreatPrintMat(8,2, *iTarget.Get(), m_cTemplate, m_cMask, m_lpMold, vMatch);
#ifndef USE_HALCON
	bool bRet = pq_match(vMatch, *iTarget.Get(), m_cTemplate, m_cMask, m_lpMold);
#else
	bool bRet = PQ_MatchAndFill(vMatch, *iTarget.Get());
#endif

	_cwprintf(L"%s : pq_match ret=%d, count=%d\r\n", __FUNCTIONW__, bRet, vMatch.size());

	iTime.push_back({ __FUNCTION__, clock() });

	tuple<int, bool, vTimer, array<int64, 3>, ImPtr::CImPtr, vMatcher>* lpParam =
		new tuple<int, bool, vTimer, array<int64, 3>, ImPtr::CImPtr, vMatcher>
	{ iFrameId, iFleece, iTime, *lpMosaicParam, iTarget, vMatch };

	::PostMessage(m_lpInitiator->GetSafeHwnd(), PQM_MATCH_MATRIX, 0, (LPARAM)lpParam);
	::PostMessage(m_lpInitiator->GetSafeHwnd(), PQM_MATCH_BACKGROUND, 0, (LPARAM)&m_iTargetImage);
	
	delete lpMosaicParam;
}
