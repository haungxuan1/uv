#pragma once
#include <map>
#include <tuple>
#include <gdiplus.h>

class CGDIPlusDlg : public CDialog
{
	DECLARE_DYNAMIC(CGDIPlusDlg)

public:
	CGDIPlusDlg(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CGDIPlusDlg();

private:
	void CreateFakeWnd();
	void DestoryFakeWnd();
	void HookControlUpdate(HWND hWnd);
	void UnhookControlUpdate(HWND hWnd);
	void Refresh();
	void DrawCtrl(Gdiplus::Graphics& graphics, HDC hDC, HWND hWnd);
	void DrawCaret(Gdiplus::Graphics& graph);

private:
	Gdiplus::Bitmap* InitImage();

private:
	virtual void OnDraw(Gdiplus::Bitmap*& lpImage) = 0;
	virtual void OnDraw(Gdiplus::Graphics& graph) = 0;

protected:
	Gdiplus::Bitmap*		m_lpImage;
	HWND					m_hFakeWnd;
	CString					m_strWndClassName;
	BOOL					m_bIsRefreshing;
	int						m_nWidth;
	int						m_nHeigh;
	int						m_nAlpha;
	static std::map<HWND, std::tuple<WNDPROC, CGDIPlusDlg*>> m_vCtrl;

protected:
	virtual BOOL OnInitDialog();
	static LRESULT CALLBACK CtrlWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

protected:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};

