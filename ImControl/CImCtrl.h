#pragma once
#include <array>
#include <string>
#include <map>
#include <functional>
#include "GDIDBuffer.h"
#include "ImPtr.h"

namespace ImCtrl
{
	union uWParam
	{
		double d;
		long long n;
		unsigned long long w;
	};

	void TrackMouseEvent(bool& bIsTracking, DWORD dwFlags, HWND hwndTrack, DWORD dwTime);

	class CImCtrl : public CStatic, public CGDIDBuffer
	{
		DECLARE_DYNAMIC(CImCtrl)

	protected:
		CImCtrl();
		virtual ~CImCtrl();

	public:
		CPoint GetSpace() { return m_ptSpace; }		// space = 距离上一个控件间距
		CPoint GetResolution() { return m_ptResolution; }	// resolution = 自身大小
		CRect GetChildRect() { return m_rtChild; } // child = 控件在父窗口中位置
		int GetCtrlID() { return GetDlgCtrlID(); }
		CWnd* GetCtrlItem() { return FromHandle(GetSafeHwnd()); }

	public:
		void SetChildRect(CRect rtChild) { m_rtChild = rtChild; }
		void SetSpace(CPoint ptSpace) { m_ptSpace = ptSpace; }
		void SetResolution(CPoint ptResolution) { m_ptResolution = ptResolution; }

	public:
		void SetClickedFunc(int nCtrlId, std::function<void()> fClicked) { m_fClicked.insert(std::pair<int, std::function<void()>>(nCtrlId, fClicked)); }
		void SetChangedFunc(int nCtrlId, std::function<void()> fChanged) { m_fChanged.insert(std::pair<int, std::function<void()>>(nCtrlId, fChanged)); }

	protected:
		void SetCtrlParam(CImCtrl* lpCtrl, array<int, 2> vSpace, array<int, 2> vResolution);
		void CreateCtrl(CImCtrl* lpCtrl, CImCtrl* lpReference, CString rText, unsigned long uStyle, CWnd* lpParentWnd, int nCtrlId);

	private:
		void OnPaintEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth);

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct) {}
		virtual void OnDestroyEx() {}
		virtual void OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth) {}

	private:
		CRect m_rtChild;
		CPoint m_ptSpace;
		CPoint m_ptResolution;
		std::map<int, std::function<void()>> m_fClicked;
		std::map<int, std::function<void()>> m_fChanged;

	private:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnPaint();
		afx_msg void OnDestroy();

	private:
		afx_msg LRESULT OnClickedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnChangedMsg(WPARAM wparam, LPARAM lparam);

		DECLARE_MESSAGE_MAP()
	};

	class CImStatic : public CImCtrl
	{
		DECLARE_DYNAMIC(CImStatic)

	public:
		CImStatic();
		virtual ~CImStatic();

	public:
		void SetIcon(std::wstring strIcon) { m_strIcon = strIcon; }
		void SetTextOrigin(CPoint ptTextOrigin) { m_ptTextOrigin = ptTextOrigin; }
	
	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		virtual void OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth);
		void OnDrawBa(CDC& dc, const CRect& rtCanvas);
		void OnDrawTe(CDC& dc, const CRect& rtCanvas);

	private:
		CPoint m_ptTextOrigin; // 文字输入补偿值  
		std::wstring m_strIcon;
		std::array<CString, 2> m_vText;

	private:
		afx_msg int OnSetText(LPCTSTR);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

		DECLARE_MESSAGE_MAP()
	};

	class CImPreview : public CImCtrl
	{
		DECLARE_DYNAMIC(CImPreview)

	public:
		CImPreview();
		virtual ~CImPreview();

	public:
		void SetImPtr(ImPtr::CImPtr cImPtr);

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		virtual void OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth);

	private:
		ImPtr::CImPtr m_cImPtr;

	private:
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

		DECLARE_MESSAGE_MAP()
	};

	class CImButton : public CImCtrl
	{
		DECLARE_DYNAMIC(CImButton)

	private:
		enum {eBST_Normal = 0, eBST_Hover, eBST_Pushed, eBST_Disable};

	public:
		CImButton();
		virtual ~CImButton();

	public:
		void SetIcon(std::wstring strIconDir);
		void SetTextOrigin(CPoint ptTextOrigin) { m_ptTextOrigin = ptTextOrigin; }
		void SetPng(std::wstring strPngPath);


	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		virtual void OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth);
		void OnDrawBa(CDC& dc, const CRect& rtCanvas);
		void OnDrawTe(CDC& dc, const CRect& rtCanvas);

	private:
		void SetState(int nState);
		int GetState();

	private:
		array<int, 2> m_nState; // cur | last
		bool m_bIsTracking;
		CPoint m_ptTextOrigin; // 文字输入补偿值  
		array<wstring, 4> m_strIcon;
		std::array<CString, 2> m_vText;

	private:
		afx_msg int OnSetText(LPCTSTR);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMouseHover(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
		afx_msg void OnEnable(BOOL bEnable);

		DECLARE_MESSAGE_MAP()
	};

	class CImCheckBox : public CImCtrl
	{
		DECLARE_DYNAMIC(CImCheckBox)

	public:
		CImCheckBox();
		virtual ~CImCheckBox();

	public:
		void SetIcon(std::wstring strIconDir);
		array<int, 2> GetIconSize();

	public:
		void SetCheck(bool bChecked) { SetStatus(bChecked); }
		bool GetCheck() { return m_bChecked[1]; }

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		virtual void OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth);

	private:
		void SetStatus(bool bChecked);

	private:
		std::array<bool,2> m_bChecked;
		std::array<std::wstring,2> m_strIcon;

	private:
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

		DECLARE_MESSAGE_MAP()
	};

	class CImEdit : public CImCtrl
	{
		DECLARE_DYNAMIC(CImEdit)

	private:
		enum {eEST_Normal = 0, eEST_Focus, eEST_Disable}; // EST = edit state type

	public:
		CImEdit();
		virtual ~CImEdit();

	public:
		void SetIcon(wstring strIcon);
		void SetTextOrigin(CPoint ptTextOrigin) { m_ptTextOrigin = ptTextOrigin; }
		void SetWindowTextEx(CString rText);

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		virtual void OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth);
		void OnDrawBa(CDC& dc, const CRect& rtCanvas);
		void OnDrawTe(CDC& dc, const CRect& rtCanvas);
		void OnDrawSe(CDC& dc, const CRect& rtCanvas);
		void OnDrawCa(CDC& dc, const CRect& rtCanvas);

	private: // OnChar - 扩展
		void OnBack();
		void ClearSelected(std::array<int, 2> vInterval, CString& rText);// 光标区间即为选中区域

	private:
		int CharFromPos(CPoint point);
		void OnDrag(UINT nFlags, CPoint point);

	private:
		void SetState(int nState);
		int GetState();

	private:
		array<int, 2> m_nState; // cur | last
		CPoint m_ptCaret;
		CPoint m_ptTextOrigin;
		array<wstring,3> m_strIcon;
		array<CString, 2> m_vText;
		array<int, 5> m_vCaretPos;// 光标位置 // 选中区域 = [0, 1] // 2 = lbdown // 3 = os // 4 = max

	private:
		CBitmap m_bitmapCaret;

	private:
		bool OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		bool OnReturn();

	private:
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	private:
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg int OnSetText(LPCTSTR);
		afx_msg int OnGetText(int, LPTSTR);
		afx_msg void OnSetFocus(CWnd*);
		afx_msg void OnKillFocus(CWnd*);
		afx_msg void OnEnable(BOOL bEnable);

		DECLARE_MESSAGE_MAP()
	};
}
