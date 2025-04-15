#pragma once
#include "Merge2Dlg.h"

class CDPIDlg : public CMerge2Dlg
{
	DECLARE_DYNAMIC(CDPIDlg)

public:
	CDPIDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CDPIDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	virtual void OnDPI(double hor, double ver);

private:
	void OnPrDPI();

private:
	CEdit m_etDPI[4];

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg LRESULT OnProDPIMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnDPIMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};
