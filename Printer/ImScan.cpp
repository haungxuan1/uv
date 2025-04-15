#include "pch.h"
#include "ImScan.h"

CImScan::CImScan():
	m_uThSize(16),
	m_lpThParam(nullptr), m_lpTerminate(nullptr), m_lpThread(nullptr), m_lpTermination(nullptr)
{

}

CImScan::~CImScan()
{

}

void CImScan::InitParam()
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
			bind(&CImScan::OnImScan, this, placeholders::_1));
	}
}

void CImScan::DestoryParam()
{
	m_lpThParam ? [this]() {delete[] m_lpThParam; }() : []() {}(); m_lpThParam = nullptr;

	m_lpTermination ? [this]() {delete[] m_lpTermination; }() : []() {}(); m_lpTermination = nullptr;

	m_lpThread ? [this]() {delete[] m_lpThread; }() : []() {}(); m_lpThread = nullptr;

	m_lpTerminate ? [this]() {delete[] m_lpTerminate; }() : []() {}(); m_lpTerminate = nullptr;

	m_vParams.clear();
}

bool CImScan::Reg()
{
	if (!CPrinterInit::Reg()) return false;

	for (int i = 0; i < m_uThSize; ++i)
	{
		HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_lpThParam[i], 0, NULL);

		CloseHandle(lpHandle);

		if (!(m_lpThread[i] = (!!lpHandle)))return false;
	}

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	return true;
}

void CImScan::Unreg()
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

	CPrinterInit::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CImScan::AllocateMmemory()
{
	CPrinterInit::AllocateMmemory();

	InitParam();
}

void CImScan::DeallocateMemory()
{
	DestoryParam();

	CPrinterInit::DeallocateMemory();
}

void CImScan::ImScan(vParam& iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cImScan.signal();
}

void CImScan::OnImScan(vParam& iParam)
{
	auto& uIdx = get<ePI_FrameId>(iParam);

	auto& iFrame = get<ePI_ImPtr>(iParam);

	string strFile = m_strImBaseDir + to_string(uIdx) + ".png";

	imwrite(strFile.c_str(), *iFrame.Get());
}