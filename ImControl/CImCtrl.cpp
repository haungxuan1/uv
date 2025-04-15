#include "pch.h"
#include "CImCtrl.h"
#include "DrawPng.h"
#include "PQMsg.h"
#include "Transform.h"
#include <iostream>
#include <Imm.h>
using namespace std;

namespace ImCtrl
{
	void TrackMouseEvent(bool& bIsTracking, DWORD dwFlags, HWND hwndTrack, DWORD dwTime)
	{
		if (bIsTracking) return;

		bIsTracking = true;
		TRACKMOUSEEVENT tme = { 0 };
		tme.cbSize = sizeof(tme);
		tme.dwFlags = dwFlags;
		tme.hwndTrack = hwndTrack;
		tme.dwHoverTime = dwTime;
		::TrackMouseEvent(&tme);
	}
	/********************************************************************************/
	IMPLEMENT_DYNAMIC(CImCtrl, CStatic)

	BEGIN_MESSAGE_MAP(CImCtrl, CStatic)
		ON_WM_CREATE()
		ON_WM_PAINT()
		ON_WM_DESTROY()
		ON_MESSAGE(PQM_IM_CTRL_CLICKED, OnClickedMsg)
		ON_MESSAGE(PQM_IM_CTRL_CHANGED, OnChangedMsg)
	END_MESSAGE_MAP()
	CImCtrl::CImCtrl() :
		m_ptSpace{ 0,0 }, m_ptResolution{ 0,0 }, m_rtChild{ 0,0,0,0 }
	{

	}

	CImCtrl::~CImCtrl()
	{

	}

