#pragma once
#include "afxdialogex.h"
#include "opencv2/opencv.hpp"
#include "../Control/StyleDialog.h"
#include "../Control/StyleButton.h"

#define MAX_POINT_COUNT		500

//class MakeTemplateMaskDialog : public CDialog
class MakeTemplateMaskDialog : public StyleDialog
{
	DECLARE_DYNAMIC(MakeTemplateMaskDialog)

public:
	MakeTemplateMaskDialog(CWnd* pParent, string tTempFile);   // 标准构造函数
	virtual ~MakeTemplateMaskDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAKE_TEMPMASK_DIALOG };
#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* ptMsg);

	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT uFlags, CPoint tPoint);
	afx_msg void OnMouseMove(UINT uFlags, CPoint tPoint);
	afx_msg void OnLButtonUp(UINT uFlags, CPoint tPoint);
	afx_msg void OnRButtonUp(UINT uFlags, CPoint tPoint);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedOk();

public:
	StyleButton m_tResetBtn;
	StyleButton m_tOkBtn;
	StyleButton m_tCancelBtn;

private:
	float m_fScale = 1.0f;
	int m_iMatX = 0;
	int m_iMatY = 0;
	CBrush m_tBrush;
	CRect m_tRect;
	string m_tTempFile;
	CBitmap m_tBmpCanvas;
	CDC	  m_tMemDC;
	Mat m_tDispMat;
	Mat m_tTemplateMat;
	int m_iPointCount;
	CPoint m_tPoints[MAX_POINT_COUNT];
	bool m_bDrawEnable = true;
	bool m_bMousePress = false;

	void createBg();
	void drawPaint(bool bMouseMove = false);
};
