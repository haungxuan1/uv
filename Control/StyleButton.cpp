#include "pch.h"
#include "StyleButton.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// StyleButton dialog

StyleButton::StyleButton()
	: m_hIcon(nullptr)
{

}

StyleButton::~StyleButton()
{
	if (m_hIcon != nullptr) {
		DestroyIcon(m_hIcon);
	}
}

BEGIN_MESSAGE_MAP(StyleButton, CButton)
    ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

void StyleButton::PreSubclassWindow()
{
    CButton::PreSubclassWindow();
    ModifyStyle(0, BS_OWNERDRAW);
}

BOOL StyleButton::OnEraseBkgnd(CDC* pDC)
{
    return TRUE; // 不进行默认的背景擦除操作
}

void StyleButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (lpDrawItemStruct != nullptr) {
        CDC* ptDC = CDC::FromHandle(lpDrawItemStruct->hDC);

        if (ptDC != nullptr) {
            Gdiplus::Graphics tGraphics(ptDC->m_hDC);

            ptDC->SetBkMode(TRANSPARENT);

            tGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);                                // 设置抗锯齿

            CRect tRect(lpDrawItemStruct->rcItem);
            Gdiplus::Rect gdiplusRect(tRect.left, tRect.top, tRect.Width(), tRect.Height());

            // 绘制渐变填充的圆角矩形
            Gdiplus::Color tStartColor = m_tBeginColor;
            Gdiplus::Color tEndColor = m_tEndColor;

            if (lpDrawItemStruct->itemState & ODS_SELECTED) {
                tStartColor = m_tEndColor;
                tEndColor = m_tBeginColor;
            }

            drawRoundedGradientRect(tGraphics, gdiplusRect, m_iRadius, tStartColor, tEndColor, m_tBorderColor);

            bool bAlignCenter = true;

            // 绘制图标
            if (m_hIcon) {
                int iIconSize = m_iIconSize;
                int iIconX = tRect.left + 10;
                int iIconY = (tRect.Height() - iIconSize) / 2 + tRect.top;

                DrawIconEx(ptDC->m_hDC, iIconX, iIconY, m_hIcon, iIconSize, iIconSize, 0, NULL, DI_NORMAL);

                tRect.left += 36;

                bAlignCenter = false;
            }

            //绘制按钮文本
            CString tStrText;
            CRect tTextRect = tRect;

            GetWindowText(tStrText);

            tTextRect.top += 3;

            drawText(tGraphics, Gdiplus::RectF(tTextRect.left, tTextRect.top, tTextRect.Width(), tTextRect.Height()), tStrText, m_tTextColor, bAlignCenter);
        }
    }
}

void StyleButton::drawShadow(Gdiplus::Graphics& tGraphics, const Gdiplus::Rect& tRect, int iRadius, int iShadowOffset, int iShadowAlpha)
{
    Gdiplus::GraphicsPath tShadowPath;
    Gdiplus::Rect tShadowRect(tRect.X + iShadowOffset, tRect.Y + iShadowOffset, tRect.Width, tRect.Height);
    
    tShadowPath.AddArc(tShadowRect.X, tShadowRect.Y, iRadius * 2, iRadius * 2, 180, 90);
    tShadowPath.AddArc(tShadowRect.X + tShadowRect.Width - iRadius * 2-1, tShadowRect.Y, iRadius * 2, iRadius * 2, 270, 90);
    tShadowPath.AddArc(tShadowRect.X + tShadowRect.Width - iRadius * 2-1, tShadowRect.Y + tShadowRect.Height - iRadius * 2-1, iRadius * 2, iRadius * 2, 0, 90);
    tShadowPath.AddArc(tShadowRect.X, tShadowRect.Y + tShadowRect.Height - iRadius * 2-1, iRadius * 2, iRadius * 2, 90, 90);
    tShadowPath.CloseFigure();

    Gdiplus::SolidBrush tSolidBrush(Gdiplus::Color(iShadowAlpha, 0, 0, 0));
    tGraphics.FillPath(&tSolidBrush, &tShadowPath);
}

