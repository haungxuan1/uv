#include "pch.h"
#include "afx.h"
#include "MakeTemplateView.h"
#include "resource.h"
#include "../utils/OpenCvUtil.h"

IMPLEMENT_DYNCREATE(MakeTemplateView, CScrollView)

MakeTemplateView::MakeTemplateView() noexcept
{
    m_tPointBegin = CPoint(0, 0);
    m_tPointEnd = m_tPointBegin;
}

MakeTemplateView::~MakeTemplateView()
{
    if (m_tTemplateDC.GetSafeHdc()) {
        m_tTemplateDC.SelectObject(m_ptOldBmpTemplate);

        m_tTemplateDC.DeleteDC();
    }
    m_ptBmpTemplate.DeleteObject();

    if (m_tProductDC.GetSafeHdc()) {
        m_tProductDC.SelectObject(m_ptOldBmpProduct);

        m_tProductDC.DeleteDC();
    }
    m_ptBmpProduct.DeleteObject();

    if (m_tMemDC.GetSafeHdc()) {
        m_tMemDC.SelectObject(m_ptOldBmpBg);

        m_tMemDC.DeleteDC();
    }
    m_ptBmpBg.DeleteObject();
}

BEGIN_MESSAGE_MAP(MakeTemplateView, CScrollView)
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONUP()
    ON_WM_MOUSEWHEEL()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
END_MESSAGE_MAP()

/*******************************************************
 * 功能：初始化数据
 * 参数：
 *       tTemplateMat： 模板图
 *       tProductMat：  产品图
 * 返回值：无
*******************************************************/
void MakeTemplateView::init(Mat& tTemplateMat, Mat& tProductMat, float fScale)
{
    cv::cvtColor(tTemplateMat, m_tTemplateMatBGRA, cv::COLOR_GRAY2BGRA);

    if (tProductMat.channels() > 4) {
        std::vector<cv::Mat> tChnMatVec;

        cv::split(tProductMat, tChnMatVec);

        std::vector<cv::Mat> tNewMatVec;

        tNewMatVec.push_back(tChnMatVec[0]);
        tNewMatVec.push_back(tChnMatVec[1]);
        tNewMatVec.push_back(tChnMatVec[2]);
        tNewMatVec.push_back(tChnMatVec[3]);

        cv::merge(tNewMatVec, m_tProductMatBGRA);
    }
    else {
        m_tProductMatBGRA = tProductMat;
    }

    m_fScale = fScale;

    GetClientRect(&m_tOriginRect);
    reset();
}

/*******************************************************
 * 功能：重新生成模板图
 * 参数：无
 * 返回值：无
*******************************************************/
void MakeTemplateView::resetTemplate()
{
    if (m_tTemplateDC.GetSafeHdc()) {
        m_tTemplateDC.SelectObject(m_ptOldBmpTemplate);

        m_tTemplateDC.DeleteDC();
    }

    m_ptBmpTemplate.DeleteObject();

    if (!m_tTemplateMatBGRA.empty()) {
        CDC* ptDC = GetDC();

        if (ptDC != nullptr) {
            Mat tTempMat;

            m_tTemplateSize.cx = m_fScale * m_tTemplateMatBGRA.cols;
            m_tTemplateSize.cy = m_fScale * m_tTemplateMatBGRA.rows;

            resize(m_tTemplateMatBGRA, tTempMat, cv::Size(m_tTemplateSize.cx, m_tTemplateSize.cy), 0, 0, cv::INTER_AREA);
            convertScaleAbs(tTempMat, tTempMat, m_fContrastRatio, 0);

            //填充模板到内存DC
            m_tTemplateDC.CreateCompatibleDC(ptDC);
            m_ptBmpTemplate.CreateCompatibleBitmap(ptDC, m_tTemplateSize.cx, m_tTemplateSize.cy);
            m_ptOldBmpTemplate = m_tTemplateDC.SelectObject(&m_ptBmpTemplate);

            int iPixelBytes = tTempMat.channels() * (tTempMat.depth() + 1);
            BITMAPINFO tBitmapInfo;

            memset((void*)&tBitmapInfo, 0, sizeof(tBitmapInfo));

            tBitmapInfo.bmiHeader.biBitCount      = 8 * tTempMat.channels();
            tBitmapInfo.bmiHeader.biWidth         = tTempMat.cols;
            tBitmapInfo.bmiHeader.biHeight        = -tTempMat.rows;
            tBitmapInfo.bmiHeader.biPlanes        = 1;
            tBitmapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
            tBitmapInfo.bmiHeader.biCompression   = BI_RGB;
            tBitmapInfo.bmiHeader.biClrImportant  = 0;
            tBitmapInfo.bmiHeader.biClrUsed       = 0;
            tBitmapInfo.bmiHeader.biSizeImage     = 0;
            tBitmapInfo.bmiHeader.biXPelsPerMeter = 0;
            tBitmapInfo.bmiHeader.biYPelsPerMeter = 0;

            ::StretchDIBits(m_tTemplateDC, 0, 0, tTempMat.cols, tTempMat.rows,
                0, 0, tTempMat.cols, tTempMat.rows, tTempMat.data,
                &tBitmapInfo, DIB_RGB_COLORS, SRCCOPY);


            ReleaseDC(ptDC);
        }
    }
}

