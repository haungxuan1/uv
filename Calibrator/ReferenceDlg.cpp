#include "pch.h"
#include "ReferenceDlg.h"
#include "PQMsg.h"
#include "Helper.h"

IMPLEMENT_DYNAMIC(CReferenceDlg, CForegroundDlg)

BEGIN_MESSAGE_MAP(CReferenceDlg, CForegroundDlg)
	ON_MESSAGE(PQM_CALIBRATOR_PRO_REFERENCE, OnProReferenceMsg)
	ON_MESSAGE(PQM_CALIBRATOR_REFERENCE, OnReferenceMsg)
END_MESSAGE_MAP()

CReferenceDlg::CReferenceDlg(CString rDir, CWnd* pParent)
	:CForegroundDlg(rDir, pParent)
{

}

CReferenceDlg::~CReferenceDlg()
{

}

void CReferenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CForegroundDlg::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ET_PADDING_ERODE, m_cParam.nErode);

	DDX_Text(pDX, IDC_ET_PADDING_DILATE, m_cParam.nDilate);

	DDX_Text(pDX, IDC_ET_PADDING_DENSITY, m_cParam.nDensity);

	DDX_Text(pDX, IDC_ET_PADDING_GAUSSIAN, m_cParam.nGaussian);

	DDX_Text(pDX, IDC_ET_PADDING_REFERENCE_X, m_cParam.nReference[0]);

	DDX_Text(pDX, IDC_ET_PADDING_REFERENCE_Y, m_cParam.nReference[1]);

	DDX_Text(pDX, IDC_ET_PADDING_REFERENCE_W, m_cParam.nReference[2]);

	DDX_Text(pDX, IDC_ET_PADDING_REFERENCE_H, m_cParam.nReference[3]);


	DDX_Control(pDX, IDC_ET_PADDING_ERODE, m_etErode);

	DDX_Control(pDX, IDC_ET_PADDING_DILATE, m_etDilate);

	DDX_Control(pDX, IDC_ET_PADDING_DENSITY, m_etDensity);

	DDX_Control(pDX, IDC_ET_PADDING_GAUSSIAN, m_etGaussian);

	DDX_Control(pDX, IDC_ET_PADDING_REFERENCE_X, m_etReference[0]);

	DDX_Control(pDX, IDC_ET_PADDING_REFERENCE_Y, m_etReference[1]);

	DDX_Control(pDX, IDC_ET_PADDING_REFERENCE_W, m_etReference[2]);

	DDX_Control(pDX, IDC_ET_PADDING_REFERENCE_H, m_etReference[3]);
}

BOOL CReferenceDlg::OnInitDialog()
{
	CForegroundDlg::OnInitDialog();

	return TRUE;
}

void CReferenceDlg::OnDisableCtrl()
{
	m_etErode.EnableWindow(FALSE);

	m_etDilate.EnableWindow(FALSE);

	m_etDensity.EnableWindow(FALSE);

	m_etGaussian.EnableWindow(FALSE);

	CForegroundDlg::OnDisableCtrl();
}

void CReferenceDlg::OnEnableCtrl()
{
	CForegroundDlg::OnEnableCtrl();

	m_etErode.EnableWindow(TRUE);

	m_etDilate.EnableWindow(TRUE);

	m_etDensity.EnableWindow(TRUE);

	m_etGaussian.EnableWindow(TRUE);
}

void CReferenceDlg::OnPrReference()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir + L"\\camera";

	m_rDir[1] = m_rBaseDir;

	m_rDir[2] = m_rBaseDir + L"\\cache";

	m_cParam.nGaussian = 15;

	m_cParam.nDensity = 30;

	m_cParam.nErode = 30;

	m_cParam.nDilate = 250;

	UpdateData(FALSE);

	UpdateData(TRUE);
}

LRESULT CReferenceDlg::OnProReferenceMsg(WPARAM wparam, LPARAM lparam)
{
	m_cImControl.ClearLayer();

	OnPrReference();

	vector<array<wstring, 2>> vFile;

	FileInDir(m_rDir[0].GetBuffer(), vFile);

	SortingFiles(vFile);

	SetWorkType(eWT_Reference);

	SetReferenceParam(
		vFile, 
		m_cParam.nExtTriggerFrameCount, 
		{ m_cParam.nGaussian, m_cParam.nForeground, m_cParam.nDensity, m_cParam.nErode, m_cParam.nDilate });

	Reg();

	return 0;
}

void CReferenceDlg::OnReference(Rect& rtRef, ImPtr::CImPtr& cImPtr)
{
	PostMessage(PQM_CALIBRATOR_REFERENCE, (WPARAM)new Rect(rtRef), (LPARAM)new ImPtr::CImPtr(cImPtr));
}

LRESULT CReferenceDlg::OnReferenceMsg(WPARAM wparam, LPARAM lparam)
{
	Rect* lpRef = (Rect*)wparam;

	ImPtr::CImPtr* lpPtr = (ImPtr::CImPtr*)lparam;

	m_cImControl.ClearLayer();

	m_cImControl.CreateLayer(true, true, false, "", true, 1.0, L"²ÎÕÕÎï", *lpPtr, true, 0.0, 0, 0);

	m_cParam.nReference[0] = lpRef->x;

	m_cParam.nReference[1] = lpRef->y;

	m_cParam.nReference[2] = lpRef->width;

	m_cParam.nReference[3] = lpRef->height;

	++m_cParam.nType;

	Unreg();

	UpdateData(FALSE);

	delete lpRef; delete lpPtr;

	PostMessage(PQM_CALIBRATOR_PARAM_CHANGED);

	PostMessage(PQM_CALIBRATOR_TYPE_CHANGED);

	PostMessage(PQM_CALIBRATOR_PRO_ALIGN);

	return 0;
}
