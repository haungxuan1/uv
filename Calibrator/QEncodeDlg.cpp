#include "pch.h"
#include "QEncodeDlg.h"
#include "Helper.h"

IMPLEMENT_DYNAMIC(CQEncodeDlg, CDPIDlg)

BEGIN_MESSAGE_MAP(CQEncodeDlg, CDPIDlg)
	ON_BN_CLICKED(IDC_BT_CALIBRATION_QENCODE, OnClickedQEncode)
	ON_MESSAGE(PQM_CALIBRATOR_QENCODE, OnQEncodeMsg)
END_MESSAGE_MAP()

CQEncodeDlg::CQEncodeDlg(CString rDir, CWnd* pParent)
	:CDPIDlg(rDir, pParent)
{

}

CQEncodeDlg::~CQEncodeDlg()
{

}

void CQEncodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDPIDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BT_CALIBRATION_QENCODE, m_btQEncode);
}

BOOL CQEncodeDlg::OnInitDialog()
{
	CDPIDlg::OnInitDialog();

	return TRUE;
}

void CQEncodeDlg::OnDisableCtrl()
{
	m_btQEncode.EnableWindow(FALSE);

	CDPIDlg::OnDisableCtrl();
}

void CQEncodeDlg::OnEnableCtrl()
{
	CDPIDlg::OnEnableCtrl();

	m_btQEncode.EnableWindow(TRUE);
}

void CQEncodeDlg::OnClickedQEncode()
{
	PostMessage(PQM_CALIBRATOR_DISABLE); // 禁用控件

	PostMessage(PQM_CALIBRATOR_PRO_FREEZE); // 停止相机采集

	PostMessage(PQM_CALIBRATOR_QENCODE);	// 打印二维码
}

void CQEncodeDlg::PrQEncode()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir;

	m_rDir[1] = m_rBaseDir;

	m_rDir[2] = m_rBaseDir + L"\\cache";

	UpdateData(FALSE);

	UpdateData(TRUE);
}

void CQEncodeDlg::QEncode(Mat& cChannel)
{
//	string strQEncode = CString2String(m_rDir[0] + L"\\QEncode.png");

//	Mat cQEncode = imread(strQEncode.c_str(), 0);

	Mat cQEncode = imread("G:\\11.15\\PrQencode\\1.png", 0);

	double dDPI = cQEncode.rows;

	double dPixInMM = dDPI / 25.4;

	int nSpace = dPixInMM * 7.0, nQEncode = 30;

	Mat cTag(Mat::zeros(cQEncode.rows * nQEncode + nSpace * (nQEncode - 1), cQEncode.cols, CV_8UC1));

	Rect rtCopy(0, 0, cQEncode.cols, cQEncode.rows);

	for (int i = 0; i < nQEncode; ++i)
	{
		int idx = i + 1;

		string strFile = string("G:\\11.15\\PrQencode\\") + to_string(idx) + ".png";

		cQEncode = imread(strFile, 0);

		rtCopy.y = i * (cQEncode.rows + nSpace);

		cQEncode.copyTo(cTag(rtCopy));
	}

	copyMakeBorder(cTag, cTag, nSpace, nSpace, nSpace, nSpace, BORDER_CONSTANT, cv::Scalar(0x00));

	double dPrDPI[2] = { 635.0, 1200.0 };

	double dTime[2] = { dPrDPI[0] / dDPI, dPrDPI[1] / dDPI };

	resize(cTag, cTag, Size(dTime[0] * cTag.cols, dTime[1] * cTag.rows));	// dpi 转换

	int nWidth = (cTag.cols + 31) / 32 * 32;

	if (nWidth != cTag.cols) copyMakeBorder(cTag, cTag, 0, 0, 0, nWidth - cTag.cols, BORDER_CONSTANT, cv::Scalar(0x00));

	Gray2Cmykwv(cTag, cChannel, { 3 });
}

LRESULT CQEncodeDlg::OnQEncodeMsg(WPARAM wparam, LPARAM lparam)
{
	PrQEncode();

	PQPrinter().SetPrinterType(CPrinterBase::ePT_Empty);

	PQPrinter().SetInitiator(this);

	PQPrinter().Enable1bitDll(false);

	PQPrinter().Reg();

	Mat cChannel;

	QEncode(cChannel);

	tuple<unsigned int, unsigned char*> vPrtHeader;

	PrtHeader(cChannel.cols, cChannel.rows, cChannel.channels(), 2, vPrtHeader);

	ImPtr::CImPtr cHeader(new Mat(1, get<0>(vPrtHeader), CV_8UC1));

	memcpy(cHeader.Get()->data, get<1>(vPrtHeader), get<0>(vPrtHeader));

	PQPrinter().TCP({ -1, false, {{__FUNCTION__, clock()} }, new array<uint, 3>{false, 0,get<0>(vPrtHeader) },cHeader });

	ImPtr::CImPtr cPr(new Mat(cChannel));

	PQPrinter().Mat2Prt({ 0, false, { {__FUNCTION__, clock()} },new bool(true),  cPr });

	delete get<1>(vPrtHeader);

	return 0;
}
