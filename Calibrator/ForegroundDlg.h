#pragma once
#include "CameraDlg.h"
#include "ChartCtrl\ChartCtrl.h"
#include "ChartCtrl\ChartLineSerie.h"
#include "ChartCtrl\ChartPointsSerie.h"

class CForegroundDlg : public CCameraDlg
{
	DECLARE_DYNAMIC(CForegroundDlg)

public:
	CForegroundDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CForegroundDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	virtual void OnHistogram(int nRemaining, ImPtr::CImPtr& cImage, ImPtr::CImPtr& cHistogram);
	virtual void OnForeground(int nForeground);

private:
	void OnInitHistogram();
	void OnPrForeground();

protected:
	CString m_rDir[3]; //  ‰»Î |  ‰≥ˆ | ª∫¥Ê

protected:
	CChartCtrl m_cHistogram;
	CChartLineSerie* m_lpHistogram;
	CChartPointsSerie* m_lpSplitPoint;

private:
	CEdit m_etDir[3];
	CEdit m_etBlock;
	CEdit m_etForeground;

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg LRESULT OnProForegroundMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnHistogramMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnForegroundMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};