#include "pch.h"
#include "PQPrinter.h"
#include "PQMsg.h"

CPQPrinter::CPQPrinter() :
	m_bThread(false), m_bTermination(false),
	m_lpImInLine(nullptr)
{
	m_vThParam = make_tuple(
		__FUNCTION__,
		this,
		&m_lLock,
		&m_cImCapture,
		&m_cTermination,
		&m_bTermination,
		&m_vParams,
		bind(&CPQPrinter::OnImCapture, this, placeholders::_1));
}

CPQPrinter::~CPQPrinter()
{

}

bool CPQPrinter::Reg(bool bCollect)
{
	CPQPrinter::DeallocateMemory();

	CPQPrinter::AllocateMmemory();

	if (!CFleece::Reg()) return false;

	cout << "Param size: " << m_vParams.size() << endl;

	Restart(m_bTermination, m_lLock);

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle); 

	m_uPrinterType == ePT_Calibrator ? PrinterInit({ 0, false , { {__FUNCTION__, clock()} } ,nullptr, ImPtr::CImPtr() }) : []() {}();

	//采集时，不发送prt头信息
	if (!bCollect) {
		m_uPrinterType == ePT_Print ? PrinterInit({ 0, false , { {__FUNCTION__, clock()} } ,nullptr, ImPtr::CImPtr() }) : []() {}();
	}

	m_uPrinterType == ePT_FastPrint ? ::PostMessage(m_lpInitiator->GetSafeHwnd(), PQM_FLEECE_BEGIN, 0, 0) : []()->BOOL { return 0; }();

	return (m_bThread = (!!lpHandle));
}

void CPQPrinter::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CFleece::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CPQPrinter::DeallocateMemory();
}

void CPQPrinter::AllocateMmemory()
{
	CFleece::AllocateMmemory();

	m_lpImInLine = new vector<vParam>();
}

void CPQPrinter::DeallocateMemory()
{
	m_lpImInLine ? [this]() {delete m_lpImInLine; }() : []() {}(); m_lpImInLine = nullptr;

	m_vParams.clear();

	CFleece::DeallocateMemory();
}

void CPQPrinter::OnPrt()
{
	cout << __FUNCTION__ << endl;

	::SendMessage(m_lpInitiator->GetSafeHwnd(), PQM_FRAME_RESET_IDX, 0, 0); // 帧序号清零

	PrinterInit({ 0, false, { {__FUNCTION__, clock()} } ,nullptr, ImPtr::CImPtr() });
}

void CPQPrinter::StopSend()
{
	unsigned char* lpCmd = nullptr; int nCmdSize = 0;

	Stop(lpCmd, nCmdSize);

	tuple<unsigned char*, int, int, bool> vStop = { lpCmd, nCmdSize, 1, true };

	PQPrinter().StateCmd(lpCmd, nCmdSize);

	tuple<unsigned char*, int, int, bool> vReqStatus = { lpCmd, nCmdSize, 0, false };

	PushCmd(vStop);

	PushCmd(vReqStatus);
}

void CPQPrinter::ImCapture(vParam iParam)
{
	if (m_uPrinterType == ePT_Empty) return;

	Pushback(iParam, m_vParams, m_lLock);

	m_cImCapture.signal();
}

void CPQPrinter::OnImCapture(vParam& iParam)
{
	m_uPrinterType == ePT_Scan ? CPQPrinter::OnImWriteTh(iParam) : CPQPrinter::OnImCaptureTh(iParam);
}

void CPQPrinter::OnImWriteTh(vParam& iParam)
{
	CImRotate::ImRotate(iParam); // 仅透传
}

void CPQPrinter::OnImCaptureTh(vParam& iParam)
{
	IsFleece() ? OnImFleeceTh(iParam) : OnImPrintTh(iParam);
}

void CPQPrinter::OnImFleeceTh(vParam& iParam)
{
	auto& iFrameIdx = get<ePI_FrameId>(iParam);

	if (IsFleeceSkip(iFrameIdx)) return;

	auto& iFleece = get<ePI_Fleece>(iParam); iFleece = true;

	OnPrFrame(iParam);
}

void CPQPrinter::OnImPrintTh(vParam& iParam)
{
	auto& iFrameIdx = get<ePI_FrameId>(iParam);

	if (IsSkip(iFrameIdx)) return;

	if (!IsEnable(iFrameIdx)) return;

	OnPrFrame(iParam);
}

bool CPQPrinter::IsFleeceSkip(int& nFrameIdx)
{
	nFrameIdx -= m_vSkipFrame[0];

	return nFrameIdx <= 0;
}

bool CPQPrinter::IsSkip(int& nFrameIdx)
{
	nFrameIdx -= m_vSkipFrame[1];

	return nFrameIdx <= 0;
}

bool CPQPrinter::IsEnable(int& nFrameIdx)
{
	int nLine = (nFrameIdx - 1) / m_nImInLine;

	return !!!(nLine % m_nSkipLine);
}

void CPQPrinter::OnPrFrame(vParam& iParam)
{
	m_lpImInLine->push_back(iParam);

	if (m_lpImInLine->size() < m_nImInLine) return;

	cout << __FUNCTION__ << endl;

	auto iBegin = m_lpImInLine->begin();

	auto& iFrameId = get<ePI_FrameId>(*iBegin);

	auto& iFleece = get<ePI_Fleece>(*iBegin);

	auto& iTimer = get<ePI_Timer>(*iBegin); iTimer.push_back({ __FUNCTION__, clock() });

	CHomography::ImHomography({ iFrameId, iFleece, iTimer, (void*)m_lpImInLine, ImPtr::CImPtr() });

	m_lpImInLine = new vector<vParam>();
}
