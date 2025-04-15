#include "pch.h"
#include "PrtMatrix.h"
#include "PrClock/PrClock.h"
#include "PQMsg.h"

CPrtMatrix::CPrtMatrix() :
	m_bThread(false), m_bTermination(false)
{
	m_vThParam = make_tuple(
		__FUNCTION__,
		this,
		&m_lLock,
		&m_cPosition,
		&m_cTermination,
		&m_bTermination,
		&m_vParams,
		bind(&CPrtMatrix::OnPrtMatrix, this, placeholders::_1));
}

CPrtMatrix::~CPrtMatrix()
{

}

bool CPrtMatrix::Reg()
{
	if (!CPrinterDPIAlignment::Reg()) return false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	Restart(m_bTermination, m_lLock);

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle);



	return (m_bThread = (!!lpHandle));
}

void CPrtMatrix::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CPrinterDPIAlignment::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CPrtMatrix::AllocateMmemory()
{
	CPrinterDPIAlignment::AllocateMmemory();

	m_cSight = Mat(Mat::zeros(m_vSight[2], m_vSight[1], CV_8UC(m_nTIFFChannel)));

	m_cCache = Mat(Mat::zeros(m_cSight.rows, m_cSight.cols, CV_8UC(m_nTIFFChannel)));
}

void CPrtMatrix::DeallocateMemory()
{
	m_vTarget.clear(); m_cSight = Mat(); m_cCache = Mat();

	for (auto& iParam : m_vParams)
	{
		tuple<UserParam, vTarget>* lpMatch = (tuple<UserParam, vTarget>*)get<ePI_UserParam>(iParam); 
		
		delete lpMatch;
	}

	m_vParams.clear();

	CPrinterDPIAlignment::DeallocateMemory();
}

void CPrtMatrix::PrtMatrix(vParam iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cPosition.signal();
}

void CPrtMatrix::OnPrtMatrix(vParam& iParam)
{
	tuple<UserParam, vTarget>* lpMatch = (tuple<UserParam, vTarget>*)get<ePI_UserParam>(iParam);

	auto& iUserParam = get<0>(*lpMatch); auto& iTarget = get<1>(*lpMatch);

	auto& iFrameId = get<ePI_FrameId>(iUserParam);

	auto& iFleece = get<ePI_Fleece>(iParam);

	auto& iTimer = get<ePI_Timer>(iUserParam);

	auto& iMosaicParam = get<ePI_UserParam>(iUserParam);

	if (iMosaicParam[1]) OnMove(iMosaicParam[1]);// 上移

	if (!iTarget.empty()) OnCopyTemp(iTarget);// 贴图

	if (iMosaicParam[2]) SendMatrix(iFrameId, iFleece, iTimer, !!iMosaicParam[0], iMosaicParam[2]); // 发送

	delete lpMatch;
}

void CPrtMatrix::SendMatrix(int& nFrameId, bool& bFleece, vector<tuple<string, int>>& vTimer, bool bFinish, int64& nCy)
{
	ImPtr::CImPtr cSend(new Mat(nCy, m_cSight.cols, CV_8UC(m_nTIFFChannel)));

	m_cSight(Rect(0, 0, cSend.Get()->cols, cSend.Get()->rows)).copyTo(*cSend.Get());

	vTimer.push_back({ __FUNCTION__, clock() });

	CPrinterDPIAlignment::DPIAlignment({ nFrameId, bFleece, vTimer, new bool(bFinish), cSend });
}

void CPrtMatrix::OnMove(int64& nOy)
{
	OnImMove(nOy);

	OnTeMove(nOy);
}

void CPrtMatrix::OnImMove(int64& nOy)
{
	if (!!!nOy) return;

	Rect rtBack, rtFront, rtEmpty;

	rtFront.x = 0, rtFront.width = m_cSight.cols;

	rtFront.y = 0, rtFront.height = m_cSight.rows - nOy;

	rtBack.x = 0, rtBack.width = rtFront.width;

	rtBack.y = nOy, rtBack.height = rtFront.height;

	rtEmpty.x = 0, rtEmpty.width = rtFront.width;

	rtEmpty.y = rtFront.height; rtEmpty.height = nOy;

	Mat cEmpty(Mat::zeros(rtEmpty.height, rtEmpty.width, CV_8UC(m_nTIFFChannel)));

	auto iBack = m_cSight(rtBack);

	auto iFront = m_cSight(rtFront);

	auto iEmpty = m_cSight(rtEmpty);

	iBack.copyTo(m_cCache(rtFront));

	m_cCache(rtFront).copyTo(iFront);

	cEmpty.copyTo(iEmpty);
}

void CPrtMatrix::OnTeMove(int64& nOy)
{
	if (m_vTarget.empty()) return;

	for (auto iTarg = m_vTarget.begin(); iTarg != m_vTarget.end();)
	{
		iTarg->y -= nOy;

		if (0 <= iTarg->y) { ++iTarg; continue; }

		iTarg = m_vTarget.erase(iTarg);
	}
}

void CPrtMatrix::OnCopyTemp(vector<tuple<bool, Rect, ImPtr::CImPtr>>& vTarg)
{
	for (auto& iTarg : vTarg) OnCopyTemp(iTarg);
}

void CPrtMatrix::OnCopyTemp(tuple<bool, Rect, ImPtr::CImPtr>& vTarg)
{
	auto& iEnabel = get<0>(vTarg);

	auto& iPos = get<1>(vTarg);

	auto& iImPtr = get<2>(vTarg);

	if (!iEnabel || iPos.empty() || IsUnite(iPos)) return;

	m_vTarget.push_back(iPos);

	for (uint64 y = 0; y < iPos.height; ++y)
	{
		for (uint64 x = 0; x < iPos.width; ++x)
		{
			uint64 idx1 = (y * iPos.width + x) * m_nTIFFChannel;

			uint64 idx2 = ((y + iPos.y) * m_cSight.cols + (x + iPos.x)) * m_nTIFFChannel;

			for (uint64 c = 0; c < m_nTIFFChannel; ++c) // cmyk
			{
				m_cSight.data[idx2 + c] = (iImPtr.Get()->data[idx1 + c] ? iImPtr.Get()->data[idx1 + c] : m_cSight.data[idx2 + c]);
			}
		}
	}
}

bool CPrtMatrix::IsUnite(Rect& rtTarg)
{
	Rect rtCache[2];

	for (auto& iTarg : m_vTarget)
	{
		rtCache[0] = rtTarg | iTarg;

		rtCache[1] = rtTarg & iTarg;

		uint64 area1 = (uint64)rtCache[0].width * rtCache[0].height * 0.8;

		uint64 area2 = (uint64)rtCache[1].width * rtCache[1].height;

		if(area1 < area2)
		{
			return true;
		}
	}
	return false;
}
