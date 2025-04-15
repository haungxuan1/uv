#pragma once
#include "afxdialogex.h"

// StepDialog 对话框

class StepDialog : public CDialog
{
	DECLARE_DYNAMIC(StepDialog)

public:
	StepDialog(int iMax);   // 标准构造函数
	virtual ~StepDialog();

	void setStepText(CString tStepText);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STEP_DIALOG };
#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg BOOL OnEraseBkgnd(CDC* ptDC);
	afx_msg HBRUSH OnCtlColor(CDC* ptDC, CWnd* ptWnd, UINT uCtlColor);
	

	DECLARE_MESSAGE_MAP()

protected:
	void DoPaint(CDC *ptDC);

private:
	int m_iMax;
	int m_iPos;

	CString m_tText;
	CBitmap m_tBitmapBg;
	CBitmap m_tBitmapBuf;
	CDC		m_tMemDC1;
	CDC		m_tMemDC2;

	void drawFragment(CDC* ptDC, CRect* ptRect, int iTotal, int iOffset, int iLength, COLORREF uClrFill, bool b3D);
	COLORREF calculateColor(COLORREF uClrFg, COLORREF uClrBg, int iAlpha);
};
