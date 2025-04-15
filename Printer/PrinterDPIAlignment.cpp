#include "pch.h"
#include "PrinterDPIAlignment.h"
#include "PrClock/PrClock.h"
#include "PQMsg.h"

CPrinterDPIAlignment::CPrinterDPIAlignment() :
	m_bThread(false), m_bTermination(false), m_nPrintRows(0)
{
	m_vThParam = make_tuple(
		__FUNCTION__,
		this,
		&m_lLock,
		&m_cAlignment,
		&m_cTermination,
		&m_bTermination,
		&m_vParams,
		bind(&CPrinterDPIAlignment::OnDPIAlignment, this, placeholders::_1));
}

CPrinterDPIAlignment::~CPrinterDPIAlignment()
{

}

bool CPrinterDPIAlignment::Reg()
{
	if (!CMat2Prt::Reg()) return false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	Restart(m_bTermination, m_lLock);

	m_nPrintRows = 0;

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle);

	return (m_bThread = (!!lpHandle));
}

void CPrinterDPIAlignment::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CMat2Prt::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CPrinterDPIAlignment::AllocateMmemory()
{
	CMat2Prt::AllocateMmemory();


}

void CPrinterDPIAlignment::DeallocateMemory()
{
	for (auto& iParam : m_vParams) 
	{ 
		bool* lpFinish = (bool*)get<ePI_UserParam>(iParam); 

		delete lpFinish; 
	}

	m_vParams.clear();

	CMat2Prt::DeallocateMemory();
}

void CPrinterDPIAlignment::DPIAlignment(vParam iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cAlignment.signal();
}

void CPrinterDPIAlignment::OnDPIAlignment(vParam& iParam)
{
	auto& iFrame = get<ePI_FrameId>(iParam);

	auto& iTimer = get<ePI_Timer>(iParam);

	auto& iFinish = *(bool*)get<ePI_UserParam>(iParam);

	auto& iImage = get<ePI_ImPtr>(iParam);

	array<double, 2> dRatio = { m_vPrintDPI[0] / m_dCameraDPI[1], m_vPrintDPI[1] / m_dCameraDPI[1] };

	MakeBorder(*iImage.Get(), 0, 0, m_vOffset[0], 0); // 补偿相机墨头水平距离

	resize(*iImage.Get(), *iImage.Get(), Size2d(dRatio[0] * iImage.Get()->cols, dRatio[1] * iImage.Get()->rows));// 相机dpi->打印dpi

	flip(*iImage.Get(), *iImage.Get(), 1);	//// 打印时需要旋转180°

//	cout << "head: " << m_vPrtHeader[0] << ", " << iImage.Get()->cols << endl;
	// 此时转出的图像宽度应与prt头发出时相等

	if (m_vPrtHeader[0] < iImage.Get()->cols) { cout << "error: " << "Header: " << m_vPrtHeader[0] << " | " << "Im: " << iImage.Get()->cols << endl; }

	m_vPrtHeader[0] != iImage.Get()->cols ? 
		MakeBorder(*iImage.Get(), 0, 0, 0, m_vPrtHeader[0] - iImage.Get()->cols) :
		[]() {}();

	if (iFinish && (m_vPrtHeader[1] != (m_nPrintRows + iImage.Get()->rows)))
	{
		ImPtr::CImPtr cImPtr(new Mat(m_vPrtHeader[1] - m_nPrintRows, iImage.Get()->cols, CV_8UC(m_nTIFFChannel)));

		Rect rtAlig(0, 0, cImPtr.Get()->cols, min(iImage.Get()->rows, cImPtr.Get()->rows));

		(*iImage.Get())(rtAlig).copyTo((*cImPtr.Get())(rtAlig));

		iImage = cImPtr;
	}

	m_nPrintRows += iImage.Get()->rows;

	::PostMessage(m_lpInitiator->GetSafeHwnd(), PQM_MATCH_SCHEDULE, m_nPrintRows, m_vPrtHeader[1]);

	iTimer.push_back({ __FUNCTION__, clock() });

	CMat2Prt::Mat2Prt(iParam);
}

void CPrinterDPIAlignment::MakeBorder(Mat& cMat, int top, int bottom, int left, int right)
{
	Rect rtDes(0, 0, cMat.cols, cMat.rows);

	rtDes.width += (left + right);

	rtDes.height += (top + bottom);

	Mat cDes(rtDes.height, rtDes.width, CV_8UC(cMat.channels()), Scalar(0));

	Rect rtSrc(left, top, min(cMat.cols, cDes.cols), min(cMat.rows, cDes.rows));

	Rect rtWin(0, 0, rtSrc.width, rtSrc.height);

	cMat(rtWin).copyTo(cDes(rtSrc));

	cMat = cDes;
}