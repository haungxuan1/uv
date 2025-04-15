#pragma once
#include "AlignDlg.h"

class CMergeDlg : public CAlignDlg
{
	DECLARE_DYNAMIC(CMergeDlg)

public:
	CMergeDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CMergeDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	virtual void OnMerging(int nRemaining, ImPtr::CImPtr cImPtr);
	virtual void OnMerge(int nRemaining, ImPtr::CImPtr cImPtr);

private:
	void OnPrMerge();

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg LRESULT OnProMergeMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMergingMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMergeMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};
