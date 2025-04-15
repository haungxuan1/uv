#pragma once
#include "resource.h"
#include "CameraParam.h"
#include "ThDPI.h"
#include "ImControl2.h"

class CCameraDlg : public CDialog, public CCameraParam, public CThDPI
{
	DECLARE_DYNAMIC(CCameraDlg)

public:
	CCameraDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CCameraDlg();

	enum { IDD = IDD_DLG_CALIBRATOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	int m_nFrameIdx;

protected:
	CString m_rBaseDir;

protected:
	ImControl2::CImControl m_cImControl;

private:
	CEdit m_etFrame;

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg LRESULT OnProCrabMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnProFreezeMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnCaptureMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnGrabMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};