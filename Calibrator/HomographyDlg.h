#pragma once
#include "PrespectiveDlg.h"

class CHomographyDlg : public CPrespectiveDlg
{
	DECLARE_DYNAMIC(CHomographyDlg)

public:
	CHomographyDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CHomographyDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	virtual void OnTranslating(ImPtr::CImPtr cImPtr);
	virtual void OnHomography(array<int, 2> vWH2Ex, ImPtr::CImPtr cImPtr);

private:
	void OnPrHomography();

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg LRESULT OnProHomographyMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnTranslatingMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnHomographyMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};
