#pragma once
#include <functional>

class CGDIDBuffer
{
protected:
	CGDIDBuffer();
	virtual ~CGDIDBuffer();

public:
	void InitCanvas(CWnd* lpWnd, CRect rtCanvas);
	void ResetCanvas(CWnd* lpWnd, CRect rtCanvas);

public:
	void SetBackdrop(COLORREF crColor);
	void SetTextColor(COLORREF crColor);
	void SetTxFormat(unsigned int uTxFormat) { m_uTxFormat = uTxFormat; }

protected:
	void OnDraw1(CDC* lpSrc, std::function<void(CDC&, const CRect&)> fOnPaint);
	void OnDraw2(CDC* lpSrc, std::function<void(CDC&, unsigned char*&, const long&)> fOnPaint);
	void OnDraw3(CDC* lpSrc, std::function<void(CDC&, const CRect&, unsigned char*&, const long&)> fOnPaint);

	unsigned char*& UpdatePixel(unsigned char* lpPixel = nullptr);

private:
	void InitBitmapInfo();
	void ResetCanvas();
	void DelCurCanvas();

protected:
	CRect m_rtClient;
	CRect m_rtCanvas;
	CDC	  m_dcCanvas;
	CBitmap m_bmpCanvas;
	CBitmap* m_bmpDefult;
	CBrush m_brCanvas;
	COLORREF m_dwTxColor;
	unsigned int m_uTxFormat;
	unsigned char* m_lpPixel;
	unsigned int m_uBufferSize;
	BITMAPINFOHEADER m_tBitmapHeader;
};

