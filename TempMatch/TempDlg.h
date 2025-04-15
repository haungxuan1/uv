#pragma once
#include "resource.h"
#include "CGDIPlusDlg.h"
#include "opencv.hpp" 
#include <map>
#include <functional>

class CTempDlg : public CGDIPlusDlg
{
	DECLARE_DYNAMIC(CTempDlg)

protected:
	enum { eOT_Begin, eOT_TopLeft, eOT_BottomRight };
	enum { eRT_Frame, eRT_Feature};
	enum { eFFT_l, eFFT_t, eFFT_r, eFFT_b};
	typedef std::tuple<int, int, int, int> FeatureFrame;

public:
	CTempDlg(CWnd* pParent = NULL);
	virtual ~CTempDlg();

	enum { IDD = IDD_MAKE_FEATURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

private:
	virtual void OnDraw(Gdiplus::Bitmap*& lpImage);
	virtual void OnDraw(unsigned char* lpPixel, int nHight, int nWidth) = 0;

private:
	void OnInitGDIPlusImage(const CRect &rtFrame, const CRect &rtFeature);

private:
	void OnDragMove(const CPoint& point);
	void OnDragBegin(const CPoint& point);
	void OnTrace(UINT nFlags);
	void OnMouseMove(CPoint point);
	void OnDragEnd();

protected:
	bool m_bTrace;
	bool m_bLbdown;
	CRect	m_rtFeature[2];
	CPoint m_ptOperation[3];

protected:
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};

