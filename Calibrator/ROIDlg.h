#pragma once
#include "MergeDlg.h"

class CROIDlg : public CMergeDlg
{
	DECLARE_DYNAMIC(CROIDlg)

public:
	CROIDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CROIDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	virtual void OnROI(ImPtr::CImPtr cImPtr, array<Point2f, 4> vVertex);

private:
	void OnPrROI();

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg LRESULT OnProROIMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnROIMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};
