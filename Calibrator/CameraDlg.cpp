#include "pch.h"
#include "CameraDlg.h"
#include "PQMsg.h"

IMPLEMENT_DYNAMIC(CCameraDlg, CDialog)

BEGIN_MESSAGE_MAP(CCameraDlg, CDialog)
	ON_MESSAGE(PQM_CALIBRATOR_PRO_CRAB, OnProCrabMsg)
	ON_MESSAGE(PQM_CALIBRATOR_PRO_FREEZE, OnProFreezeMsg)
	ON_MESSAGE(PQM_CAMERA_CAPTURE, OnCaptureMsg)
	ON_MESSAGE(PQM_CAMERA_GRAB, OnGrabMsg)
END_MESSAGE_MAP()

CCameraDlg::CCameraDlg(CString rDir, CWnd* pParent)
	:CDialog(IDD, pParent),
	m_rBaseDir(rDir),
	m_nFrameIdx(0)
{
	int nIdx = m_rBaseDir.ReverseFind(L'\\');

	int nMaxIdx = m_rBaseDir.GetLength() - 1;

	if (nIdx != -1 && nIdx == nMaxIdx)
	{
		m_rBaseDir = m_rBaseDir.Left(nIdx);
	}
}

CCameraDlg::~CCameraDlg()
{

}

void CCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ET_CALIBRATOR_FRAME, m_cParam.nExtTriggerFrameCount);

	DDX_Control(pDX, IDC_ET_CALIBRATOR_FRAME, m_etFrame);
}

BOOL CCameraDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}

void CCameraDlg::OnDisableCtrl()
{
	m_etFrame.EnableWindow(FALSE);
}

void CCameraDlg::OnEnableCtrl()
{
	m_etFrame.EnableWindow(TRUE);
}

LRESULT CCameraDlg::OnProCrabMsg(WPARAM wparam, LPARAM lparam)
{
	UpdateData(TRUE);

	PQCamera().Grab(m_cParam.nExtTriggerFrameCount, GetSafeHwnd());

	return 0;
}

LRESULT CCameraDlg::OnProFreezeMsg(WPARAM wparam, LPARAM lparam)
{
	PQCamera().Freeze(GetSafeHwnd());

	return 0;
}

LRESULT CCameraDlg::OnCaptureMsg(WPARAM wparam, LPARAM lparam)
{
	ImPtr::CImPtr* lpFrame = (ImPtr::CImPtr*)lparam;

	++m_nFrameIdx;

	if (PQPrinter().GetPrinterType() != CPrinterBase::ePT_Calibrator)
	{
		if (!!!((m_nFrameIdx - 1) % m_cParam.nExtTriggerFrameCount)) m_cImControl.ClearLayer();

		ImPtr::CImPtr cIm(new Mat);

		*cIm.Get() = lpFrame->Get()->clone();

		m_cImControl.CreateLayer(true, true, false, "", true, 1.0, to_wstring(m_nFrameIdx).c_str(), cIm, true, 0.0, 0, 0);
	}
	PostMessage(PQM_CALIBRATOR_CAPTURE, (WPARAM)m_nFrameIdx, lparam);

	return 0;
}

LRESULT CCameraDlg::OnGrabMsg(WPARAM wparam, LPARAM lparam)
{
	m_nFrameIdx = 0;

	return 0;
}
