#include "pch.h"
#include "DPIDlg.h"
#include "PQMsg.h"
#include "Helper.h"

IMPLEMENT_DYNAMIC(CDPIDlg, CMerge2Dlg)

BEGIN_MESSAGE_MAP(CDPIDlg, CMerge2Dlg)
	ON_MESSAGE(PQM_CALIBRATOR_PRO_DPI, OnProDPIMsg)
	ON_MESSAGE(PQM_CALIBRATOR_DPI, OnDPIMsg)
END_MESSAGE_MAP()

CDPIDlg::CDPIDlg(CString rDir, CWnd* pParent)
	:CMerge2Dlg(rDir, pParent)
{

}

CDPIDlg::~CDPIDlg()
{

}

void CDPIDlg::DoDataExchange(CDataExchange* pDX)
{
	CMerge2Dlg::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ET_CALIBRATION_DPI_ROWS, m_cParam.vDPI2[0]);

	DDX_Text(pDX, IDC_ET_CALIBRATION_DPI_COLS, m_cParam.vDPI2[1]);

	DDX_Text(pDX, IDC_ET_CALIBRATION_DPI_ROWS2, m_cParam.vDPI1[0]);

	DDX_Text(pDX, IDC_ET_CALIBRATION_DPI_COLS2, m_cParam.vDPI1[1]);


	DDX_Control(pDX, IDC_ET_CALIBRATION_DPI_ROWS, m_etDPI[0]);

	DDX_Control(pDX, IDC_ET_CALIBRATION_DPI_COLS, m_etDPI[1]);

	DDX_Control(pDX, IDC_ET_CALIBRATION_DPI_ROWS2, m_etDPI[2]);

	DDX_Control(pDX, IDC_ET_CALIBRATION_DPI_COLS2, m_etDPI[3]);
}

BOOL CDPIDlg::OnInitDialog()
{
	CMerge2Dlg::OnInitDialog();

	return TRUE;
}

void CDPIDlg::OnDisableCtrl()
{
	CMerge2Dlg::OnDisableCtrl();
}

void CDPIDlg::OnEnableCtrl()
{
	CMerge2Dlg::OnEnableCtrl();
}

void CDPIDlg::OnPrDPI()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir;

	m_rDir[1] = m_rBaseDir;

	m_rDir[2] = m_rBaseDir + L"\\cache";

	m_cParam.nErode = 75;

	m_cParam.nDilate = 100;

	UpdateData(FALSE);

	UpdateData(TRUE);
}

LRESULT CDPIDlg::OnProDPIMsg(WPARAM wparam, LPARAM lparam)
{
	m_cImControl.ClearLayer();

	OnPrDPI();

	vector<array<wstring, 2>> vROI{
	{ L"ROIMergedEx.png", CString2WString(m_rDir[0] + L"\\ROIMergedEx.png")},

	{ L"Chart.png", CString2WString(m_rDir[0] + L"\\Chart.png")}};

	SetWorkType(eWT_DPI);

	SetDPIParam(
		vROI, 
		{ m_cParam.nForeground, m_cParam.nErode, m_cParam.nDilate }, 
		CString2String(m_rDir[1] + L"\\DPI.png"), 
		CString2String(m_rDir[2]));

	Reg();

	return 0;
}

void CDPIDlg::OnDPI(double hor, double ver)
{
	union
	{
		unsigned long long w;
		long long p;
		double d;
	}h, v;

	h.d = hor, v.d = ver;

	PostMessage(PQM_CALIBRATOR_DPI, h.w, v.p);
}

LRESULT CDPIDlg::OnDPIMsg(WPARAM wparam, LPARAM lparam)
{
	union
	{
		unsigned long long w;
		long long p;
		double d;
	}h, v;
	
	h.w = wparam, v.p = lparam;

	m_cParam.vDPI2[0] = h.d * m_cParam.vDPI1[0];

	m_cParam.vDPI2[1] = v.d * m_cParam.vDPI1[1];

	++m_cParam.nType;

	Unreg();

	UpdateData(FALSE);

	PostMessage(PQM_CALIBRATOR_PARAM_CHANGED);

	PostMessage(PQM_CALIBRATOR_PARAM_END);

	return 0;
}

