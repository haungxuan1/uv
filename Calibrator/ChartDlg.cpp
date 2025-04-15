#include "pch.h"
#include "ChartDlg.h"
#include "PQMsg.h"
#include "Helper.h"
#include "CalibratorDlg.h"

IMPLEMENT_DYNAMIC(CChartDlg, CQEncodeDlg)

BEGIN_MESSAGE_MAP(CChartDlg, CQEncodeDlg)
	ON_BN_CLICKED(IDC_BT_CALIBRATION_SCAN, OnClickedImScan)
	ON_BN_CLICKED(IDC_BT_CALIBRATION_GEN, OnClickedImGen)
	ON_BN_CLICKED(IDC_BT_CALIBRATION_PRINT_CALIBRATION_IMAGE, OnClickedImPrint)
	ON_BN_CLICKED(IDC_BT_CALIBRATION_PRINT2, OnClickedImPrint2)

	ON_MESSAGE(PQM_CALIBRATOR_CHART, OnChartMsg)
	ON_MESSAGE(PQM_CALIBRATOR_SCAN, OnScanMsg)
	ON_MESSAGE(PQM_CALIBRATOR_PRINT, OnPrintMsg)
	ON_MESSAGE(PQM_MATCH_IM_TCP_FINISH, OnTCPFinishMsg)
	ON_MESSAGE(PQM_CALIBRATOR_CAPTURE, OnCaptureMsg)
	ON_MESSAGE(PQM_MATCH_TRANSPARENT, OnTransparentMsg)
END_MESSAGE_MAP()

CChartDlg::CChartDlg(CString rDir, CWnd* pParent)
	:CQEncodeDlg(rDir, pParent),
	m_nChart{ 20, 35 }, // 打印 ：{20, 35} | 扫描：{30, 70}
	m_nArea{ 2200, 500 }
{

}

CChartDlg::~CChartDlg()
{

}

void CChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CQEncodeDlg::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ET_CALIBRATION_AREA_ROWS, m_nArea[0]);

	DDX_Text(pDX, IDC_ET_CALIBRATION_AREA_COLS, m_nArea[1]);

	DDX_Text(pDX, IDC_ET_CALIBRATION_DIST_ROWS, m_cParam.vDist[0]);

	DDX_Text(pDX, IDC_ET_CALIBRATION_DIST_COLS, m_cParam.vDist[1]);

	DDX_Text(pDX, IDC_ET_CALIBRATION_CHART_ROWS, m_nChart[0]);

	DDX_Text(pDX, IDC_ET_CALIBRATION_CHART_COLS, m_nChart[1]);


	DDX_Control(pDX, IDC_BT_CALIBRATION_GEN, m_btImGen);

	DDX_Control(pDX, IDC_BT_CALIBRATION_PRINT_CALIBRATION_IMAGE, m_btImPrint);

	DDX_Control(pDX, IDC_BT_CALIBRATION_SCAN, m_btImScan);

	DDX_Control(pDX, IDC_BT_CALIBRATION_PRINT2, m_btImPrint2);

	DDX_Control(pDX, IDC_ET_CALIBRATION_AREA_ROWS, m_etArea[0]);

	DDX_Control(pDX, IDC_ET_CALIBRATION_AREA_COLS, m_etArea[1]);

	DDX_Control(pDX, IDC_ET_CALIBRATION_DIST_ROWS, m_etDist[0]);

	DDX_Control(pDX, IDC_ET_CALIBRATION_DIST_COLS, m_etDist[1]);

	DDX_Control(pDX, IDC_ET_CALIBRATION_CHART_ROWS, m_etChart[0]);

	DDX_Control(pDX, IDC_ET_CALIBRATION_CHART_COLS, m_etChart[1]);
}

BOOL CChartDlg::OnInitDialog()
{
	CQEncodeDlg::OnInitDialog();

	return TRUE;
}

void CChartDlg::OnDisableCtrl()
{
	m_btImGen.EnableWindow(FALSE);

	m_btImPrint.EnableWindow(FALSE);

	m_btImScan.EnableWindow(FALSE);

	m_btImPrint2.EnableWindow(FALSE);

	m_etArea[0].EnableWindow(FALSE); 

	m_etArea[1].EnableWindow(FALSE);

	m_etDist[0].EnableWindow(FALSE); 

	m_etDist[1].EnableWindow(FALSE);

	m_etChart[0].EnableWindow(FALSE); 

	m_etChart[1].EnableWindow(FALSE);

	CQEncodeDlg::OnDisableCtrl();
}

void CChartDlg::OnEnableCtrl()
{
	CQEncodeDlg::OnEnableCtrl();

	m_btImGen.EnableWindow(TRUE);

	m_btImPrint.EnableWindow(TRUE);

	m_btImScan.EnableWindow(TRUE);

	m_btImPrint2.EnableWindow(TRUE);

	m_etArea[0].EnableWindow(TRUE);

	m_etArea[1].EnableWindow(TRUE);

	m_etDist[0].EnableWindow(TRUE);

	m_etDist[1].EnableWindow(TRUE);

	m_etChart[0].EnableWindow(TRUE);

	m_etChart[1].EnableWindow(TRUE);
}

