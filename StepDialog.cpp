// StepDialog.cpp: 实现文件
//

#include "pch.h"
#include "VisionLocalization.h"
#include "afxdialogex.h"
#include "StepDialog.h"


// StepDialog 对话框

IMPLEMENT_DYNAMIC(StepDialog, CDialog)

StepDialog::StepDialog(int iMax)
	: CDialog(IDD_STEP_DIALOG, nullptr)
{
	m_iPos  = 0;
	m_iMax  = iMax;
}

StepDialog::~StepDialog()
{
}

void StepDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(StepDialog, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL StepDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect tRect;

	GetWindowRect(tRect);

	m_tBitmapBg.LoadBitmap(IDB_BITMAP_BG);

	CClientDC tClientDC(this);

	m_tMemDC1.CreateCompatibleDC(&tClientDC);
	m_tMemDC2.CreateCompatibleDC(&tClientDC);

	m_tBitmapBuf.CreateCompatibleBitmap(&tClientDC, tRect.Width(), tRect.Height());

	//SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	return TRUE;
}

BOOL StepDialog::OnEraseBkgnd(CDC* ptDC)
{
	//CDialog::OnEraseBkgnd(ptDC);
	ptDC->SetBkMode(TRANSPARENT);
	/*CRect tRect;
	GetClientRect(&tRect);

	BITMAP tBitmap;
	m_tBitmapBg.GetBitmap(&tBitmap);

	ptDC->StretchBlt(0, 0, tRect.Width(), tRect.Height(), &m_tMemDC, 0, 0, tBitmap.bmWidth, tBitmap.bmHeight, SRCCOPY);*/

	return TRUE;
}

HBRUSH StepDialog::OnCtlColor(CDC* ptDC, CWnd* ptWnd, UINT uCtlColor)
{
	/*HBRUSH tBrush = CDialog::OnCtlColor(ptDC, ptWnd, uCtlColor);

	// 判断是否为CStatic控件
	if (uCtlColor == CTLCOLOR_STATIC){
		ptDC->SetBkMode(TRANSPARENT);
		ptDC->SetTextColor(RGB(0, 0, 0));

		tBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return tBrush;*/

	return CDialog::OnCtlColor(ptDC, ptWnd, uCtlColor);
}

void StepDialog::setStepText(CString tStepText)
{
	CRect tRect;
	GetClientRect(&tRect);

	m_tText = tStepText;

	m_iPos++;

	/*BITMAP tBitmap;
	m_tBitmapBg.GetBitmap(&tBitmap);

	CClientDC tClientDC(this);

	tClientDC.StretchBlt(0, 0, tRect.Width(), tRect.Height(), &m_tMemDC, 0, 0, tBitmap.bmWidth, tBitmap.bmHeight, SRCCOPY);

	UINT uFormat = DT_LEFT | DT_SINGLELINE | DT_BOTTOM | DT_NOPREFIX;

	tRect.OffsetRect(20, -20);

	tClientDC.SetBkMode(TRANSPARENT);

	LOGFONT tLogFont;

	memset(&tLogFont, 0, sizeof(LOGFONT));
	tLogFont.lfHeight         = -24;  // 字体高度
	tLogFont.lfWidth          = 0;
	tLogFont.lfEscapement     = 0;
	tLogFont.lfOrientation    = 0;
	tLogFont.lfWeight         = FW_NORMAL;
	tLogFont.lfItalic         = FALSE;
	tLogFont.lfUnderline      = FALSE;
	tLogFont.lfStrikeOut      = FALSE;
	tLogFont.lfCharSet        = DEFAULT_CHARSET;
	tLogFont.lfOutPrecision   = OUT_TT_PRECIS;
	tLogFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	tLogFont.lfQuality        = ANTIALIASED_QUALITY;  // 设置抗锯齿
	tLogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy_s(tLogFont.lfFaceName, _T("微软雅黑"));

	CFont tFont;
	tFont.CreateFontIndirect(&tLogFont);

	tClientDC.SelectObject(&tFont);
	tClientDC.SetTextColor(RGB(255, 255, 255));

	tClientDC.DrawText(tStepText, &tRect, uFormat);*/

	CClientDC tClientDC(this);

	DoPaint(&tClientDC);
}

