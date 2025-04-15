/*
 *
 *	ChartBalloonLabel.h
 *
 *	Written by Cédric Moonen (cedric_moonen@hotmail.com)
 *
 *
 *
 *	This code may be used for any non-commercial and commercial purposes in a compiled form.
 *	The code may be redistributed as long as it remains unmodified and providing that the 
 *	author name and this disclaimer remain intact. The sources can be modified WITH the author 
 *	consent only.
 *	
 *	This code is provided without any garanties. I cannot be held responsible for the damage or
 *	the loss of time it causes. Use it at your own risks
 *
 *	An e-mail to notify me that you are using this code is appreciated also.
 *
 *
 */

#ifndef _CHARTBALLOONLABEL_H_ 
#define _CHARTBALLOONLABEL_H_

#include "ChartLabel.hxx"
#include "ChartFont.h"

//! Specialization of the CChartLabel to display a balloon label.
/**
	A balloon label is a label with a rounded rectangle area in which the 
	text is displayed and which is connected with a line to the point to
	which it is attached.
**/
template <class PointType>
class CChartBalloonLabel : public CChartLabel<PointType>
{
	friend CChartSerieBase<PointType>;

public:
	//! Sets the background color of the text area.
	void SetBackgroundColor(COLORREF colBackground)
	{
		m_colBackground = colBackground;
		this->m_pParentCtrl->RefreshCtrl();
	}
	//! Retrieves the background color of the text area.
	COLORREF GetBackgroundColor() const		{ return m_colBackground; }
	//! Sets the color of the line connecting the point to the text area.
	void SetLineColor(COLORREF colArrow)
	{
		m_colLine = colArrow;
		this->m_pParentCtrl->RefreshCtrl();
	}
	//! Retrieves the color of the line connecting the point to the text area.
	COLORREF GetLineColor() const			{ return m_colLine; }
	//! Sets the color of border's text area.
	void SetBorderColor(COLORREF colBorder)
	{
		m_colBorder = colBorder;
		this->m_pParentCtrl->RefreshCtrl();
	}
	//! Retrieves the color of border's text area.
	COLORREF GetBorderColor() const			{ return m_colBorder; }

	//! Specifies if the text area is rounded or not.
	void SetRoundedRect(bool bRounded)
	{
		m_bRoundedRect = bRounded;
		this->m_pParentCtrl->RefreshCtrl();
	}
	//! Returns true if the text area is rounded.
	bool GetRoundedRect() const		{ return m_bRoundedRect; }

	//! Sets the font of the text.
	/**
		@param nPointSize
			The font point size.
		@param strFaceName
			The font face name ("Times New Roman", "Arial", ...)
	**/
	void SetFont(int nPointSize, const TChartString& strFaceName)
	{
		m_Font.SetFont(strFaceName, nPointSize);
		if (this->m_pParentCtrl)
			this->m_pParentCtrl->RefreshCtrl();
	}
	//! Sets the font of the text.
	/**
		This function allows to set extended font style by passing
		a CChartFont object.
		@param newFont
			The new font.
	**/
	void SetFont(const CChartFont& newFont)
	{
		m_Font = newFont;
		if (this->m_pParentCtrl)
			this->m_pParentCtrl->RefreshCtrl();
	}

	//! Constructor
	CChartBalloonLabel(CChartCtrl* pParentCtrl, CChartSerieBase<PointType>* pParentSeries)
		: CChartLabel(pParentCtrl, pParentSeries), m_bRoundedRect(true)
	{
		m_colBackground = RGB(255, 255, 225);
		m_colLine = RGB(255, 255, 255);
		m_colBorder = RGB(0, 0, 0);
	}
	//! Destructor
	~CChartBalloonLabel() {}

protected:
	//! Draw the label.
	void Draw(CDC* pDC, unsigned uPointIndex)
	{
		if (this->m_pLabelProvider)
		{
			PointType Point = this->m_pParentSeries->GetPoint(uPointIndex);
			this->m_strLabelText = this->m_pLabelProvider->GetText(this->m_pParentSeries, uPointIndex);
		}
		if (this->m_strLabelText == _T(""))
			return;

		CPoint screenPt = this->m_pParentSeries->GetPointScreenCoord(uPointIndex);

		// Create the pen for the arrow
		CPen newPen(PS_SOLID, 1, m_colLine);
		CPen* pOldPen = pDC->SelectObject(&newPen);

		// Draw first the arrow
		pDC->MoveTo(screenPt);
		pDC->LineTo(screenPt.x, screenPt.y - 20);

		// Create and select a new pen for the border
		newPen.DeleteObject();
		newPen.CreatePen(PS_SOLID, 1, m_colBorder);
		pDC->SelectObject(&newPen);
		m_Font.SelectFont(pDC);

		// Create the brush to fill the rectangle
		CBrush newBrush(m_colBackground);
		CBrush* pOldBrush = pDC->SelectObject(&newBrush);

		// Calculate the size of the 
		CSize labelSize = pDC->GetTextExtent(this->m_strLabelText.c_str());
		labelSize += CSize(10, 10);
		int x = screenPt.x;
		int y = screenPt.y;
		CRect labelRect(CPoint(x - labelSize.cx / 2, y - 19 - labelSize.cy), labelSize);

		// Draw the rectangle
		if (m_bRoundedRect)
			pDC->RoundRect(labelRect, CPoint(10, 10));
		else
			pDC->Rectangle(labelRect);

		// Draw the text
		pDC->TextOut(labelRect.left + 5, labelRect.top + 5, this->m_strLabelText.c_str());

		// Clean the objects
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		newPen.DeleteObject();
		newBrush.DeleteObject();
		m_Font.UnselectFont(pDC);
	}

private:
	//! Color of the liune connecting the point to the text area.
	COLORREF m_colLine;
	//! Color of the text area's background.
	COLORREF m_colBackground;
	//! Color of border's text area.
	COLORREF m_colBorder;

	//! The font used for the text label.
	CChartFont m_Font;

	//! Specifies if the rectangle is rounded or not.
	bool m_bRoundedRect;
};

//#include "ChartBalloonLabel.inl"

#endif  // _CHARTBALLOONLABEL_H_
