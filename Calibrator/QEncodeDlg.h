#pragma once
#include "DPIDlg.h"


class CQEncodeDlg : public CDPIDlg
{
	DECLARE_DYNAMIC(CQEncodeDlg)

public:
	CQEncodeDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CQEncodeDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	void QEncode(Mat& cChannel);
	void PrQEncode();

private:
	CButton m_btQEncode;

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg void OnClickedQEncode();

private:
	afx_msg LRESULT OnQEncodeMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};