#include "pch.h"
#include "CameraDPIAlignment.h"

CCameraDPIAlignment::CCameraDPIAlignment():
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
		bind(&CCameraDPIAlignment::OnCameraDPIAlignment, this, placeholders::_1));
}

CCameraDPIAlignment::~CCameraDPIAlignment()
{

}

bool CCameraDPIAlignment::Reg()
{
	if (!CImageMosaic::Reg()) return false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	Restart(m_bTermination, m_lLock);

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle);

	return (m_bThread = (!!lpHandle));
}

void CCameraDPIAlignment::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CImageMosaic::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CCameraDPIAlignment::AllocateMmemory()
{
	CImageMosaic::AllocateMmemory();

}

void CCameraDPIAlignment::DeallocateMemory()
{
	m_vParams.clear();

	CImageMosaic::DeallocateMemory();
}

void CCameraDPIAlignment::CameraDPIAlignment(vParam& iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cImFrame.signal();
}

void CCameraDPIAlignment::OnCameraDPIAlignment(vParam& iParam)
{
	auto& iFrameId = get<ePI_FrameId>(iParam);

	auto& iFrame = get<ePI_ImPtr>(iParam);

	double dRatio = m_dCameraDPI[1] / m_dCameraDPI[0];

	resize(*iFrame.Get(), *iFrame.Get(), Size2d(dRatio * iFrame.Get()->cols, iFrame.Get()->rows));

	get<ePI_Timer>(iParam).push_back({ __FUNCTION__, clock() });

	CImageMosaic::ImMosaic(iParam);
}