/*******************************************************
 * 功能：重新生成产品图
 * 参数：无
 * 返回值：无
*******************************************************/
void MakeTemplateView::resetProduct()
{
    if (m_tProductDC.GetSafeHdc()) {
        m_tProductDC.SelectObject(m_ptOldBmpProduct);

        m_tProductDC.DeleteDC();
    }

    m_ptBmpProduct.DeleteObject();

    if (!m_tProductMatBGRA.empty()) {
        CDC* ptDC = GetDC();

        if (ptDC != nullptr) {
            float fScale = m_fScale * m_fProductScale;
            Mat tProductMat;

            m_tProductSize.cx = fScale * m_tProductMatBGRA.cols;
            m_tProductSize.cy = fScale * m_tProductMatBGRA.rows;

            resize(m_tProductMatBGRA, tProductMat, cv::Size(m_tProductSize.cx, m_tProductSize.cy), 0, 0, cv::INTER_AREA);
            OpenCvUtil::rotate(tProductMat, tProductMat, m_fAngle, 0x00);

            m_tProductSize.cx = tProductMat.cols;
            m_tProductSize.cy = tProductMat.rows;

            // 填充产品图到内存DC
            m_tProductDC.CreateCompatibleDC(ptDC);
            m_ptBmpProduct.CreateCompatibleBitmap(ptDC, tProductMat.cols, tProductMat.rows);
            m_ptOldBmpProduct = m_tProductDC.SelectObject(&m_ptBmpProduct);

            m_tProductDC.SetBkMode(TRANSPARENT);

            int iPixelBytes = tProductMat.channels() * (tProductMat.depth() + 1);
            BITMAPINFO tBitmapInfo;

            memset((void*)&tBitmapInfo, 0, sizeof(tBitmapInfo));

            tBitmapInfo.bmiHeader.biBitCount      = 8 * tProductMat.channels();
            tBitmapInfo.bmiHeader.biWidth         = tProductMat.cols;
            tBitmapInfo.bmiHeader.biHeight        = -tProductMat.rows;
            tBitmapInfo.bmiHeader.biPlanes        = 1;
            tBitmapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
            tBitmapInfo.bmiHeader.biCompression   = BI_RGB;
            tBitmapInfo.bmiHeader.biClrImportant  = 0;
            tBitmapInfo.bmiHeader.biClrUsed       = 0;
            tBitmapInfo.bmiHeader.biSizeImage     = 0;
            tBitmapInfo.bmiHeader.biXPelsPerMeter = 0;
            tBitmapInfo.bmiHeader.biYPelsPerMeter = 0;

            ::StretchDIBits(m_tProductDC, 0, 0, tProductMat.cols, tProductMat.rows,
                0, 0, tProductMat.cols, tProductMat.rows, tProductMat.data,
                &tBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

            ReleaseDC(ptDC);
        }
    }
}

/*******************************************************
 * 功能：重新生成模板与产品展示图
 * 参数：无
 * 返回值：无
*******************************************************/
void MakeTemplateView::reset()
{
    if (m_tMemDC.GetSafeHdc()) {
        m_tMemDC.SelectObject(m_ptOldBmpBg);

        m_tMemDC.DeleteDC();
    }

    m_ptBmpBg.DeleteObject();

    resetTemplate();
    resetProduct();

    CDC* ptDC = GetDC();

    if (ptDC != nullptr) {
        int iBgWid = std::max({ (int)m_tTemplateSize.cx, (int)m_tProductSize.cx });
        int iBgHei = std::max({ (int)m_tTemplateSize.cy, (int)m_tProductSize.cy });
        m_tScrollRect.SetRect(0, 0, iBgWid, iBgHei);
        setScrollbar();

        m_tMemDCRect.SetRect(0, 0, std::max({ m_tScrollRect.Width(), m_tOriginRect.Width() }), std::max({ m_tScrollRect.Height(), m_tOriginRect.Height() }));

        m_tMemDC.CreateCompatibleDC(ptDC);
        m_ptBmpBg.CreateCompatibleBitmap(ptDC, m_tMemDCRect.Width(), m_tMemDCRect.Height());
        m_ptOldBmpBg = m_tMemDC.SelectObject(&m_ptBmpBg);

        m_tMemDC.FillSolidRect(m_tMemDCRect, RGB(0xff, 0xff, 0xff));

        ReleaseDC(ptDC);
    }
}

/*******************************************************
 * 功能：绘画
 * 参数：无
 * 返回值：无
*******************************************************/
void MakeTemplateView::OnDraw(CDC* ptDC)
{
    if (ptDC != nullptr) {
        CBrush tBrush;

        ptDC->SetStretchBltMode(COLORONCOLOR);
        //OnPrepareDC(ptDC);

        tBrush.CreateSolidBrush(RGB(0xff, 0xff, 0xff));

        m_tMemDC.FillRect(&m_tMemDCRect, &tBrush);

        // 模板
        /*m_tMemDC.StretchBlt(0, 0, m_tTemplateSize.cx, m_tTemplateSize.cy,
                 &m_tTemplateDC, 0, 0, m_tTemplateSize.cx, m_tTemplateSize.cy, SRCCOPY);*/

        m_tMemDC.BitBlt(0, 0, m_tTemplateSize.cx, m_tTemplateSize.cy, &m_tTemplateDC, 0, 0, SRCCOPY);

        // 产品
        BLENDFUNCTION tBlendFunction;
        tBlendFunction.BlendOp             = AC_SRC_OVER;
        tBlendFunction.BlendFlags          = 0;
        tBlendFunction.SourceConstantAlpha = m_iAlpha;  // 半透明
        tBlendFunction.AlphaFormat         = 0;

        // 将第二个 DC 的内容半透明地绘制到第一个 DC 上
        m_tMemDC.AlphaBlend(m_iProductX, m_iProductY, m_tProductSize.cx, m_tProductSize.cy,
            &m_tProductDC, 0, 0, m_tProductSize.cx, m_tProductSize.cy, tBlendFunction);

        // 绘制框选矩形
        tBrush.CreateStockObject(NULL_BRUSH);
        CBrush* ptOldBrush = m_tMemDC.SelectObject(&tBrush);

        CPen tPen(PS_SOLID, 2, RGB(200, 0, 0));
        CPen* ptOldPen = m_tMemDC.SelectObject(&tPen);

        CRect tRect = CRect(m_tPointBegin, m_tPointEnd);
        tRect.NormalizeRect();

        m_tMemDC.Rectangle(&tRect);

        m_tMemDC.SelectObject(ptOldPen);
        m_tMemDC.SelectObject(ptOldBrush);

        // 页面填充
        CRect tClientRect;
        GetClientRect(&tClientRect);

        /*ptDC->StretchBlt(0, 0, tClientRect.Width(), tClientRect.Height(),
            &m_tMemDC, m_iScrollX, m_iScrollY, tClientRect.Width(), tClientRect.Height(), SRCCOPY);*/

        ptDC->BitBlt(0, 0, tClientRect.Width(), tClientRect.Height(), &m_tMemDC, m_iScrollX, m_iScrollY, SRCCOPY);
    }
}

void MakeTemplateView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
    CScrollView::OnPrepareDC(pDC, pInfo);

    pDC->SetViewportOrg(CPoint(0, 0));
}