void CChartDlg::OnPrChart()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir;

	m_rDir[1] = m_rBaseDir;

	m_rDir[2] = m_rBaseDir + L"\\cache";

	UpdateData(FALSE);

	UpdateData(TRUE);
}

//// 打印时，校准为黑色，对比时为白色，因此需要反色处理 mat = 0xff - mat
void CChartDlg::OnClickedImGen()
{
	OnPrChart();

	Mat* lpChart = nullptr;

	Chart(m_nChart[1], m_nChart[0], lpChart);

	*lpChart = 255 - *lpChart;

	imwrite(CString2String(m_rDir[1] + L"\\Chart.png").c_str(), *lpChart);

	delete lpChart;
}

void CChartDlg::OnClickedImPrint()
{
	PostMessage(PQM_CALIBRATOR_DISABLE); // 禁用控件

	PostMessage(PQM_CALIBRATOR_PRO_FREEZE); // 停止相机采集

	PostMessage(PQM_CALIBRATOR_CHART);	// 打印校准格
}

LRESULT CChartDlg::OnChartMsg(WPARAM wparam, LPARAM lparam)
{
	UpdateData(TRUE);

	PQPrinter().SetPrinterType(CPrinterBase::ePT_Empty);

	PQPrinter().SetInitiator(this);

	PQPrinter().Enable1bitDll(false);

	PQPrinter().Reg();

	Mat* lpChart = nullptr, cChannel;

	Chart(m_nChart[1], m_nChart[0], lpChart);

	Gray2Cmykwv(*lpChart, cChannel, { 3 });

	tuple<unsigned int, unsigned char*> vPrtHeader;

	PrtHeader(cChannel.cols, cChannel.rows, cChannel.channels(), 2, vPrtHeader);

	ImPtr::CImPtr cHeader(new Mat(1, get<0>(vPrtHeader), CV_8UC1));

	memcpy(cHeader.Get()->data, get<1>(vPrtHeader), get<0>(vPrtHeader));

	PQPrinter().TCP({ -1, false, {{__FUNCTION__, clock()} }, new array<uint,3>{false, 0,get<0>(vPrtHeader) },cHeader });

	ImPtr::CImPtr cPr(new Mat(cChannel));

	PQPrinter().Mat2Prt({ 0, false, { {__FUNCTION__, clock()} },new bool(true),  cPr });

	delete get<1>(vPrtHeader); delete lpChart;

	return 0;
}

void CChartDlg::OnClickedImScan()
{
	PostMessage(PQM_CALIBRATOR_DISABLE); // 禁用控件

	PostMessage(PQM_CALIBRATOR_PRO_CRAB); // 开启相机采集

	PostMessage(PQM_CALIBRATOR_SCAN);	// 扫描校准格
}

void CChartDlg::OnPrScan()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir;

	m_rDir[1] = m_rBaseDir + L"\\camera";

	m_rDir[2] = m_rBaseDir + L"\\cache";

	UpdateData(FALSE);

	UpdateData(TRUE);
}

LRESULT CChartDlg::OnScanMsg(WPARAM wparam, LPARAM lparam)
{
	OnPrScan();

	PQPrinter().SetBaseDir(CString2String(m_rDir[1] + L"\\"));

	PQPrinter().SetPrinterType(CPrinterBase::ePT_Scan);

	PQPrinter().SetInitiator(this);

	PQPrinter().Enable1bitDll(false);

	PQPrinter().Reg();

	Mat* lpChart = nullptr, cChannel;

	Chart(m_nChart[1], m_nChart[0], lpChart);

	Gray2Cmyk(*lpChart, cChannel, {});

	tuple<unsigned int, unsigned char*> vPrtHeader;

	PrtHeader(cChannel.cols, cChannel.rows, cChannel.channels(), 2, vPrtHeader);

	ImPtr::CImPtr cHeader(new Mat(1, get<0>(vPrtHeader), CV_8UC1));

	memcpy(cHeader.Get()->data, get<1>(vPrtHeader), get<0>(vPrtHeader));

	PQPrinter().TCP({ -1, false, {{__FUNCTION__, clock()} }, new array<uint,3>{false, 0,get<0>(vPrtHeader) },cHeader });

	ImPtr::CImPtr cPr(new Mat(cChannel));

	PQPrinter().Mat2Prt({ 0, false, { {__FUNCTION__, clock()} },new bool(true),  cPr });

	delete  get<1>(vPrtHeader); delete lpChart;

	return 0;
}

// 校准打印
void CChartDlg::OnClickedImPrint2()
{
	PostMessage(PQM_CALIBRATOR_DISABLE); // 禁用控件

	PostMessage(PQM_CALIBRATOR_PRO_CRAB); // 开启相机采集

	PostMessage(PQM_CALIBRATOR_PRINT);	// 校准打印
}

