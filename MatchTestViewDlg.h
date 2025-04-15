#pragma once
#include "afxdialogex.h"

using namespace cv;
// MatchTestViewDlg 对话框

class MatchTestViewDlg : public CDialog
{
	DECLARE_DYNAMIC(MatchTestViewDlg)

public:
	MatchTestViewDlg(CWnd* pParent = nullptr,  Mat* mat = nullptr);   // 标准构造函数
	virtual ~MatchTestViewDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MATCHVIEW_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();

private:
	Mat* m_rImage;

	DECLARE_MESSAGE_MAP()
};
