#include "pch.h"
#include "PrinterInit.h"
#include "Helper.h"
#include "PQMsg.h"

CPrinterInit::CPrinterInit() :
	m_bTermination(false), m_bThread(false)
{
	m_vThParam = make_tuple(
		__FUNCTION__,
		this,
		&m_lLock,
		&m_cImInit,
		&m_cTermination,
		&m_bTermination,
		&m_vParams,
		bind(&CPrinterInit::OnPrinterInit, this, placeholders::_1));
}

CPrinterInit::~CPrinterInit()
{

}

bool CPrinterInit::Reg()
{
	if (!CHomography::Reg()) return false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	Restart(m_bTermination, m_lLock);

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle);

	return (m_bThread = (!!lpHandle));
}

void CPrinterInit::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CHomography::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CPrinterInit::AllocateMmemory()
{
	CHomography::AllocateMmemory();

}

void CPrinterInit::DeallocateMemory()
{
	m_vParams.clear();

	CHomography::DeallocateMemory();
}

void CPrinterInit::PrinterInit(vParam iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cImInit.signal();
}

void CPrinterInit::OnPrinterInit(vParam& iParam)
{
	// 发送图片 = 转打印dpi后图片
	// 
	// 发出一个prt头
	// 
	// 发出一个墨头相机垂直方向偏移
	cout << "TIFF CH: " << m_nTIFFChannel << endl;

	tuple<unsigned int, unsigned char*> vPrtHeader;

	PrtHeader(m_vPrtHeader[0], m_vPrtHeader[1], m_nTIFFChannel, (m_b1bitDll ? 1 : 2), vPrtHeader);

	ImPtr::CImPtr cHeader(new Mat(1, get<0>(vPrtHeader), CV_8UC1));

	memcpy(cHeader.Get()->data, get<1>(vPrtHeader), get<0>(vPrtHeader));
	
	// Prt 文件头
	TCP({ -1, false, {{__FUNCTION__, clock()}}, new array<unsigned int,3>{false, 0, (unsigned int)get<0>(vPrtHeader)}, cHeader });

	ImPtr::CImPtr cOy(new Mat(Mat::zeros(m_vOffset[1], m_vSight[1], CV_8UC(m_nTIFFChannel))));

	m_uPrinterType == ePT_Calibrator ? CPrinterDPIAlignment::DPIAlignment({ 0, false, {{__FUNCTION__, clock()}}, new bool(false), cOy }) : []() {}();

	m_uPrinterType == ePT_Print ? CPrinterDPIAlignment::DPIAlignment({ 0, false, {{__FUNCTION__, clock()}}, new bool(false), cOy }) : []() {}();

	m_uPrinterType == ePT_FastPrint ? ::SendMessage(m_lpInitiator->GetSafeHwnd(), PQM_FLEECE_END, 0, 0) : []()->BOOL { return 0; }();
}