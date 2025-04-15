#include "pch.h"
#include "ImRotate.h"

CImRotate::CImRotate() :
	m_uThSize(4),
	m_lpThParam(nullptr), m_lpTerminate(nullptr), m_lpThread(nullptr), m_lpTermination(nullptr)
{

}

CImRotate::~CImRotate()
{

}

void CImRotate::InitParam()
{
	m_lpTerminate = new bool[m_uThSize];

	m_lpThread = new bool[m_uThSize];

	m_lpTermination = new CCondition[m_uThSize];

	m_lpThParam = new vThParam[m_uThSize];

	memset(m_lpThread, 0, m_uThSize); memset(m_lpTerminate, 0, m_uThSize);

	for (int i = 0; i < m_uThSize; ++i)
	{
		m_lpThParam[i] = make_tuple(
			__FUNCTION__,
			this,
			&m_lLock,
			&m_cImScan,
			&m_lpTermination[i],
			&m_lpTerminate[i],
			&m_vParams,
			bind(&CImRotate::OnImRotate, this, placeholders::_1));
	}
}

void CImRotate::DestoryParam()
{
	m_lpThParam ? [this]() {delete[] m_lpThParam; }() : []() {}(); m_lpThParam = nullptr;

	m_lpTermination ? [this]() {delete[] m_lpTermination; }() : []() {}(); m_lpTermination = nullptr;

	m_lpThread ? [this]() {delete[] m_lpThread; }() : []() {}(); m_lpThread = nullptr;

	m_lpTerminate ? [this]() {delete[] m_lpTerminate; }() : []() {}(); m_lpTerminate = nullptr;

	m_vParams.clear();
}

bool CImRotate::Reg()
{
	if (!CImScan::Reg()) return false;

	for (int i = 0; i < m_uThSize; ++i)
	{
		HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_lpThParam[i], 0, NULL);

		CloseHandle(lpHandle);

		if (!(m_lpThread[i] = (!!lpHandle)))return false;
	}

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	return true;
}

void CImRotate::Unreg()
{
	if (m_lpThread)
	{
		for (int i = 0; i < m_uThSize; ++i)
		{

			if (m_lpThread[i])
			{
				CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_lpThParam[i], 0, NULL));

				m_lpTermination[i].wait(); m_lpThread[i] = false;
			}
		}
	}
	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CImScan::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CImRotate::AllocateMmemory()
{
	CImScan::AllocateMmemory();

	InitParam();
}

void CImRotate::DeallocateMemory()
{
	DestoryParam();

	CImScan::DeallocateMemory();
}

void CImRotate::ImRotate(vParam& iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cImScan.signal();
}

void CImRotate::OnImRotate(vParam& iParam)
{
	auto& iFrameId = get<ePI_FrameId>(iParam);

	auto& iFrame = get<ePI_ImPtr>(iParam);

	rotate(*iFrame.Get(), *iFrame.Get(), ROTATE_90_COUNTERCLOCKWISE);

	get<ePI_Timer>(iParam).push_back({ __FUNCTION__, clock() });

	CImScan::ImScan(iParam);
}