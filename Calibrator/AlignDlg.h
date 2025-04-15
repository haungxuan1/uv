#pragma once
#include "ReferenceDlg.h"

class CAlignDlg : public CReferenceDlg
{
	DECLARE_DYNAMIC(CAlignDlg)

public:
	CAlignDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CAlignDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	virtual void OnAligning(int nRemaining, double dScore, ImPtr::CImPtr cImPtr);
	virtual void OnAlign(int nRemaining, double dScore, ImPtr::CImPtr cImPtr, int nPassInStride);

private:
	void OnPrAlign();

private:
	CEdit m_etStride;
	CEdit m_etImFrame[4];
	CEdit m_etPassInStride;

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg LRESULT OnProAlignMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnAligningMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnAlignMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};
