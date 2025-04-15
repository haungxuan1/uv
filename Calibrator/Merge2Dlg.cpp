#include "pch.h"
#include "Merge2Dlg.h"
#include "PQMsg.h"
#include "Helper.h"

IMPLEMENT_DYNAMIC(CMerge2Dlg, CHomographyDlg)

BEGIN_MESSAGE_MAP(CMerge2Dlg, CHomographyDlg)
	ON_MESSAGE(PQM_CALIBRATOR_PRO_MERGE2, OnProMerge2Msg)
	ON_MESSAGE(PQM_CALIBRATOR_MERGING2, OnMerging2Msg)
	ON_MESSAGE(PQM_CALIBRATOR_MERGE2, OnMerge2Msg)
END_MESSAGE_MAP()

CMerge2Dlg::CMerge2Dlg(CString rDir, CWnd* pParent)
	:CHomographyDlg(rDir, pParent)
{

}

CMerge2Dlg::~CMerge2Dlg()
{

}

void CMerge2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CHomographyDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ET_CALIBRATION_ASSESSMENT, m_etAssessment);

	DDX_Text(pDX, IDC_ET_CALIBRATION_ASSESSMENT, m_cParam.dAssessment);
}

BOOL CMerge2Dlg::OnInitDialog()
{
	CHomographyDlg::OnInitDialog();

	return TRUE;
}

void CMerge2Dlg::OnDisableCtrl()
{
	CHomographyDlg::OnDisableCtrl();
}

void CMerge2Dlg::OnEnableCtrl()
{
	CHomographyDlg::OnEnableCtrl();
}

void CMerge2Dlg::OnPrMerge2()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir + L"\\camera";

	m_rDir[1] = m_rBaseDir;

	m_rDir[2] = m_rBaseDir + L"\\cache";

	UpdateData(FALSE);

	UpdateData(TRUE);
}

LRESULT CMerge2Dlg::OnProMerge2Msg(WPARAM wparam, LPARAM lparam)
{
	m_cImControl.ClearLayer();

	OnPrMerge2();

	vector<array<wstring, 2>> vFile;

	FileInDir(m_rDir[0].GetBuffer(), vFile);

	SortingFiles(vFile);

	SetWorkType(eWT_MergingEx);

	SetMergingExParam(
		vFile,
		m_cParam.nExtTriggerFrameCount,
		m_cParam.nPassInStride,
		m_cParam.nReference,
		m_cParam.vWH1,
		m_cParam.vWH2,
		m_cParam.vWH2Ex,
		m_cParam.vVertex1,
		m_cParam.vVertex2,
		m_cParam.vFill,
		m_cParam.M,
		CString2String(m_rDir[2] + L"\\Aligned.png"), 
		CString2String(m_rDir[1] + L"\\MergedEx.png"),
		CString2String(m_rDir[1] + L"\\ROIMergedEx.png"));

	Reg();

	return 0;
}

void CMerge2Dlg::OnMergingEx(ImPtr::CImPtr cImPtr)
{
	PostMessage(PQM_CALIBRATOR_MERGING2, 0, (LPARAM)new ImPtr::CImPtr(cImPtr));
}

void CMerge2Dlg::OnMergeEx(array<double, 2>& vAlign, ImPtr::CImPtr cImPtr)
{
	PostMessage(PQM_CALIBRATOR_MERGE2, (WPARAM)new array<double, 2>(vAlign), (LPARAM)new ImPtr::CImPtr(cImPtr));
}

LRESULT CMerge2Dlg::OnMerging2Msg(WPARAM wparam, LPARAM lparam)
{
	ImPtr::CImPtr* lpPtr = (ImPtr::CImPtr*)lparam;

	m_cImControl.ClearLayer();

	m_cImControl.CreateLayer(true, true, false, "", true, 1.0, L"Í¼Æ¬Æ´½Ó", *lpPtr, true, 0.0, 0, 0);

	delete lpPtr;

	return 0;
}

LRESULT CMerge2Dlg::OnMerge2Msg(WPARAM wparam, LPARAM lparam)
{
	array<double, 2>* lpAlign = (array<double, 2>*)wparam;

	OnMerging2Msg(wparam, lparam);

	m_cParam.dAssessment = (*lpAlign)[0];

	m_cParam.nPixelInPass = round((*lpAlign)[1]);

	++m_cParam.nType;

	Unreg();

	UpdateData(FALSE);

	delete lpAlign;

	PostMessage(PQM_CALIBRATOR_PARAM_CHANGED);

	PostMessage(PQM_CALIBRATOR_TYPE_CHANGED);

	PostMessage(PQM_CALIBRATOR_PRO_DPI);

	return 0;
}