/*******************************************************
 * 功能：设置滚动条
 * 参数：无
 * 返回值：无
*******************************************************/
void MakeTemplateView::setScrollbar()
{
    SetScrollSizes(MM_TEXT, CSize(0, 0));
   
    CSize tSizeTotal(m_tScrollRect.Width(), m_tScrollRect.Height());

    if ((m_tScrollRect.Width() > m_tOriginRect.Width()) && (m_tScrollRect.Height() > m_tOriginRect.Height())) {
        tSizeTotal.SetSize(m_tScrollRect.Width() - 1, m_tScrollRect.Height() - 1); 
    }

    if ((m_tScrollRect.Width() < m_tOriginRect.Width()) && (m_tScrollRect.Height() > m_tOriginRect.Height())) {
        tSizeTotal.SetSize(m_tScrollRect.Width() - 1, m_tScrollRect.Height());
    }

    if ((m_tScrollRect.Width() > m_tOriginRect.Width()) && (m_tScrollRect.Height() < m_tOriginRect.Height())) {
        tSizeTotal.SetSize(m_tScrollRect.Width(), m_tScrollRect.Height() - 1);
    }

    SetScrollSizes(MM_TEXT, tSizeTotal);
}

/*******************************************************
 * 功能：整体缩放比例
 * 参数：
 *      fScale：缩放比例
 * 返回值：无
*******************************************************/
void MakeTemplateView::setScale(float fScale)
{
    if (m_fScale != fScale) {
        float fScaleFactor = fScale / m_fScale;
        m_fScale = fScale;

        m_iProductX = (int)m_iProductX * fScaleFactor;
        m_iProductY = (int)m_iProductY * fScaleFactor;

        m_tPointBegin.SetPoint(0, 0);
        m_tPointEnd.SetPoint(0, 0);    

        SCROLLINFO tSI{};
        tSI.cbSize = sizeof(SCROLLINFO);
        tSI.fMask = SIF_ALL;

        GetScrollInfo(SB_HORZ, &tSI);
        int iHPos = tSI.nPos;
        int iHMin = tSI.nMin;
        int iHMax = tSI.nMax;

        GetScrollInfo(SB_VERT, &tSI);
        int iVPos = tSI.nPos;
        int iVMin = tSI.nMin;
        int iVMax = tSI.nMax;

        reset();

        SCROLLINFO tNewSI{};
        tNewSI.cbSize = sizeof(SCROLLINFO);
        tNewSI.fMask = SIF_ALL;

        GetScrollInfo(SB_HORZ, &tNewSI);
        int iHNewMin = tNewSI.nMin;
        int iHNewMax = tNewSI.nMax;

        GetScrollInfo(SB_VERT, &tNewSI);
        int iVNewMin = tNewSI.nMin;
        int iVNewMax = tNewSI.nMax;

        /*m_iScrollX = (int)(fScaleFactor * iHPos * (iHNewMax - iHNewMin + 1) / (iHMax - iHMin + 1));
        m_iScrollY = (int)(fScaleFactor * iVPos * (iVNewMax - iVNewMin + 1) / (iVMax - iVMin + 1));*/
        m_iScrollX = (int)((float) iHPos * (iHNewMax - iHNewMin + 1) / (iHMax - iHMin + 1));
        m_iScrollY = (int)((float) iVPos * (iVNewMax - iVNewMin + 1) / (iVMax - iVMin + 1));

        if (m_tScrollRect.Width() < iHNewMax) {
            m_iScrollX = 0;
        }

        if (m_tScrollRect.Height() < iVNewMax) {
            m_iScrollY = 0;
        }

        ScrollToPosition(CPoint(m_iScrollX, m_iScrollY));

        Invalidate(NULL);
    }
}

