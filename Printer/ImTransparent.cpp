#include "pch.h"
#include "ImTransparent.h"
#include "PrClock/PrClock.h"
#include "Helper.h"
#include "PQMsg.h"

CImTransparent::CImTransparent() :
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
		bind(&CImTransparent::OnImTransparent, this, placeholders::_1));
}

CImTransparent::~CImTransparent()
{

}

bool CImTransparent::Reg()
{
	if (!CPosition::Reg()) return false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	Restart(m_bTermination, m_lLock);

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle);

	return (m_bThread = (!!lpHandle));
}

void CImTransparent::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CPosition::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CImTransparent::AllocateMmemory()
{
	CPosition::AllocateMmemory();

}

void CImTransparent::DeallocateMemory()
{
	for (auto& iParam : m_vParams)
	{ 
		array<int64, 3>* lpMosaicParam = (array<int64, 3>*)get<ePI_UserParam>(iParam); 
		
		delete lpMosaicParam;
	}

	m_vParams.clear();

	CPosition::DeallocateMemory();
}

void CImTransparent::ImTransparent(vParam iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cImFrame.signal();
}

void CImTransparent::OnImTransparent(vParam& iParam)
{
	auto& iFleece = get<ePI_Fleece>(iParam);

	auto& iFrameId = get<ePI_FrameId>(iParam);

	auto& iTimer = get<ePI_Timer>(iParam);

	array<int64,3>* lpMosaicParam = (array<int64, 3>*)get<ePI_UserParam>(iParam); // 是否完成 | 上移 | 本次发出

	auto& iSight = get<ePI_ImPtr>(iParam);

	GaussianBlur(*iSight.Get(), m_cTransparent, Size(15, 15), 1.5, 0);

//	threshold(m_cTransparent, m_cTransparent, m_nTransparentThreshold, 255, THRESH_BINARY);
	threshold(m_cTransparent, m_cTransparent, m_nTransparentThreshold, 255, THRESH_BINARY_INV); // 白色车贴纸 - 黑色校准格

	Mat kErode = Mat::ones(5, 5, CV_8UC1);

	Mat kDilate = Mat::ones(5, 5, CV_8UC1);

	erode(m_cTransparent, m_cTransparent, kErode);

	dilate(m_cTransparent, m_cTransparent, kDilate);

	if (!!(*lpMosaicParam)[2])
	{
		ImPtr::CImPtr cImTr(new Mat((*lpMosaicParam)[2], m_cTransparent.cols, CV_8UC1));// 是否完成 | 上移 | 本次发出

		m_cTransparent(Rect(0, 0, cImTr.Get()->cols, cImTr.Get()->rows)).copyTo(*cImTr.Get());

		ImPtr::CImPtr cImMsg(new Mat);

		resize(*cImTr.Get(), *cImMsg.Get(), Size2d(0.1* cImTr.Get()->cols,0.1* cImTr.Get()->rows));

		::PostMessage(m_lpInitiator->GetSafeHwnd(), PQM_MATCH_TRANSPARENT, 0, (LPARAM)new ImPtr::CImPtr(cImMsg));

		ImPtr::CImPtr cImCMYK(new Mat);
																		
		Gray2Cmyk(*cImTr.Get(), *cImCMYK.Get(), vector<int>{ 2 }); // 0 = c| 1 = m | 2 = y | 3 = k

		iTimer.push_back({ __FUNCTION__, clock() });

		CPrinterDPIAlignment::DPIAlignment({ iFrameId, iFleece, iTimer, new bool((*lpMosaicParam)[0]), cImCMYK });
	}
	delete lpMosaicParam;
}

