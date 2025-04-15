#include "pch.h"
#include "ForegroundDlg.h"
#include "PQMsg.h"
#include "Helper.h"

IMPLEMENT_DYNAMIC(CForegroundDlg, CCameraDlg)

BEGIN_MESSAGE_MAP(CForegroundDlg, CCameraDlg)
	ON_MESSAGE(PQM_CALIBRATOR_PRO_FOREGROUND, OnProForegroundMsg)
	ON_MESSAGE(PQM_CALIBRATOR_HISTOGRAM, OnHistogramMsg)
	ON_MESSAGE(PQM_CALIBRATOR_FOREGROUND, OnForegroundMsg)
END_MESSAGE_MAP()

CForegroundDlg::CForegroundDlg(CString rDir, CWnd* pParent)
	:CCameraDlg(rDir, pParent),
	m_lpHistogram(nullptr), m_lpSplitPoint(nullptr)
{
	m_rDir[0] = m_rDir[1] = m_rDir[2] = m_rBaseDir;

	wcout << m_rBaseDir.GetBuffer() << endl;
}

CForegroundDlg::~CForegroundDlg()
{

}

void CForegroundDlg::DoDataExchange(CDataExchange* pDX)
{
	CCameraDlg::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ET_PADDING_BLOCK, m_cParam.nBlock);

	DDX_Text(pDX, IDC_ET_PADDING_TH, m_cParam.nForeground);

	DDX_Text(pDX, IDC_ET_PADDING_DIR_INPUT, m_rDir[0]);

	DDX_Text(pDX, IDC_ET_PADDING_DIR_OUTPUT, m_rDir[1]);

	DDX_Text(pDX, IDC_ET_PADDING_DIR_CACHE, m_rDir[2]);


	DDX_Control(pDX, IDC_ET_PADDING_BLOCK, m_etBlock);

	DDX_Control(pDX, IDC_ET_PADDING_TH, m_etForeground);

	DDX_Control(pDX, IDC_CH_PADDING_HISTOGRAM, m_cHistogram);

	DDX_Control(pDX, IDC_ET_PADDING_DIR_INPUT, m_etDir[0]);

	DDX_Control(pDX, IDC_ET_PADDING_DIR_OUTPUT, m_etDir[1]);

	DDX_Control(pDX, IDC_ET_PADDING_DIR_CACHE, m_etDir[2]);
}

BOOL CForegroundDlg::OnInitDialog()
{
	CCameraDlg::OnInitDialog();

	OnInitHistogram();

	UpdateData(FALSE);

	return TRUE;
}

void CForegroundDlg::OnInitHistogram()
{
	CChartAxis* pAxis = NULL;
	pAxis = m_cHistogram.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_cHistogram.CreateStandardAxis(CChartCtrl::LeftAxis);
	pAxis->SetAutomatic(true);
	m_cHistogram.SetEdgeEnabled(false);
	m_cHistogram.GetLegend()->SetVisible(true);
	m_cHistogram.SetBackColor(0xffffff);
	CChartLegend* lpLegend = m_cHistogram.GetLegend();
	lpLegend->UndockLegend(440, 20);
	lpLegend->EnableShadow(false);
	lpLegend->SetTransparent(true);
	m_lpHistogram = m_cHistogram.CreateLineSerie();
	m_lpHistogram->SetWidth(1);
	m_lpHistogram->SetColor(0xff0000);
	m_lpHistogram->EnableShadow(false);
	m_lpSplitPoint = m_cHistogram.CreatePointsSerie();
	m_lpSplitPoint->SetPointSize(10, 10);
	m_lpSplitPoint->SetPointType(CChartPointsSerie::ptEllipse);
	m_lpSplitPoint->SetColor(0x00ff00);
}

void CForegroundDlg::OnDisableCtrl()
{
	m_etBlock.EnableWindow(FALSE);

	m_etForeground.EnableWindow(FALSE);

	CCameraDlg::OnDisableCtrl();
}

void CForegroundDlg::OnEnableCtrl()
{
	CCameraDlg::OnEnableCtrl();

	m_etBlock.EnableWindow(TRUE);

	m_etForeground.EnableWindow(TRUE);
}

void CForegroundDlg::OnPrForeground()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir + L"\\camera";

	m_rDir[1] = m_rBaseDir;

	m_rDir[2] = m_rBaseDir + L"\\cache";

	UpdateData(FALSE);

	UpdateData(TRUE);
}

LRESULT CForegroundDlg::OnProForegroundMsg(WPARAM wparam, LPARAM lparam)
{
	m_cImControl.ClearLayer();

	OnPrForeground();

	m_lpHistogram->SetName(L"灰度直方图");

	m_lpSplitPoint->SetName(L"前景阈值");

	m_lpHistogram->ClearSerie();

	m_lpSplitPoint->ClearSerie();

	vector<array<wstring, 2>> vFile;

	FileInDir(m_rDir[0].GetBuffer(), vFile);

	SortingFiles(vFile);

	SetWorkType(eWT_Foreground);

	SetForegroundParam(vFile, m_cParam.nExtTriggerFrameCount, m_cParam.nBlock);

	Reg();

	return 0;
}

void CForegroundDlg::OnHistogram(int nRemaining, ImPtr::CImPtr& cImage, ImPtr::CImPtr& cHistogram)
{
	tuple<int, ImPtr::CImPtr, ImPtr::CImPtr>* lpParam = new tuple<int, ImPtr::CImPtr, ImPtr::CImPtr>(nRemaining, cImage, cHistogram);

	PostMessage(PQM_CALIBRATOR_HISTOGRAM, 0, (LPARAM)lpParam);
}

void CForegroundDlg::OnForeground(int nForeground)
{
	PostMessage(PQM_CALIBRATOR_FOREGROUND, (WPARAM)nForeground, 0);
}

LRESULT CForegroundDlg::OnHistogramMsg(WPARAM wparam, LPARAM lparam)
{
	tuple<int, ImPtr::CImPtr, ImPtr::CImPtr>* lpParam = (tuple<int, ImPtr::CImPtr, ImPtr::CImPtr>*)lparam;

	auto nRemaining = get<0>(*lpParam);

	auto cImage = get<1>(*lpParam);

	auto cHistogram = get<2>(*lpParam);

	int* lpHis = (int*)cHistogram.Get()->data;

	vector<double> x(cHistogram.Get()->cols - 1), y(cHistogram.Get()->cols - 1);

	double dMin = 0.0, dMax = 0.0;

	minMaxLoc(*cHistogram.Get(), &dMin, &dMax); // 不影响

	for (int i = 0; i < x.size(); ++i) { x[i] = i; y[i] = lpHis[i]; }

	m_lpHistogram->SetPoints(x.data(), y.data(), x.size());

	m_lpSplitPoint->AddPoint(lpHis[256], (dMax + dMin) * 0.5);

	m_cImControl.ClearLayer();

	m_cImControl.CreateLayer(true, true, false, "", true, 1.0, WString2CString(to_wstring(nRemaining)), cImage, true, 0.0, 0, 0);

	delete lpParam;

	return 0;
}

LRESULT CForegroundDlg::OnForegroundMsg(WPARAM wparam, LPARAM lparam)
{
	m_cParam.nForeground = (int)wparam;

	++m_cParam.nType;

	Unreg();

	UpdateData(FALSE);

	PostMessage(PQM_CALIBRATOR_PARAM_CHANGED);

	PostMessage(PQM_CALIBRATOR_TYPE_CHANGED);

	PostMessage(PQM_CALIBRATOR_PRO_REFERENCE);

	return 0;
}