/*******************************************************
 * 功能：获取整体缩放比例
 * 参数：无
 * 返回值：整体缩放比例
*******************************************************/
float MakeTemplateView::getScale()
{
    return m_fScale;
}

/*******************************************************
 * 功能：添加产品缩放比例
 * 参数：
 *      fOffset：缩放比例偏移值
 * 返回值：无
*******************************************************/
void MakeTemplateView::addProductScale(float fOffset)
{
    if (fOffset != 0) {
        int iCenterX = m_iProductX + m_tProductSize.cx/2;
        int iCenterY = m_iProductY + m_tProductSize.cy/2;

        m_fProductScale *= fOffset;

        resetProduct();

        m_iProductX = iCenterX - m_tProductSize.cx / 2;
        m_iProductY = iCenterY - m_tProductSize.cy / 2;

        Invalidate(NULL);
    }
}

/*******************************************************
 * 功能：获取产品缩放比例
 * 参数：无
 * 返回值：产品缩放比例
*******************************************************/
float MakeTemplateView::getProductScale()
{
    return m_fProductScale;
}

/*******************************************************
 * 功能：添加产品旋转角度
 * 参数：
 *      fDelta：角度偏移值
 * 返回值：无
*******************************************************/
void MakeTemplateView::addAngle(float fDelta)
{
    if (fDelta != 0) {
        int iCenterX = m_iProductX + m_tProductSize.cx / 2;
        int iCenterY = m_iProductY + m_tProductSize.cy / 2;

        m_fAngle += fDelta;

        resetProduct();

        m_iProductX = iCenterX - m_tProductSize.cx / 2;
        m_iProductY = iCenterY - m_tProductSize.cy / 2;

        Invalidate(NULL);
    }
}