void StyleButton::drawRoundedGradientRect(Gdiplus::Graphics& tGraphics, const Gdiplus::Rect& tRect, int iRadius,
                                          const Gdiplus::Color& tStartColor, const Gdiplus::Color& tEndColor, Gdiplus::Color tBorderColor)
{
    Gdiplus::GraphicsPath tPath;

    tPath.AddArc(tRect.X, tRect.Y, iRadius * 2, iRadius * 2, 180, 90);
    tPath.AddArc(tRect.X + tRect.Width - iRadius * 2-1, tRect.Y, iRadius * 2, iRadius * 2, 270, 90);
    tPath.AddArc(tRect.X + tRect.Width - iRadius * 2-1, tRect.Y + tRect.Height - iRadius * 2-1, iRadius * 2, iRadius * 2, 0, 90);
    tPath.AddArc(tRect.X, tRect.Y + tRect.Height - iRadius * 2-1, iRadius * 2, iRadius * 2, 90, 90);
    tPath.CloseFigure();

    Gdiplus::LinearGradientBrush tGradientBrush(Gdiplus::Point(tRect.X, tRect.Y), Gdiplus::Point(tRect.X, tRect.Y + tRect.Height-1), tStartColor, tEndColor);
    tGraphics.FillPath(&tGradientBrush, &tPath);

    Gdiplus::Pen tPen(tBorderColor, 1);
    tGraphics.DrawPath(&tPen, &tPath);
}

void StyleButton::drawText(Gdiplus::Graphics& tGraphics, const Gdiplus::RectF& tRect, 
                           CString& tStrText, const Gdiplus::Color& tTextColor, bool bAlignCenter)
{
    CFont *ptFont = this->GetFont();

    if (ptFont != nullptr) {
        LOGFONT tLogFont;

        ptFont->GetLogFont(&tLogFont);

        int iDpiY = tGraphics.GetDpiY();
        float fFontSize = static_cast<float>(abs(tLogFont.lfHeight) * 72.0 / iDpiY);

        Gdiplus::Font tFont(tLogFont.lfFaceName, fFontSize);
        Gdiplus::SolidBrush tSolidBrush(tTextColor);

        Gdiplus::StringFormat tFormat;

        tFormat.SetAlignment(bAlignCenter ? Gdiplus::StringAlignmentCenter : Gdiplus::StringAlignmentNear);
        tFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        tGraphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

        tGraphics.DrawString(tStrText, -1, &tFont, tRect, &tFormat, &tSolidBrush);
    }
}

void StyleButton::setIcon(HICON hIcon, int iSize, bool bRefresh)
{
    m_hIcon = hIcon;
    m_iIconSize = iSize;

    if (bRefresh) {
        Invalidate(NULL);
    }
}
void StyleButton::setBeginColor(Gdiplus::Color tColor, bool bRefresh)
{
    m_tBeginColor = tColor;

    if (bRefresh) {
        Invalidate(NULL);
    }
}

void StyleButton::setEndColor(Gdiplus::Color tColor, bool bRefresh)
{
    m_tEndColor = tColor;

    if (bRefresh) {
        Invalidate(NULL);
    }
}

void StyleButton::setBorderColor(Gdiplus::Color tColor, bool bRefresh)
{
    m_tBorderColor = tColor;

    if (bRefresh) {
        Invalidate(NULL);
    }
}

void StyleButton::setTextColor(Gdiplus::Color tColor, bool bRefresh)
{
    m_tTextColor = tColor;

    if (bRefresh) {
        Invalidate(NULL);
    }
}

void StyleButton::setRadius(int iRadius, bool bRefresh)
{
    if (m_iRadius != iRadius) {
        m_iRadius = iRadius;

        if (bRefresh) {
            Invalidate(NULL);
        }
    }
}