	int CImCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CStatic::OnCreate(lpCreateStruct) == -1) return -1;

		m_rtChild = CRect(lpCreateStruct->x, lpCreateStruct->y,
			lpCreateStruct->x + lpCreateStruct->cx, lpCreateStruct->y + lpCreateStruct->cy);

		InitCanvas(this, CRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy));

		OnCreateEx(lpCreateStruct);

		return 0;
	}

	void CImCtrl::OnPaint()
	{
		CPaintDC dc(this);

		OnDraw3(&dc, bind(&CImCtrl::OnPaintEx, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4));
	}

	void CImCtrl::OnPaintEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth)
	{
		OnDrawEx(dc, rtCanvas, lpPixel, biWidth);
	}

	void CImCtrl::OnDestroy()
	{
		CStatic::OnDestroy();

		OnDestroyEx();
	}

	LRESULT CImCtrl::OnClickedMsg(WPARAM wparam, LPARAM lparam)
	{
		int nCtrlId = (int)lparam;

		auto iCtrl = m_fClicked.find(nCtrlId);

		if (iCtrl != m_fClicked.end()) iCtrl->second();

		return 0;
	}

	LRESULT CImCtrl::OnChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		int nCtrlId = (int)lparam;

		auto iCtrl = m_fChanged.find(nCtrlId);

		if (iCtrl != m_fChanged.end()) iCtrl->second();

		return 0;
	}

	void CImCtrl::SetCtrlParam(CImCtrl* lpCtrl, array<int, 2> vSpace, array<int, 2> vResolution)
	{
		lpCtrl->SetSpace({ vSpace[0], vSpace[1] });

		lpCtrl->SetResolution({ vResolution[0], vResolution[1] });
	}

	void CImCtrl::CreateCtrl(CImCtrl* lpCtrl, CImCtrl* lpReference, CString rText, unsigned long uStyle, CWnd* lpParentWnd, int nCtrlId)
	{
		CRect rtCtrl;

		rtCtrl = (lpReference ? lpReference->GetChildRect(): CRect(0, 0, 0, 0));
		rtCtrl.left += lpCtrl->GetSpace().x;
		rtCtrl.top += lpCtrl->GetSpace().y;
		rtCtrl.right = rtCtrl.left + lpCtrl->GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + lpCtrl->GetResolution().y;
		lpCtrl->Create(rText, uStyle, rtCtrl, lpParentWnd, nCtrlId);
	}
	/********************************************************************************/
	IMPLEMENT_DYNAMIC(CImStatic, CImCtrl)

	BEGIN_MESSAGE_MAP(CImStatic, CImCtrl)
		ON_WM_SETTEXT()
		ON_WM_LBUTTONDOWN()
	END_MESSAGE_MAP()

	CImStatic::CImStatic() :
		m_ptTextOrigin{0,0}, m_strIcon(L""), m_vText{ L"", L"" }
	{

	}

	CImStatic::~CImStatic()
	{

	}

	void CImStatic::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(RGB(34, 41, 48));

		m_vText[1] = m_vText[0] = lpCreateStruct->lpszName;
	}

	int CImStatic::OnSetText(LPCTSTR szText)
	{
		m_vText[0] = szText;

		if (m_vText[0] == m_vText[1]) return FALSE;

		m_vText[1] = m_vText[0];

		Invalidate();

		return TRUE;
	}

	void CImStatic::OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth)
	{
		OnDrawBa(dc, rtCanvas);

		OnDrawTe(dc, rtCanvas);
	}

	void CImStatic::OnDrawBa(CDC& dc, const CRect& rtCanvas)
	{
		if (m_strIcon.empty()) return;

		DrawPng::DrawPng(dc.GetSafeHdc(), rtCanvas, m_strIcon);
	}

	void CImStatic::OnDrawTe(CDC& dc, const CRect& rtCanvas)
	{
		if (m_vText[1].IsEmpty()) return;

		CRect rtTextPos(rtCanvas);

		rtTextPos.left += m_ptTextOrigin.x;

		rtTextPos.top += m_ptTextOrigin.y;

		dc.DrawText(m_vText[1], rtTextPos, m_uTxFormat);
	}

	void CImStatic::OnLButtonDown(UINT nFlags, CPoint point)
	{
		::PostMessage(GetParent()->GetSafeHwnd(),
			WM_LBUTTONDOWN,
			MAKELONG(GetCtrlID(), nFlags),
			MAKELONG(point.x, point.y));
	}
	/********************************************************************************/
	IMPLEMENT_DYNAMIC(CImPreview, CImCtrl)

	BEGIN_MESSAGE_MAP(CImPreview, CImCtrl)
		ON_WM_LBUTTONDOWN()
	END_MESSAGE_MAP()

	CImPreview::CImPreview()
	{

	}

	CImPreview::~CImPreview()
	{

	}

	void CImPreview::SetImPtr(ImPtr::CImPtr cImPtr)
	{
		m_cImPtr = cImPtr;

		GetSafeHwnd() ? Invalidate() : []() {}();
	}

	void CImPreview::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(RGB(34, 41, 48));
	}

	void CImPreview::OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth)
	{
		if (m_cImPtr.Empty()) return;

		auto lpIm = m_cImPtr.Get();

		int* lp = new int;

		for (uint64 y = 0; y < min(lpIm->rows, rtCanvas.Height()); ++y)
		{
			for (uint64 x = 0; x < min(lpIm->cols, rtCanvas.Width()); ++x)
			{
				for (uint64 c = 0; c < 4; ++c)
				{
					lpPixel[(y * biWidth + x) * 4 + c] = lpIm->data[(y * lpIm->cols + x) * 4 + c];
				}
			}
		}
		lpPixel = UpdatePixel(lpPixel);
	}

	void CImPreview::OnLButtonDown(UINT nFlags, CPoint point)
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_CTRL_CLICKED, 0, (LPARAM)GetCtrlID());
	}
	/********************************************************************************/
	IMPLEMENT_DYNAMIC(CImButton, CImCtrl)

	BEGIN_MESSAGE_MAP(CImButton, CImCtrl)
		ON_WM_ENABLE()
		ON_WM_SETTEXT()
		ON_WM_LBUTTONUP()
		ON_WM_LBUTTONDOWN()
		ON_WM_MOUSEMOVE()
		ON_WM_MOUSEHOVER()
		ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	END_MESSAGE_MAP()

	CImButton::CImButton() :
		m_bIsTracking(false), m_nState{ eBST_Normal, eBST_Normal },
		m_ptTextOrigin{0,0},
		m_strIcon{L"", L""}, m_vText{ L"", L"" }
	{

	}

	CImButton::~CImButton()
	{

	}

	void CImButton::SetIcon(std::wstring strIconDir)
	{
		m_strIcon[eBST_Normal] = strIconDir + L"\\Normal.png";

		m_strIcon[eBST_Hover] = strIconDir + L"\\Hover.png";

		m_strIcon[eBST_Pushed] = strIconDir + L"\\Pushed.png";

		m_strIcon[eBST_Disable] = strIconDir + L"\\Disable.png";
	}

	void CImButton::SetPng(std::wstring strPngPath)
	{

		if (!strPngPath.empty())
		{
			std::wstring sub_str = strPngPath.substr(0, strPngPath.length() - 4);

//			sub_str += L"_temp.png";

			sub_str = L"Pic_temp.png";

			cv::Mat image = cv::imread(Transform::Wstring2String(strPngPath), cv::IMREAD_UNCHANGED);

			if (!image.empty())
			{
				cv::Mat resizedImage;

				cv::Size newSize(800, 600); // 新的宽度和高度

				cv::resize(image, resizedImage, newSize, 0, 0, cv::INTER_LINEAR);

				cv::imwrite(Transform::Wstring2String(sub_str), resizedImage);

				m_strIcon[eBST_Normal] = sub_str;

				m_strIcon[eBST_Hover] = sub_str;

				m_strIcon[eBST_Pushed] = sub_str;

				m_strIcon[eBST_Disable] = sub_str;
			}
		}
	}

	inline void CImButton::SetState(int nState)
	{
		m_nState[0] = nState;

		if (m_nState[0] == m_nState[1]) return;

		m_nState[1] = m_nState[0];

		Invalidate();
	}

	inline int CImButton::GetState()
	{
		return m_nState[0];
	}

	void CImButton::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		m_vText[1] = m_vText[0] = lpCreateStruct->lpszName;
	}

	void CImButton::OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth)
	{
		OnDrawBa(dc, rtCanvas);

		OnDrawTe(dc, rtCanvas);
	}

	void CImButton::OnDrawBa(CDC& dc, const CRect& rtCanvas)
	{
		if (m_strIcon[GetState()].empty()) return;

		DrawPng::DrawPng(dc.GetSafeHdc(), rtCanvas, m_strIcon[GetState()]);
	}

	void CImButton::OnDrawTe(CDC& dc, const CRect& rtCanvas)
	{
		if (m_vText[0].IsEmpty()) return;

		CRect rtTextPos(rtCanvas);

		rtTextPos.left += m_ptTextOrigin.x;

		rtTextPos.top += m_ptTextOrigin.y;

		dc.DrawText(m_vText[0], rtTextPos, m_uTxFormat);
	}

	int CImButton::OnSetText(LPCTSTR szText)
	{
		m_vText[0] = szText;

		if (m_vText[0] == m_vText[1]) return FALSE;

		m_vText[1] = m_vText[0];

		Invalidate();

		return TRUE;
	}

	void CImButton::OnEnable(BOOL bEnable)
	{
		SetState(bEnable ? eBST_Normal : eBST_Disable);

		CImCtrl::OnEnable(bEnable);
	}

	void CImButton::OnLButtonUp(UINT nFlags, CPoint point)
	{
		SetState(eBST_Hover);
	}

	void CImButton::OnLButtonDown(UINT nFlags, CPoint point)
	{
		SetState(eBST_Pushed);

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_CTRL_CLICKED, 0, (LPARAM)GetCtrlID());
	}

	void CImButton::OnMouseMove(UINT nFlags, CPoint point)
	{
		SetState((nFlags & MK_LBUTTON) ? eBST_Pushed : eBST_Hover);

		TrackMouseEvent(m_bIsTracking, TME_LEAVE | TME_HOVER, GetSafeHwnd(), 100);
	}

	void CImButton::OnMouseHover(UINT nFlags, CPoint point)
	{
		SetState((nFlags & MK_LBUTTON) ? eBST_Pushed : eBST_Hover);

		if (nFlags & MK_LBUTTON)
			::SendMessage(GetParent()->GetSafeHwnd(), PQM_IM_CTRL_CLICKED, 0, (LPARAM)GetCtrlID());

		m_bIsTracking = false;

		TrackMouseEvent(m_bIsTracking, TME_LEAVE | TME_HOVER, GetSafeHwnd(), 100);
	}

	LRESULT CImButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
	{
		SetState(eBST_Normal);

		m_bIsTracking = false;

		return 0;
	}
	/********************************************************************************/
	IMPLEMENT_DYNAMIC(CImCheckBox, CImCtrl)

	BEGIN_MESSAGE_MAP(CImCheckBox, CImCtrl)
		ON_WM_LBUTTONDOWN()
	END_MESSAGE_MAP()

	CImCheckBox::CImCheckBox() :
		m_bChecked{ true, true }, m_strIcon{L"", L""}
	{

	}

	CImCheckBox::~CImCheckBox()
	{

	}

	void CImCheckBox::SetIcon(std::wstring strIconDir)
	{
		m_strIcon[0] = strIconDir + L"\\Unchecked.png";

		m_strIcon[1] = strIconDir + L"\\Checked.png";
	}

	array<int, 2> CImCheckBox::GetIconSize()
	{
		Mat cChecked = imread(Transform::Wstring2String(m_strIcon[1]));

		Mat cUnchecked = imread(Transform::Wstring2String(m_strIcon[0]));

		return { max(cChecked.cols, cUnchecked.cols), max(cChecked.rows,cUnchecked.rows) };
	}

	void CImCheckBox::SetStatus(bool bChecked)
	{
		m_bChecked[0] = bChecked;

		if (m_bChecked[0] == m_bChecked[1]) return;

		m_bChecked[1] = m_bChecked[0];

		Invalidate();
	}

	void CImCheckBox::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(RGB(34, 41, 48));
	}

	void CImCheckBox::OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth)
	{
		if (m_strIcon[GetCheck()].empty()) return;

		DrawPng::DrawPng(dc.GetSafeHdc(), rtCanvas, m_strIcon[GetCheck()]);
	}

	void CImCheckBox::OnLButtonDown(UINT nFlags, CPoint point)
	{
		SetStatus(!GetCheck());

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_CTRL_CLICKED, 0, (LPARAM)GetCtrlID());
	}
	/********************************************************************************/
	IMPLEMENT_DYNAMIC(CImEdit, CImCtrl)

	BEGIN_MESSAGE_MAP(CImEdit, CImCtrl)
		ON_WM_LBUTTONDOWN()
		ON_WM_MOUSEMOVE()
		ON_WM_SETTEXT()
		ON_WM_GETTEXT()
		ON_WM_SETFOCUS()
		ON_WM_KILLFOCUS()
		ON_WM_ENABLE()
	END_MESSAGE_MAP()

	CImEdit::CImEdit() :
		m_nState{ eEST_Normal, eEST_Normal },
		m_ptCaret(1, 10), m_ptTextOrigin(10, 0),
		m_vText{L"", L""}, m_vCaretPos{ 0,0,0,0,10 }
	{

	}

	CImEdit::~CImEdit()
	{

	}

	void CImEdit::SetIcon(wstring strIcon)
	{ 
		m_strIcon[eEST_Normal] = strIcon + L"\\Normal.png";

		m_strIcon[eEST_Focus] = strIcon + L"\\Focus.png";

		m_strIcon[eEST_Disable] = strIcon + L"\\Disable.png";
	}

	void CImEdit::SetState(int nState)
	{
		m_nState[0] = nState;

		if (m_nState[0] == m_nState[1]) return;

		m_nState[1] = m_nState[0];

		Invalidate();
	}

	int CImEdit::GetState()
	{
		return m_nState[0];
	}

	void CImEdit::SetWindowTextEx(CString rText)
	{
		UpdateData(FALSE);

		SetWindowText(rText);

		UpdateData(FALSE);
	}

	void CImEdit::OnEnable(BOOL bEnable)
	{
		SetState(bEnable ? eEST_Normal : eEST_Disable);

		CImCtrl::OnEnable(bEnable);
	}

	void CImEdit::OnSetFocus(CWnd* lpOldWnd)
	{
		HIMC  hImc = ImmGetContext(GetSafeHwnd());

		!!hImc ? ImmAssociateContext(GetSafeHwnd(), NULL) : []()->HIMC {return nullptr; }();

		ImmReleaseContext(GetSafeHwnd(), hImc);

		SetState(eEST_Focus);

		Invalidate();

		CImCtrl::OnSetFocus(lpOldWnd);
	}

	void CImEdit::OnKillFocus(CWnd* lpNewWnd)
	{
		HideCaret();

		::DestroyCaret();

		SetState(eEST_Normal);

		Invalidate();

		CImCtrl::OnKillFocus(lpNewWnd);
	}

	void CImEdit::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		// 创建光标
		SetTextColor(0);

		m_bitmapCaret.CreateCompatibleBitmap(&m_dcCanvas, m_ptCaret.x, m_ptCaret.y);

		CBitmap* lpBitmap = m_dcCanvas.SelectObject(&m_bitmapCaret);

		SetBackdrop(0xffffff);

		m_dcCanvas.FillRect(CRect(0, 0, m_ptCaret.x, m_ptCaret.y), &m_brCanvas);

		m_dcCanvas.SelectObject(lpBitmap);
	}

	int CImEdit::OnSetText(LPCTSTR szName)
	{
		m_vText[0] = szName;

		if (m_vText[0] == m_vText[1]) return FALSE;

		m_vText[1] = m_vText[0];

		m_vCaretPos[4] = m_vText[1].GetLength();

		Invalidate();

		return TRUE;
	}

	int CImEdit::OnGetText(int nSize, LPTSTR szbuf)
	{
		if (nSize <= (m_vText[1].GetLength() * sizeof(wchar_t))) return FALSE;

		memcpy(szbuf, m_vText[1].GetBuffer(), m_vText[1].GetLength() * sizeof(wchar_t));

		return TRUE;
	}

	void CImEdit::OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth)
	{
		// 绘制背景
		OnDrawBa(dc, rtCanvas);
		// 绘制选中区
		OnDrawSe(dc, rtCanvas);
		// 绘制文字
		OnDrawTe(dc, rtCanvas);
		// 绘制光标
		OnDrawCa(dc, rtCanvas);
	}

	void CImEdit::OnDrawBa(CDC& dc, const CRect& rtCanvas)
	{
		if (m_strIcon[GetState()].empty()) return;

		DrawPng::DrawPng(dc.GetSafeHdc(), rtCanvas, m_strIcon[GetState()]);
	}

	void CImEdit::OnDrawSe(CDC& dc, const CRect& rtCanvas)
	{
		if (m_vCaretPos[0] == m_vCaretPos[1]) return;

		CRect rtSolid;

		CString rLeft = m_vText[1].Left(m_vCaretPos[0]);

		CString rRight = m_vText[1].Left(m_vCaretPos[1]);

		rtSolid.left = m_ptTextOrigin.x + dc.GetTextExtent(rLeft).cx;

		rtSolid.right = m_ptTextOrigin.x + dc.GetTextExtent(rRight).cx;

		rtSolid.top = (rtCanvas.Height() - m_ptCaret.y) * 0.5;

		rtSolid.bottom = rtCanvas.Height() - rtSolid.top;

		dc.FillSolidRect(rtSolid, RGB(136, 186, 214));
	}

	void CImEdit::OnDrawTe(CDC& dc, const CRect& rtCanvas)
	{
		if (m_vText[1].IsEmpty()) return;

		CRect rtTextPos(rtCanvas);

		rtTextPos.left += m_ptTextOrigin.x;

		rtTextPos.top += m_ptTextOrigin.y;

		dc.DrawText(m_vText[1], rtTextPos, m_uTxFormat);
	}

	void CImEdit::OnDrawCa(CDC& dc, const CRect& rtCanvas)
	{
		if (GetCtrlItem() != GetFocus()) return;

		CPoint ptCaretPos;

		CString rLeft = m_vText[1].Left(m_vCaretPos[0]);

		ptCaretPos.x = m_ptTextOrigin.x + dc.GetTextExtent(rLeft).cx;

		ptCaretPos.y = rtCanvas.top + (rtCanvas.Height() - m_ptCaret.y) * 0.5;

		CreateCaret(&m_bitmapCaret);

		SetCaretPos(ptCaretPos);
	}

	BOOL CImEdit::PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->message == WM_CHAR) return OnChar(pMsg->wParam, 1, 0);

		else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return OnReturn();

		else if (pMsg->message == WM_KEYDOWN && OnKeyDown(pMsg->wParam, 1, 0)) return true;

		return CImCtrl::PreTranslateMessage(pMsg);
	}

	bool CImEdit::OnReturn()
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_CTRL_CHANGED, 0, (LPARAM)GetCtrlID());

		return true;
	}

	bool CImEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		// 鼠标左键按下，则不响应
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) return false;

		// 仅处理方向键
		if ((nChar < VK_LEFT) || (VK_DOWN < nChar)) return false;

		nChar = (nChar != VK_UP ? nChar : VK_LEFT);

		nChar = (nChar != VK_DOWN ? nChar : VK_RIGHT);

		bool bShift = (GetKeyState(VK_SHIFT) & 0x8000);

		m_vCaretPos[3] += (nChar == VK_LEFT ? (-1) : 1);

		m_vCaretPos[3] = max(m_vCaretPos[3], 0);

		m_vCaretPos[3] = min(m_vCaretPos[3], m_vCaretPos[4]);

		m_vCaretPos[2] = (bShift ? m_vCaretPos[2] : m_vCaretPos[3]);

		m_vCaretPos[0] = (bShift ? min(m_vCaretPos[2], m_vCaretPos[3]) : m_vCaretPos[3]);

		m_vCaretPos[1] = (bShift ? max(m_vCaretPos[2], m_vCaretPos[3]) : m_vCaretPos[3]);

		Invalidate();

		return true;
	}

	bool CImEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		// 鼠标左键按下，则不响应
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) return false;

		if (nChar == VK_BACK) { OnBack(); return true; }

		CString rCache(m_vText[1]);

		ClearSelected({ m_vCaretPos[0], m_vCaretPos[1] }, rCache);

		rCache.Insert(m_vCaretPos[0], (wchar_t)nChar);

		++m_vCaretPos[0];

		m_vCaretPos[3] = m_vCaretPos[2] = m_vCaretPos[1] = m_vCaretPos[0];

		SetWindowTextEx(rCache);

		return true;
	}

	void CImEdit::OnBack()
	{
		CString rCache(m_vText[1]);

		// 有选中区域，则删除选中
		m_vCaretPos[0] -= (m_vCaretPos[1] == m_vCaretPos[0]);

		m_vCaretPos[0] = max(m_vCaretPos[0], 0);

		ClearSelected({ m_vCaretPos[0], m_vCaretPos[1] }, rCache);

		m_vCaretPos[3] = m_vCaretPos[2] = m_vCaretPos[1] = m_vCaretPos[0];

		SetWindowTextEx(rCache);
	}

	void CImEdit::ClearSelected(std::array<int, 2> vInterval, CString& rText)
	{
		if (vInterval[0] == vInterval[1]) return; // 不存在区间

		rText.Delete(vInterval[0], vInterval[1] - vInterval[0]);
	}

	// 实现cedit::CharFromPos
	int CImEdit::CharFromPos(CPoint point)
	{
		// 若小于初始范围，则移动光标至首
		if (point.x <= m_ptTextOrigin.x) return 0;

		// 大于有效内容末尾，则移动光标至尾
		if ((m_ptTextOrigin.x + m_dcCanvas.GetTextExtent(m_vText[1]).cx) < point.x) return m_vText[1].GetLength();

		// 找出有效区间，返回临近位 
		int i = 0, nPrev = m_ptTextOrigin.x, nNext = m_ptTextOrigin.x;

		do
		{
			nPrev = nNext;

			nNext = m_ptTextOrigin.x + m_dcCanvas.GetTextExtent(m_vText[1].Left(i++)).cx;
		} while (nNext < point.x);

		return (point.x - nPrev) < (nNext - point.x) ? (i - 2) : (i - 1);
	}

	void CImEdit::OnLButtonDown(UINT nFlags, CPoint point)
	{
		SetFocus();

		// 移动光标
		m_vCaretPos[3] = m_vCaretPos[2] = m_vCaretPos[1] = m_vCaretPos[0] = CharFromPos(point);

		Invalidate();

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_CTRL_CLICKED, 0, (LPARAM)GetCtrlID());
	}

	void CImEdit::OnDrag(UINT nFlags, CPoint point)
	{
		if (!(nFlags & MK_LBUTTON)) return;

		m_vCaretPos[3] = CharFromPos(point);

		m_vCaretPos[0] = min(m_vCaretPos[2], m_vCaretPos[3]);

		m_vCaretPos[1] = max(m_vCaretPos[2], m_vCaretPos[3]);

		Invalidate();
	}

	void CImEdit::OnMouseMove(UINT nFlags, CPoint point)
	{
		OnDrag(nFlags, point);
	}
}
