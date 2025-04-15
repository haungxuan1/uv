#pragma once
#include "HomographyDlg.h"

class CMerge2Dlg : public CHomographyDlg
{
	DECLARE_DYNAMIC(CMerge2Dlg)

public:
	CMerge2Dlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CMerge2Dlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	virtual void OnMergingEx(ImPtr::CImPtr cImPtr);
	virtual void OnMergeEx(array<double, 2>& vAlign, ImPtr::CImPtr cImPtr);

private:
	void OnPrMerge2();

private:
	CEdit m_etAssessment;

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg LRESULT OnProMerge2Msg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMerging2Msg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMerge2Msg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};