/*******************************************************
 * 功能：获取产品旋转角度
 * 参数：无
 * 返回值：产品旋转角度
*******************************************************/
float MakeTemplateView::getAngle()
{
    return m_fAngle;
}

/*******************************************************
 * 功能：X方向平移
 * 参数：
 *      fValue：偏移值
 * 返回值：无
*******************************************************/
void MakeTemplateView::setOffsetX(float fValue)
{
    m_iProductX += fValue;

    Invalidate(NULL);
}

/*******************************************************
 * 功能：Y方向平移
 * 参数：
 *      fValue：偏移值
 * 返回值：无
*******************************************************/
void MakeTemplateView::setOffsetY(float fValue)
{
    m_iProductY += fValue;

    Invalidate(NULL);
}

/*******************************************************
 * 功能：设置产品半透明度
 * 参数：
 *      iAlpha：半透明度
 * 返回值：无
*******************************************************/
void MakeTemplateView::setAlpha(int iAlpha)
{
    if (m_iAlpha != iAlpha) {
        m_iAlpha = iAlpha;

        resetProduct();

        Invalidate(NULL);
    }
}

/*******************************************************
 * 功能：设置模板对比度（有些模板不清晰）
 * 参数：
 *      fContrastRatio：对比度
 * 返回值：无
*******************************************************/
void MakeTemplateView::setContrastRatio(float fContrastRatio)
{
    m_fContrastRatio = fContrastRatio;

    resetTemplate();

    Invalidate(NULL);
}

/*******************************************************
 * 功能：复位
 * 参数：无
 * 返回值：无
*******************************************************/
void MakeTemplateView::resetAll()
{
    m_fAngle = 0;
    m_fScale = 0.25f;
    m_fProductScale = 1.0f;
    m_iProductX = 0;
    m_iProductY = 0;
    m_iAlpha = 128;

    m_tPointBegin = CPoint(0, 0);
    m_tPointEnd = m_tPointBegin;

    ScrollToPosition(CPoint(0, 0));
    m_iScrollX = 0;
    m_iScrollY = 0;

    reset();
    Invalidate(NULL);
}

