#pragma once
#include <afxwin.h>
#include <gdiplus.h>

class StyleButton : public CButton
{
public:
	StyleButton();
	~StyleButton();

	void setIcon(HICON hIcon, int iSize, bool bRefresh = false);
	void setBeginColor(Gdiplus::Color tColor, bool bRefresh = false);
	void setEndColor(Gdiplus::Color tColor, bool bRefresh = false);
	void setBorderColor(Gdiplus::Color tColor, bool bRefresh = false);
	void setTextColor(Gdiplus::Color tColor, bool bRefresh = false);
	void setRadius(int iRadius, bool bRefresh = false);

protected:
	virtual void PreSubclassWindow();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

private:
	HICON m_hIcon;
	int m_iIconSize = 28;
	int m_iRadius = 10;
	Gdiplus::Color m_tBeginColor  = Gdiplus::Color(255, 0, 128, 64);
	Gdiplus::Color m_tEndColor    = Gdiplus::Color(255, 240, 240, 240);
	Gdiplus::Color m_tBorderColor = Gdiplus::Color(255, 50, 50, 255);
	Gdiplus::Color m_tTextColor   = Gdiplus::Color(255, 0, 0, 0);

	void drawShadow(Gdiplus::Graphics& tGraphics, const Gdiplus::Rect& tRect, int iRadius, int iShadowOffset, int iShadowAlpha);
	void drawRoundedGradientRect(Gdiplus::Graphics& tGraphics, const Gdiplus::Rect& tRect, int iRadius,
		                         const Gdiplus::Color& tStartColor, const Gdiplus::Color& tEndColor, Gdiplus::Color tBorderColor);
	void drawText(Gdiplus::Graphics& tGraphics, const Gdiplus::RectF& tRect,
		CString& tStrText, const Gdiplus::Color& tTextColor, bool bAlignCenter);
};

