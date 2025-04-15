#include "pch.h"
#include "HomographyDlg.h"
#include "PQMsg.h"
#include "Helper.h"

IMPLEMENT_DYNAMIC(CHomographyDlg, CPrespectiveDlg)

BEGIN_MESSAGE_MAP(CHomographyDlg, CPrespectiveDlg)
	ON_MESSAGE(PQM_CALIBRATOR_PRO_HOMOGRAPHY, OnProHomographyMsg)
	ON_MESSAGE(PQM_CALIBRATOR_TRANSLATING, OnTranslatingMsg)
	ON_MESSAGE(PQM_CALIBRATOR_HOMOGRAPHY, OnHomographyMsg)
END_MESSAGE_MAP()

CHomographyDlg::CHomographyDlg(CString rDir, CWnd* pParent)
	:CPrespectiveDlg(rDir, pParent)
{

}

CHomographyDlg::~CHomographyDlg()
{

}

void CHomographyDlg::DoDataExchange(CDataExchange* pDX)
{
	CPrespectiveDlg::DoDataExchange(pDX);
}

BOOL CHomographyDlg::OnInitDialog()
{
	CPrespectiveDlg::OnInitDialog();

	return TRUE;
}

void CHomographyDlg::OnDisableCtrl()
{
	CPrespectiveDlg::OnDisableCtrl();
}

void CHomographyDlg::OnEnableCtrl()
{
	CPrespectiveDlg::OnEnableCtrl();
}

void CHomographyDlg::OnPrHomography()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir + L"\\camera";

	m_rDir[1] = m_rBaseDir + L"\\camera";

	m_rDir[2] = m_rBaseDir + L"\\cache";

	UpdateData(FALSE);

	UpdateData(TRUE);
}

LRESULT CHomographyDlg::OnProHomographyMsg(WPARAM wparam, LPARAM lparam)
{
	m_cImControl.ClearLayer();

	OnPrHomography();

	vector<array<wstring, 2>> vFile;

	FileInDir(m_rDir[0].GetBuffer(), vFile);

	SortingFiles(vFile);

	SetWorkType(eWT_Homography);

	SetHomographyParam(
		vFile, 
		m_cParam.nExtTriggerFrameCount, 
		m_cParam.nPassInStride,
		m_cParam.vFill,
		m_cParam.vWH2,
		m_cParam.M,
		CString2String(m_rDir[1]));

	Reg();

	return 0;
}

void CHomographyDlg::OnTranslating(ImPtr::CImPtr cImPtr)
{
	PostMessage(PQM_CALIBRATOR_TRANSLATING, 0, (LPARAM)new ImPtr::CImPtr(cImPtr));
}

void CHomographyDlg::OnHomography(array<int, 2> vWH2Ex, ImPtr::CImPtr cImPtr)
{
	PostMessage(PQM_CALIBRATOR_HOMOGRAPHY, (WPARAM)new array<int, 2>(vWH2Ex), (LPARAM)new ImPtr::CImPtr(cImPtr));
}

LRESULT CHomographyDlg::OnTranslatingMsg(WPARAM wparam, LPARAM lparam)
{
	ImPtr::CImPtr* lpPtr = (ImPtr::CImPtr*)lparam;

	m_cImControl.ClearLayer();

	m_cImControl.CreateLayer(true, true, false, "", true, 1.0, L"单应性变换", *lpPtr, true, 0.0, 0, 0);

	delete lpPtr;

	return 0;
}

LRESULT CHomographyDlg::OnHomographyMsg(WPARAM wparam, LPARAM lparam)
{
	array<int, 2>* lpWH2Ex = (array<int, 2>*)wparam;

	OnTranslatingMsg(wparam, lparam);

	m_cParam.vWH2Ex = *lpWH2Ex;

	++m_cParam.nType;

	Unreg();

	delete lpWH2Ex;

	PostMessage(PQM_CALIBRATOR_PARAM_CHANGED);

	PostMessage(PQM_CALIBRATOR_TYPE_CHANGED);

	PostMessage(PQM_CALIBRATOR_PRO_REFERENCE);

	return 0;
}
