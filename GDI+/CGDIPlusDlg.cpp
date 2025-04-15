#include "pch.h"
#include "CGDIPlusDlg.h"
#pragma comment (lib, "Gdiplus.lib")

std::map<HWND, std::tuple<WNDPROC, CGDIPlusDlg*>> CGDIPlusDlg::m_vCtrl;

IMPLEMENT_DYNAMIC(CGDIPlusDlg, CDialog)

BEGIN_MESSAGE_MAP(CGDIPlusDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATE()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CGDIPlusDlg::CGDIPlusDlg(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	m_nAlpha = 0xff;

	m_bIsRefreshing = false;

	m_strWndClassName.Format(_T("FakeWnd%X"), GetTickCount());
}

CGDIPlusDlg::~CGDIPlusDlg()
{
	if (!!m_lpImage) delete m_lpImage;

	m_lpImage = nullptr;
}

Gdiplus::Bitmap* CGDIPlusDlg::InitImage()
{
	CRect rtClient;

	GetClientRect(rtClient);

	Gdiplus::BitmapData data;

	Gdiplus::Bitmap* image = 
		new Gdiplus::Bitmap(
		rtClient.Width(), 
		rtClient.Height(), 
		PixelFormat32bppARGB);

	Gdiplus::Rect rc(0, 0, image->GetWidth(), image->GetHeight());

	image->LockBits(&rc, Gdiplus::ImageLockModeWrite, image->GetPixelFormat(), &data);

	unsigned char* pixels = (unsigned char*)data.Scan0;

	int byteCount = data.Stride * data.Height;

	memset(pixels, 0xf0, byteCount);

	image->UnlockBits(&data);

	return image;
}

BOOL CGDIPlusDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lpImage = InitImage();

	m_nWidth = m_lpImage->GetWidth();

	m_nHeigh = m_lpImage->GetHeight();

	::SetWindowPos(GetSafeHwnd()
		, NULL
		, 0
		, 0
		, m_nWidth
		, m_nHeigh
		, SWP_NOMOVE | SWP_NOZORDER
	);

	CreateFakeWnd();

	::SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	BYTE bTran = 0xff; 0x5;

	::SetLayeredWindowAttributes(m_hWnd, 0, bTran, LWA_ALPHA);

	HookControlUpdate(GetSafeHwnd());

	return TRUE;
}

void CGDIPlusDlg::CreateFakeWnd()
{
	DestoryFakeWnd();

	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = ::DefWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = ::GetModuleHandle(NULL);
	wcex.hIcon = NULL;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = m_strWndClassName;
	wcex.hIconSm = NULL;

	VERIFY(RegisterClassEx(&wcex));

	CRect rc;
	GetWindowRect(rc);
	m_hFakeWnd = ::CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_LEFT
		, m_strWndClassName
		, NULL
		, WS_VISIBLE | WS_OVERLAPPED
		, rc.left
		, rc.top
		, rc.Width()
		, rc.Height()
		, GetSafeHwnd()
		, NULL
		, ::GetModuleHandle(NULL)
		, NULL
	);
	ASSERT(m_hFakeWnd != NULL);

}

void CGDIPlusDlg::DestoryFakeWnd()
{
	if (m_hFakeWnd != NULL)
	{
		::DestroyWindow(m_hFakeWnd);

		m_hFakeWnd = NULL;

		::UnregisterClass(m_strWndClassName, ::GetModuleHandle(NULL));
	}
}

void CGDIPlusDlg::HookControlUpdate(HWND hWnd)
{
	m_vCtrl[hWnd] = std::make_tuple((WNDPROC)::GetWindowLongPtr(hWnd, GWLP_WNDPROC), this);

	::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)(&CGDIPlusDlg::CtrlWndProc));

	HWND hChild = ::GetWindow(hWnd, GW_CHILD);

	while (hChild)
	{
		HookControlUpdate(hChild);

		hChild = ::GetWindow(hChild, GW_HWNDNEXT);
	}
}

void CGDIPlusDlg::UnhookControlUpdate(HWND hWnd)
{
	if (!::IsWindow(hWnd)) return;

	auto iUnhook = m_vCtrl.find(hWnd);

	if (iUnhook != m_vCtrl.end())m_vCtrl.erase(iUnhook);

	HWND hChild = ::GetWindow(hWnd, GW_CHILD);

	while (hChild)
	{
		UnhookControlUpdate(hChild);

		hChild = ::GetWindow(hChild, GW_HWNDNEXT);
	}
}

LRESULT CALLBACK CGDIPlusDlg::CtrlWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	auto iCtrl = m_vCtrl.find(hWnd);

	if (iCtrl == m_vCtrl.end()) return ::DefWindowProc(hWnd, nMsg, wParam, lParam);

	LRESULT lResult = std::get<0>(iCtrl->second)(hWnd, nMsg, wParam, lParam);

	switch (nMsg)
	{
	case WM_PAINT:
	case WM_CAPTURECHANGED:
		case WM_NCPAINT:
		case WM_CTLCOLOR:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORSCROLLBAR:
	{
		std::get<1>(iCtrl->second)->Refresh();
		break;
	}

	default:
		break;
	}
	return lResult;
}