/*******************************************************
 * 功能：自动定位
 * 参数：
 *      fAngle：  旋转角度
 *      ptLT:     左上角坐标
 * 返回值：无
*******************************************************/
void MakeTemplateView::autoProductPosition(float fAngle, int iCenterX, int iCenterY)
{
    if (fAngle != 0) {
        m_fAngle = fAngle;  
        resetProduct();

        m_iProductX = iCenterX * m_fScale - m_tProductSize.cx / 2.0f;
        m_iProductY = iCenterY * m_fScale - m_tProductSize.cy / 2.0f;
    }
    
    Invalidate(NULL);
}

/*******************************************************
 * 功能：获取模板选中区域坐标
 * 参数：无
 * 返回值：坐标信息
*******************************************************/
cv::Rect MakeTemplateView::getTemplateSelectRect()
{
    cv::Rect tRect(0, 0, 0, 0);

    if (m_tPointBegin != m_tPointEnd) {
        CRect tClientRect = CRect(m_tPointBegin, m_tPointEnd);
        tClientRect.NormalizeRect();

        int iLeft   = tClientRect.left;
        int iTop    = tClientRect.top;
        int iRight  = tClientRect.right;
        int iBottom = tClientRect.bottom;

        if (iLeft < 0)	iLeft = 0;
        if (iTop < 0)	iTop = 0;

        if (iRight >  m_tTemplateSize.cx)  iRight = m_tTemplateSize.cx;
        if (iBottom > m_tTemplateSize.cy) iBottom = m_tTemplateSize.cy;

        iLeft   = (int)((float)iLeft / m_fScale);
        iTop    = (int)((float)iTop / m_fScale);
        iRight  = (int)((float)iRight / m_fScale);
        iBottom = (int)((float)iBottom / m_fScale);

        tRect = cv::Rect(iLeft, iTop, iRight - iLeft + (int)(1 / m_fScale), iBottom - iTop + (int)(1 / m_fScale));
    }

    return tRect;
}

/*******************************************************
 * 功能：获取产品选中区域坐标
 * 参数：无
 * 返回值：坐标信息
*******************************************************/
cv::Rect MakeTemplateView::getProductSelectRect()
{
    cv::Rect tRect(0, 0, 0, 0);

    if (m_tPointBegin != m_tPointEnd) {
        float fScale = m_fScale * m_fProductScale;

        CRect tClientRect = CRect(m_tPointBegin, m_tPointEnd);
        tClientRect.NormalizeRect();

        int iLeft   = tClientRect.left;
        int iTop    = tClientRect.top;
        int iRight  = tClientRect.right;
        int iBottom = tClientRect.bottom;

        if (iLeft < m_iProductX)    iLeft = m_iProductX;
        if (iTop  < m_iProductY)	iTop  = m_iProductY;

        if (iRight  > (m_iProductX + m_tProductSize.cx)) iRight  = m_iProductX + m_tProductSize.cx;
        if (iBottom > (m_iProductY + m_tProductSize.cy)) iBottom = m_iProductY + m_tProductSize.cy;

        iLeft   = (int)((iLeft - m_iProductX) / fScale);
        iTop    = (int)((iTop - m_iProductY) / fScale);
        iRight  = (int)((iRight - m_iProductX) / fScale);
        iBottom = (int)((iBottom - m_iProductY) / fScale);

        tRect = cv::Rect(iLeft, iTop, iRight - iLeft + (int)(1 / fScale), iBottom - iTop + (int)(1 / fScale));
    }

    return tRect;
}

int MakeTemplateView::OnMouseActivate(CWnd* ptWnd, UINT uHitTest, UINT uMsg)
{
    return CWnd::OnMouseActivate(ptWnd, uHitTest, uMsg);
}

