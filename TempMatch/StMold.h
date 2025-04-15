#pragma once
#include <tuple>
#include <string>
#include "GDIDBuffer.h"
#include "opencv.hpp" 

class CStMold : public CWnd, public CGDIDBuffer
{
	DECLARE_DYNAMIC(CStMold)

protected:
	enum { eOT_Begin, eOT_TopLeft, eOT_BottomRight };
	enum { eFFT_l, eFFT_t, eFFT_r, eFFT_b };
	typedef std::tuple<int, int, int, int> FeatureFrame;

protected:
	CStMold();
	virtual ~CStMold();

private:
	void OnDragBegin(const CPoint& point);
	void OnDragEnd();
	void OnTrace(UINT nFlags);
	void OnMouseMove(CPoint point);

protected:
	bool m_bTrace;
	bool m_bLbdown;
	CPoint m_ptOperation[3];

private:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};

