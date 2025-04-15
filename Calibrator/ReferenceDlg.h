#pragma once
#include "ForegroundDlg.h"

class CReferenceDlg : public CForegroundDlg
{
	DECLARE_DYNAMIC(CReferenceDlg)

public:
	CReferenceDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CReferenceDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	virtual void OnReference(Rect& rtRef, ImPtr::CImPtr& cImPtr);

private:
	void OnPrReference();

private:
	CEdit m_etErode;
	CEdit m_etDilate;
	CEdit m_etDensity;
	CEdit m_etGaussian;
	CEdit m_etReference[4];

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg LRESULT OnProReferenceMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnReferenceMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};