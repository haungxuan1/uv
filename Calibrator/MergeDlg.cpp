#include "pch.h"
#include "MergeDlg.h"
#include "PQMsg.h"
#include "Helper.h"

IMPLEMENT_DYNAMIC(CMergeDlg, CAlignDlg)

BEGIN_MESSAGE_MAP(CMergeDlg, CAlignDlg)
	ON_MESSAGE(PQM_CALIBRATOR_PRO_MERGE, OnProMergeMsg)
	ON_MESSAGE(PQM_CALIBRATOR_MERGING, OnMergingMsg)
	ON_MESSAGE(PQM_CALIBRATOR_MERGE, OnMergeMsg)
END_MESSAGE_MAP()

CMergeDlg::CMergeDlg(CString rDir, CWnd* pParent)
	:CAlignDlg(rDir, pParent)
{

}

CMergeDlg::~CMergeDlg()
{

}

void CMergeDlg::DoDataExchange(CDataExchange* pDX)
{
	CAlignDlg::DoDataExchange(pDX);

}

BOOL CMergeDlg::OnInitDialog()
{
	CAlignDlg::OnInitDialog();

	return TRUE;
}

void CMergeDlg::OnDisableCtrl()
{
	CAlignDlg::OnDisableCtrl();
}

void CMergeDlg::OnEnableCtrl()
{
	CAlignDlg::OnEnableCtrl();
}

void CMergeDlg::OnPrMerge()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir + L"\\camera";

	m_rDir[1] = m_rBaseDir;

	m_rDir[2] = m_rBaseDir + L"\\cache";

	UpdateData(FALSE);

	UpdateData(TRUE);
}

LRESULT CMergeDlg::OnProMergeMsg(WPARAM wparam, LPARAM lparam)
{
	m_cImControl.ClearLayer();

	OnPrMerge();

	vector<array<wstring, 2>> vFile;

	FileInDir(m_rDir[0].GetBuffer(), vFile);

	SortingFiles(vFile);

	SetWorkType(eWT_Merging);

	SetMergingParam(vFile, 
		{
		max(int(ceil(m_cParam.vWH1[0])), int(ceil(m_cParam.vWH2[0]))),
		max(int(ceil(m_cParam.vWH1[1])), int(ceil(m_cParam.vWH2[1]))),
		m_cParam.nExtTriggerFrameCount,
		m_cParam.nPixelInPass,
		m_cParam.nReference[0],
		m_cParam.nReference[1],
		m_cParam.nReference[2],
		m_cParam.nReference[3],},
		m_cParam.nPassInStride,
		CString2String(m_rDir[2] + L"\\Aligned.png"),
		CString2String(m_rDir[1] + L"\\Merged.png"),
		CString2String(m_rDir[1] + L"\\ROIMerged.png"));

	Reg();

	return 0;
}

void CMergeDlg::OnMerging(int nRemaining, ImPtr::CImPtr cImPtr)
{
	PostMessage(PQM_CALIBRATOR_MERGING, (WPARAM)nRemaining, (LPARAM)new ImPtr::CImPtr(cImPtr));
}

void CMergeDlg::OnMerge(int nRemaining, ImPtr::CImPtr cImPtr)
{
	PostMessage(PQM_CALIBRATOR_MERGE, (WPARAM)nRemaining, (LPARAM)new ImPtr::CImPtr(cImPtr));
}

LRESULT CMergeDlg::OnMergingMsg(WPARAM wparam, LPARAM lparam)
{
	int nRemaining = (int)wparam;

	ImPtr::CImPtr* lpPtr = (ImPtr::CImPtr*)lparam;

	m_cImControl.ClearLayer();

	m_cImControl.CreateLayer(true, true, false, "", true, 1.0, WString2CString(to_wstring(nRemaining)), *lpPtr, true, 0.0, 0, 0);

	delete lpPtr;

	return 0;
}

LRESULT CMergeDlg::OnMergeMsg(WPARAM wparam, LPARAM lparam)
{
	OnMergingMsg(wparam, lparam);

	++m_cParam.nType;

	Unreg();

	PostMessage(PQM_CALIBRATOR_PARAM_CHANGED);

	PostMessage(PQM_CALIBRATOR_TYPE_CHANGED);

	PostMessage(PQM_CALIBRATOR_PRO_ROI);

	return 0;
}