/*******************************************************
 * 功能：鼠标按下事件
 * 参数：...
 * 返回值：无
*******************************************************/
void MakeTemplateView::OnLButtonDown(UINT uFlags, CPoint tPoint)
{
    this->SetCapture();

    m_tPointBegin.SetPoint(tPoint.x + m_iScrollX, tPoint.y + m_iScrollY);
    
    if ((m_tPointBegin.x < m_iProductX) || (m_tPointBegin.y < m_iProductY)
         || (m_tPointBegin.x > (m_tProductSize.cx + m_iProductX))
         || (m_tPointBegin.y > (m_tProductSize.cy + m_iProductY))) {
        //MessageBox(L"框选区域不在贴图范围内!", L"生成模板", MB_OK | MB_ICONWARNING);

        m_tPointBegin.SetPoint(0, 0);
        m_tPointEnd.SetPoint(0, 0);

        return;
    }
    
    m_tPointEnd   = m_tPointBegin;
    m_bMousePressed = true;
}

/*******************************************************
 * 功能：鼠标移动事件
 * 参数：...
 * 返回值：无
*******************************************************/
void MakeTemplateView::OnMouseMove(UINT uFlags, CPoint tPoint)
{
    if (m_bMousePressed) {
        m_tPointEnd.SetPoint(tPoint.x + m_iScrollX, tPoint.y + m_iScrollY);

        if(m_tPointEnd.x < m_iProductX) m_tPointEnd.x = m_iProductX;
        if(m_tPointEnd.y < m_iProductY) m_tPointEnd.y = m_iProductY;

        if (m_tPointEnd.x > (m_tProductSize.cx + m_iProductX)) {
            m_tPointEnd.x = m_tProductSize.cx + m_iProductX;
        }

        if (m_tPointEnd.y > (m_tProductSize.cy + m_iProductY)) {
            m_tPointEnd.y = m_tProductSize.cy + m_iProductY;
        }

        Invalidate(NULL);
    }
}

/*******************************************************
 * 功能：鼠标抬起事件
 * 参数：...
 * 返回值：无
*******************************************************/
void MakeTemplateView::OnLButtonUp(UINT uFlags, CPoint tPoint)
{
    m_bMousePressed = false;

    ReleaseCapture();
}

/*******************************************************
 * 功能：鼠标右键抬起事件
 * 参数：...
 * 返回值：无
*******************************************************/
void MakeTemplateView::OnRButtonUp(UINT uFlags, CPoint tPoint)
{
    m_tPointBegin = CPoint(0, 0);
    m_tPointEnd   = m_tPointBegin;

    Invalidate(NULL);
}

/*******************************************************
 * 功能：鼠标滚动事件
 * 参数：...
 * 返回值：无
*******************************************************/
BOOL MakeTemplateView::OnMouseWheel(UINT uFlags, short sDelta, CPoint tPoint)
{
    if (uFlags & MK_CONTROL) {
        if (sDelta > 0) {
            m_fScale *= 1.1f;
        }
        else {
            m_fScale /= 1.1f;

            if (m_fScale < 0.1f) {
                m_fScale = 0.1f;
            }
        }

        reset();
        Invalidate(NULL);

        CScrollView::OnMouseWheel(uFlags, sDelta, tPoint);
    }

    return TRUE;
}

/*******************************************************
 * 功能：垂直滚动条事件
 * 参数：...
 * 返回值：无
*******************************************************/
void MakeTemplateView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);

    CPoint ptScroll = GetScrollPosition();
    m_iScrollX = ptScroll.x;

    CDC* ptDC = GetDC();
    CRect tClientRect;
    GetClientRect(&tClientRect);

    ptDC->BitBlt(0, 0, tClientRect.Width(), tClientRect.Height(), &m_tMemDC, ptScroll.x, ptScroll.y, SRCCOPY);
    ReleaseDC(ptDC);

    //Invalidate(FALSE);
}

/*******************************************************
 * 功能：水平滚动条事件
 * 参数：...
 * 返回值：无
*******************************************************/
void MakeTemplateView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);

    CPoint ptScroll = GetScrollPosition();
    m_iScrollY = ptScroll.y;

    CDC* ptDC = GetDC();
    CRect tClientRect;
    GetClientRect(&tClientRect);

    ptDC->BitBlt(0, 0, tClientRect.Width(), tClientRect.Height(), &m_tMemDC, ptScroll.x, ptScroll.y, SRCCOPY);
    ReleaseDC(ptDC);

    //Invalidate(FALSE);
}