void StepDialog::DoPaint(CDC* ptDC)
{
	CBitmap* ptOld1Bitmap = m_tMemDC1.SelectObject(&m_tBitmapBg);
	CBitmap* ptOld2Bitmap = m_tMemDC2.SelectObject(&m_tBitmapBuf);

	CRect tRect;
	GetClientRect(&tRect);

	m_tMemDC2.BitBlt(0, 0, tRect.Width(), tRect.Height(), &m_tMemDC1, 0, 0, SRCCOPY);

	m_tMemDC2.SetBkMode(TRANSPARENT);
	m_tMemDC2.SetTextColor(RGB(0, 0, 0));

	LOGFONT tLogFont;

	memset(&tLogFont, 0, sizeof(LOGFONT));
	tLogFont.lfHeight         = -20;  // 字体高度
	tLogFont.lfWidth          = 0;
	tLogFont.lfEscapement     = 0;
	tLogFont.lfOrientation    = 0;
	tLogFont.lfWeight         = FW_NORMAL;
	tLogFont.lfItalic         = FALSE;
	tLogFont.lfUnderline      = FALSE;
	tLogFont.lfStrikeOut      = FALSE;
	tLogFont.lfCharSet        = DEFAULT_CHARSET;
	tLogFont.lfOutPrecision   = OUT_TT_PRECIS;
	tLogFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	tLogFont.lfQuality        = ANTIALIASED_QUALITY;  // 设置抗锯齿
	tLogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy_s(tLogFont.lfFaceName, _T("微软雅黑"));

	CFont tFont;
	tFont.CreateFontIndirect(&tLogFont);

	m_tMemDC2.SelectObject(&tFont);

	CRect tRc(10, tRect.top, tRect.Width()-20, tRect.Height()-10);

	UINT uFormat = DT_LEFT | DT_SINGLELINE | DT_BOTTOM | DT_NOPREFIX;

	tRc.OffsetRect(-1, 0);
	m_tMemDC2.DrawText(m_tText, &tRc, uFormat);
	tRc.OffsetRect(2, 0);
	m_tMemDC2.DrawText(m_tText, &tRc, uFormat);
	tRc.OffsetRect(-1, -1);
	m_tMemDC2.DrawText(m_tText, &tRc, uFormat);
	tRc.OffsetRect(0, 2);
	m_tMemDC2.DrawText(m_tText, &tRc, uFormat);
	tRc.OffsetRect(0, -1);
	m_tMemDC2.DrawText(m_tText, &tRc, uFormat);

	m_tMemDC2.SetTextColor(RGB(255, 255, 255));
	m_tMemDC2.DrawText(m_tText, &tRc, uFormat);

	tRc.SetRect(10, tRect.bottom-60, tRect.Width()-20, tRect.bottom-50);
	m_tMemDC2.Draw3dRect(&tRc, RGB(0x40, 0x40, 0x40), RGB(0x40, 0x40, 0x40));
	tRc.DeflateRect(1, 1);
	m_tMemDC2.FillSolidRect(&tRc, RGB(0x25, 0x25, 0x25));

	int iOffset = m_iMax - min(m_iPos, m_iMax);

	drawFragment(&m_tMemDC2, &tRc, m_iMax, iOffset, min(m_iPos, m_iMax), RGB(0x20, 0xB0, 0x20), TRUE);
	m_tMemDC2.SelectClipRgn(NULL);

	int iRadius = 40;
	CRgn tRgn;

	tRgn.CreateRoundRectRgn(tRect.left, tRect.top, tRect.right, tRect.bottom, iRadius, iRadius);

	ptDC->SelectClipRgn(&tRgn);
	ptDC->SetBkMode(TRANSPARENT);
	
	CBrush *ptOldBrush = (CBrush*)ptDC->SelectStockObject(NULL_BRUSH);

	ptDC->BitBlt(0, 0, tRect.Width(), tRect.Width(), &m_tMemDC2, 0, 0, SRCCOPY);

	ptDC->SelectObject(ptOldBrush);

	m_tMemDC1.SelectObject(ptOld1Bitmap);
	m_tMemDC2.SelectObject(ptOld2Bitmap);
}

void StepDialog::drawFragment(CDC* ptDC, CRect* ptRect,
				int iTotal, int iOffset, int iLength, COLORREF uClrFill, bool b3D)
{
	CRect rcArea;

	iOffset = iTotal - iOffset - iLength;

	rcArea.left = ptRect->left + (int)((double)(ptRect->Width() + 1) / (double)iTotal * (double)iOffset);
	rcArea.right = ptRect->left + (int)((double)(ptRect->Width() + 1) / (double)iTotal * (double)(iOffset + iLength));

	rcArea.top = ptRect->top;
	rcArea.bottom = ptRect->bottom;

	rcArea.left = max(rcArea.left, ptRect->left);
	rcArea.right = min(rcArea.right, ptRect->right);

	if (rcArea.right <= rcArea.left) return;

	if (b3D && rcArea.Width() > 2){
		ptDC->Draw3dRect(&rcArea, calculateColor(uClrFill, RGB(255, 255, 255), 75),
					    calculateColor(uClrFill, RGB(0, 0, 0), 75));

		rcArea.DeflateRect(1, 1);
		ptDC->FillSolidRect(&rcArea, uClrFill);
		rcArea.InflateRect(1, 1);
	}
	else{
		ptDC->FillSolidRect(&rcArea, uClrFill);
	}

	ptDC->ExcludeClipRect(&rcArea);
}

COLORREF StepDialog::calculateColor(COLORREF uClrFg, COLORREF uClrBg, int iAlpha)
{
	int iRed = GetRValue(uClrFg) * (255 - iAlpha) / 255 + GetRValue(uClrBg) * iAlpha / 255;
	int iGreen = GetGValue(uClrFg) * (255 - iAlpha) / 255 + GetGValue(uClrBg) * iAlpha / 255;
	int iBlue = GetBValue(uClrFg) * (255 - iAlpha) / 255 + GetBValue(uClrBg) * iAlpha / 255;

	return RGB(iRed, iGreen, iBlue);
}