void CGDIPlusDlg::Refresh()
{
	if (m_bIsRefreshing) return;

	if (!IsWindow(m_hFakeWnd)) return;

	m_bIsRefreshing = TRUE;

	RECT rc;
	::GetWindowRect(m_hFakeWnd, &rc);
	POINT ptSrc = { 0, 0 };
	POINT ptWinPos = { rc.left, rc.top };
	SIZE szWin = { m_nWidth, m_nHeigh };
	BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, m_nAlpha, AC_SRC_ALPHA };
	HDC hDC = ::GetDC(m_hFakeWnd);
	HDC hdcMemory = ::CreateCompatibleDC(hDC);
	BITMAPINFOHEADER stBmpInfoHeader = { 0 };
	int nBytesPerLine = ((m_nWidth * 32 + 31) & (~31)) >> 3;
	stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	stBmpInfoHeader.biWidth = m_nWidth;
	stBmpInfoHeader.biHeight = m_nHeigh;
	stBmpInfoHeader.biPlanes = 1;
	stBmpInfoHeader.biBitCount = 32;
	stBmpInfoHeader.biCompression = BI_RGB;
	stBmpInfoHeader.biClrUsed = 0;
	stBmpInfoHeader.biSizeImage = nBytesPerLine * m_nHeigh;

	PVOID pvBits = NULL;
	HBITMAP hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, &pvBits, NULL, 0);
	ASSERT(hbmpMem != NULL);
	memset(pvBits, 0, m_nWidth * 4 * m_nHeigh);
	if (hbmpMem)
	{
		HGDIOBJ hbmpOld = ::SelectObject(hdcMemory, hbmpMem);
		Gdiplus::Graphics graph(hdcMemory);

		graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

		OnDraw(m_lpImage);

		graph.DrawImage(m_lpImage, 0, 0, m_nWidth, m_nHeigh);

		OnDraw(graph); // Gdiplus::Graphics

		HWND hwndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD);

		while (hwndChild)
		{
			DrawCtrl(graph, hDC, hwndChild);

			hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
		}

		DrawCaret(graph);

		::UpdateLayeredWindow(m_hFakeWnd
			, hDC
			, &ptWinPos
			, &szWin
			, hdcMemory
			, &ptSrc
			, 0
			, &stBlend
			, ULW_ALPHA
		);

		graph.ReleaseHDC(hdcMemory);
		::SelectObject(hdcMemory, hbmpOld);
		::DeleteObject(hbmpMem);
	}


	::DeleteDC(hdcMemory);
	::DeleteDC(hDC);

	m_bIsRefreshing = FALSE;
}

void CGDIPlusDlg::DrawCtrl(Gdiplus::Graphics& graphics, HDC hDC, HWND hWnd)
{
	if (!::IsWindow(hWnd)) return;

	RECT rc;

	::GetWindowRect(hWnd, &rc);

	ScreenToClient(&rc);

	HDC hdcMemory = ::CreateCompatibleDC(hDC);

	HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, rc.right - rc.left, rc.bottom - rc.top);

	HGDIOBJ hbmpOld = ::SelectObject(hdcMemory, hBitmap);

	::SendMessage(hWnd, WM_PRINT, (WPARAM)hdcMemory, (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_ERASEBKGND | PRF_OWNED);

	Gdiplus::Bitmap bitmap(hBitmap, NULL);

	graphics.DrawImage(&bitmap, (INT)rc.left, (INT)rc.top);

	::SelectObject(hdcMemory, hbmpOld);

	::DeleteDC(hdcMemory);

	::DeleteObject(hBitmap);
}

void CGDIPlusDlg::DrawCaret(Gdiplus::Graphics& graph)
{
	CWnd* pWnd = GetFocus();
	if (pWnd == NULL ||
		!::IsWindow(pWnd->GetSafeHwnd()))
	{
		return;
	}

	TCHAR tszBuf[MAX_PATH] = { '\0' };
	::GetClassName(pWnd->GetSafeHwnd(), tszBuf, MAX_PATH);
	CString strClassName(tszBuf);

	if (strClassName.CompareNoCase(_T("EDIT")) != 0)
		return;

	CPoint pt = GetCaretPos();
	pWnd->ClientToScreen(&pt);
	ScreenToClient(&pt);

	Gdiplus::Pen pen(Gdiplus::Color::Black, 1.0f);

	graph.DrawLine(&pen, (INT)pt.x, (INT)pt.y, (INT)pt.x, (INT)pt.y + 13);
}

void CGDIPlusDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	::ShowWindow(m_hFakeWnd, bShow ? SW_SHOWNA : SW_HIDE);
}

void CGDIPlusDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_ACTIVE)
	{
		Invalidate(TRUE);
	}
}

void CGDIPlusDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	if (::IsWindow(m_hFakeWnd))
	{
		::SetWindowPos(m_hFakeWnd
			, NULL
			, x
			, y
			, 0
			, 0
			, SWP_NOZORDER | SWP_NOSIZE
		);
	}
}

void CGDIPlusDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (::IsWindow(m_hFakeWnd))
	{
		::SetWindowPos(m_hFakeWnd
			, NULL
			, 0
			, 0
			, cx
			, cy
			, SWP_NOZORDER | SWP_NOMOVE
		);
	}
}

void CGDIPlusDlg::OnClose()
{
	DestoryFakeWnd();

	CDialog::OnClose();
}

BOOL CGDIPlusDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}