void CChartDlg::OnPrPrint()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir;

	m_rDir[1] = m_rBaseDir;

	m_rDir[2] = m_rBaseDir + L"\\cache";

	UpdateData(FALSE);

	UpdateData(TRUE);
}

LRESULT CChartDlg::OnPrintMsg(WPARAM wparam, LPARAM lparam)
{
	OnPrPrint();

	Mat M(3, 3, CV_64FC1);

	memcpy(M.data, m_cParam.M.data(), sizeof(double) * m_cParam.M.size());

	array<double, 2> vPixInMM = { m_cParam.vDPI1[0] / 25.4, m_cParam.vDPI1[1] / 25.4 }; // 打印 像素/mm

	PQPrinter().SetImInLine(m_cParam.nExtTriggerFrameCount);

	PQPrinter().SetForegroundThreshold(m_cParam.nForeground);

	PQPrinter().SetTransparentThreshold(m_cParam.nForeground);

	PQPrinter().SetSkipFrame({ 0, 2 * PQPrinter().GetImInLine() }); // 针对光电传感器补丁

	PQPrinter().SetSkipLine(m_cParam.nPassInStride);

	PQPrinter().SetImWH(m_cParam.vWH1);

	PQPrinter().SetImWH2(m_cParam.vWH2);

	PQPrinter().SetImWH2Ex(m_cParam.vWH2Ex);

	PQPrinter().SetImHomography(M);

	PQPrinter().SetImCompensation(m_cParam.vFill);	// 透视变换参数

	PQPrinter().SetAligParam({ m_cParam.nForeground, 0, m_cParam.nPixelInPass, 32, m_cParam.nErode, m_cParam.nDilate }); // 设置参照物对齐参数

	PQPrinter().SetCmeraDpi(m_cParam.vDPI2);

	PQPrinter().SetPrinteDpi(m_cParam.vDPI1);

	PQPrinter().SetOffset(m_cParam.vDist); // 偏移距离 // 该值为相机dpi

	PQPrinter().SetPrintHeader({ (int)(vPixInMM[0] * m_nArea[0]),  (int)(vPixInMM[1] * m_nArea[1]) });	// 打印dpi

	auto iCaDpi = PQPrinter().GetCmeraDpi();

	auto iPrDpi = PQPrinter().GetPrinteDpi();

	array<double, 2> dPr2Camer = { iCaDpi[0] / iPrDpi[0], iCaDpi[1] / iPrDpi[1] }; // 相机原始采图 // 此时相机dpi尚未做对齐

	array<int, 2> vHeader = PQPrinter().GetPrintHeader();

	//// 转相机dpi
	array<double, 2> vArea = { dPr2Camer[0] * vHeader[0], dPr2Camer[1] * vHeader[1] };

	double dFrameInLine = (double)(vArea[0] - m_cParam.vDist[0]) / 30000.0;

	cout << "area: " << vArea[0] << ", " << vArea[1] << " | " << dFrameInLine << endl;

	double dX2Y = iCaDpi[1] / iCaDpi[0];

	array<int, 2> vSight = { (int)(dX2Y * vArea[0]) - m_cParam.vDist[0], (int)(13056 * 2.0) }; // 相机实际需要提供的视野填充 = prt.w - 填充.w // 此时相机dpi需要对齐

	PQPrinter().SetSight(vSight);

	PQPrinter().Enable1bitDll(false);

	PQPrinter().SetPrinterType(CPrinterBase::ePT_Calibrator);

	PQPrinter().SetInitiator(this);

	PQPrinter().SetBaseDir(CString2String(m_rDir[1] + L"\\"));

	PQPrinter().SetTIFF("", 4);

	PQPrinter().Reg();

	return 0;
}

LRESULT CChartDlg::OnTCPFinishMsg(WPARAM wparam, LPARAM lparam)
{
	SendMessage(PQM_CALIBRATOR_PRO_FREEZE); // 开启相机采集

	PQPrinter().Unreg();

	m_cImControl.ClearLayer();

	PostMessage(PQM_CALIBRATOR_ENABLE); // 控件使能

	return 0;
}

LRESULT CChartDlg::OnCaptureMsg(WPARAM wparam, LPARAM lparam)
{
	int nFrameId = (int)wparam;

	ImPtr::CImPtr* lpFrame = (ImPtr::CImPtr*)lparam;

	PQPrinter().ImCapture({ nFrameId, false, { {__FUNCTION__, clock()} }, nullptr, *lpFrame });

	delete lpFrame;

	return 0;
}

LRESULT CChartDlg::OnTransparentMsg(WPARAM wparam, LPARAM lparam)
{
	ImPtr::CImPtr* lpImTr = (ImPtr::CImPtr*)lparam;

	m_cImControl.ClearLayer();

	m_cImControl.CreateLayer(true, true, false, "", true, 1.0, L"透传测试", *lpImTr, true, 0.0, 0, 0);

	delete lpImTr;

	return 0;
}

