#include "pch.h"
#include "GDIDBuffer.h"

CGDIDBuffer::CGDIDBuffer():
	m_lpPixel(nullptr), m_uBufferSize(0),
	m_bmpDefult(nullptr), 
	m_uTxFormat(DT_LEFT | DT_TOP),m_dwTxColor(0xffffff),
	m_rtCanvas(0, 0, 0, 0), m_tBitmapHeader{ 0 }
{
	m_brCanvas.CreateSolidBrush(RGB(0xf0, 0xf0, 0xf0)); // MFC默认背景
}

CGDIDBuffer::~CGDIDBuffer()
{
	DelCurCanvas();

	m_brCanvas.DeleteObject();
}

void CGDIDBuffer::DelCurCanvas()
{
	if (!!m_lpPixel) { delete[] m_lpPixel; m_lpPixel = nullptr; }

	if (m_bmpDefult) m_dcCanvas.SelectObject(m_bmpDefult);

	if (m_bmpCanvas.GetSafeHandle()) m_bmpCanvas.DeleteObject();

	if (m_dcCanvas.GetSafeHdc()) m_dcCanvas.DeleteDC();
}

void CGDIDBuffer::SetBackdrop(COLORREF crColor)
{
	m_brCanvas.DeleteObject();

	m_brCanvas.CreateSolidBrush(crColor);
}

void CGDIDBuffer::ResetCanvas(CWnd* lpWnd, CRect rtCanvas)
{
	DelCurCanvas();

	InitCanvas(lpWnd, rtCanvas);
}

void CGDIDBuffer::SetTextColor(COLORREF crColor)
{
	m_dwTxColor = crColor;

	if (!m_dcCanvas.GetSafeHdc()) return;

	m_dcCanvas.SetBkMode(TRANSPARENT);

	m_dcCanvas.SetTextColor(crColor);
}

void CGDIDBuffer::InitCanvas(CWnd* lpWnd, CRect rtCanvas)
{
	m_rtCanvas = rtCanvas;

	CDC* lpDc = lpWnd->GetDC();

	lpWnd->GetClientRect(m_rtClient);

	m_bmpCanvas.CreateCompatibleBitmap(lpDc, m_rtCanvas.Width(), m_rtCanvas.Height());

	m_dcCanvas.CreateCompatibleDC(lpDc);

	m_bmpDefult = m_dcCanvas.SelectObject(&m_bmpCanvas);

	SetTextColor(m_dwTxColor); 

	lpWnd->ReleaseDC(lpDc); // 释放绘图资源

	InitBitmapInfo();
}

void CGDIDBuffer::OnDraw1(CDC* lpSrc, std::function<void(CDC&, const CRect&)> fOnPaint)
{
	ResetCanvas();

	fOnPaint(m_dcCanvas, m_rtCanvas);

	lpSrc->StretchBlt(
		m_rtClient.left, m_rtClient.top, m_rtClient.Width(), m_rtClient.Height(),
		&m_dcCanvas, 0, 0, m_rtCanvas.Width(), m_rtCanvas.Height(), SRCCOPY);
}

void CGDIDBuffer::OnDraw2(CDC* lpSrc, std::function<void(CDC&, unsigned char*&, const long&)> fOnPaint)
{
	ResetCanvas();

	fOnPaint(m_dcCanvas, UpdatePixel(), m_tBitmapHeader.biWidth);

	lpSrc->StretchBlt(
		m_rtClient.left, m_rtClient.top, m_rtClient.Width(), m_rtClient.Height(),
		&m_dcCanvas, 0, 0, m_rtCanvas.Width(), m_rtCanvas.Height(), SRCCOPY);
}

void CGDIDBuffer::OnDraw3(CDC* lpSrc, std::function<void(CDC&, const CRect&, unsigned char*&, const long&)> fOnPaint)
{
	ResetCanvas();

	fOnPaint(m_dcCanvas, m_rtCanvas, UpdatePixel(), m_tBitmapHeader.biWidth);

	lpSrc->StretchBlt(
		m_rtClient.left, m_rtClient.top, m_rtClient.Width(), m_rtClient.Height(),
		&m_dcCanvas, 0, 0, m_rtCanvas.Width(), m_rtCanvas.Height(), SRCCOPY);
}

unsigned char*& CGDIDBuffer::UpdatePixel(unsigned char* lpPixel)
{
	if (lpPixel)
	{
		SetDIBits(m_dcCanvas.GetSafeHdc(), m_bmpCanvas, 0, abs(m_tBitmapHeader.biHeight), lpPixel, (LPBITMAPINFO)&m_tBitmapHeader, DIB_RGB_COLORS);
	}
	GetDIBits(m_dcCanvas.GetSafeHdc(), m_bmpCanvas, 0, abs(m_tBitmapHeader.biHeight), m_lpPixel, (LPBITMAPINFO)&m_tBitmapHeader, DIB_RGB_COLORS);

	return m_lpPixel; // 调用者自己需要做越界判断
}

void CGDIDBuffer::InitBitmapInfo()
{
	BITMAP tBitMap;
	m_bmpCanvas.GetBitmap(&tBitMap);
	m_tBitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_tBitmapHeader.biWidth = tBitMap.bmWidth;
	m_tBitmapHeader.biHeight = (0 - abs(tBitMap.bmHeight));
	m_tBitmapHeader.biPlanes = 1;
	m_tBitmapHeader.biBitCount = 32;
	m_tBitmapHeader.biCompression = BI_RGB; // 防止后期扩展
	m_tBitmapHeader.biSizeImage = 0;
	m_tBitmapHeader.biXPelsPerMeter = 0;
	m_tBitmapHeader.biYPelsPerMeter = 0;
	m_tBitmapHeader.biClrUsed = 0;
	m_tBitmapHeader.biClrImportant = 0;
	DWORD dwBmpSize = (m_tBitmapHeader.biWidth * m_tBitmapHeader.biBitCount + 31) / 32 * abs(m_tBitmapHeader.biHeight) * (m_tBitmapHeader.biBitCount / 8);
	m_lpPixel = new unsigned char[dwBmpSize];
	m_uBufferSize = dwBmpSize;
}

void CGDIDBuffer::ResetCanvas()
{
	m_dcCanvas.FillRect(&m_rtCanvas, &m_brCanvas);
}