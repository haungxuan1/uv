#pragma once
#include "ROIDlg.h"

class CPrespectiveDlg : public CROIDlg
{
	DECLARE_DYNAMIC(CPrespectiveDlg)

public:
	CPrespectiveDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CPrespectiveDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	virtual void OnPerspective(
		array<double, 9> vM,
		array<int, 4> vFill,
		array<float, 2> vImWH2,
		array<Point2f, 4> vVertex2);

private:
	void OnPrPerspective();

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg LRESULT OnProPerspectiveMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnPerspectiveMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};
