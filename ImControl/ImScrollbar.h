#pragma once
#include <string>
#include "CImCtrl.h"

namespace ImScrollbar
{
	class CThumb : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CThumb)

	public:
		CThumb(bool bVer, int nMinThumb);
		virtual ~CThumb();

	public:
		void SetIcon(COLORREF crNormal, COLORREF crPushed);

	public:
		int GetMinThumb() { return m_nMinThumb; }

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	private:
		void SetStatus(bool bPushed);
		bool IsPushed();
		void OnDrag(UINT nFlags, CPoint point);

	private:
		std::array<bool,2> m_bStatus; // cur | last // 0 = normal | 1 = pushed
		bool m_bVer;
		int m_nMinThumb;
		bool m_bIsTracking;
		CPoint m_ptButtonDown;
		std::array<COLORREF,2> m_crColor;

	private:
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnMouseHover(UINT nFlags, CPoint point);
		afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

		DECLARE_MESSAGE_MAP()
	};

	class CChute :public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CChute)

	public:
		CChute(bool bVer);
		virtual ~CChute();

	public:
		void SetIcon(std::wstring strIconDir);
		void OnScalarChanged(int nField, int nView);
		double GetRatio();

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		void OnInitCtrl(const int& cx, const int& cy);
		void OnInitCtrl();

	private:
		void OnDrag(int os);

	private:
		void OnHold(bool bPushed, CPoint point);

	private:
		void OnClickedLineUp();
		void OnClickedLineDown();
		void OnClickedPageUp();
		void OnClickedPageDown();

	private:
		bool m_bVer; // 0 hor | 1 ver
		bool m_bIsTracking;
		std::array<double, 2> m_vRatio;

	private:
		ImCtrl::CImButton m_btLineUp;
		ImCtrl::CImButton m_btLineDown;
		ImCtrl::CImButton m_btPageUp;
		ImCtrl::CImButton m_btPageDown;
		CThumb m_cThumb;

	private:
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnMouseHover(UINT nFlags, CPoint point);
		afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	private:
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

		DECLARE_MESSAGE_MAP()
	};

	class CImScrollbar : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CImScrollbar)

	public:
		CImScrollbar(bool bVer);
		virtual ~CImScrollbar();

	public:
		void SetIcon(wstring strIconDir);

	public:
		void OnScalarChanged(int nField, int nView);
		double GetRatio();

	private:
		void OnInitCtrl(const int& cx, const int& cy);
		void OnInitCtrl();

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	private:
		bool m_bVer; // 0 hor | 1 ver
		CChute m_cChute;

	private:
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

		DECLARE_MESSAGE_MAP()
	};
}



