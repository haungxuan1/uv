// MakeTemplateMaskDialog.cpp: 实现文件
//

#include "pch.h"
#include "VisionLocalization.h"
#include "afxdialogex.h"
#include "MakeTemplateMaskDialog.h"
#include "../Local Setting/Transform.h"
#include "../utils/FileUtil.h"

using namespace Transform;

IMPLEMENT_DYNAMIC(MakeTemplateMaskDialog, StyleDialog)

MakeTemplateMaskDialog::MakeTemplateMaskDialog(CWnd* pParent, string tTempFile)
	: StyleDialog(IDD_MAKE_TEMPMASK_DIALOG, pParent)
{
	m_iPointCount = 0;
	m_bDrawEnable = true;
	m_tTempFile = tTempFile;

	m_tTemplateMat = imread(m_tTempFile, cv::IMREAD_GRAYSCALE);
	if (!m_tTemplateMat.empty()) {
		cv::cvtColor(m_tTemplateMat, m_tTemplateMat, cv::COLOR_GRAY2BGRA);
	}

	SetCapture();
}

MakeTemplateMaskDialog::~MakeTemplateMaskDialog()
{
	ReleaseCapture();

	if (!m_tDispMat.empty())		m_tDispMat.release();
	if (!m_tTemplateMat.empty())	m_tTemplateMat.release();
}

void MakeTemplateMaskDialog::DoDataExchange(CDataExchange* pDX)
{
	StyleDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_RESET, m_tResetBtn);
	DDX_Control(pDX, IDOK, m_tOkBtn);
	DDX_Control(pDX, IDCANCEL, m_tCancelBtn);
}


BEGIN_MESSAGE_MAP(MakeTemplateMaskDialog, StyleDialog)
	//ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDCANCEL, &MakeTemplateMaskDialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_RESET, &MakeTemplateMaskDialog::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDOK, &MakeTemplateMaskDialog::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL MakeTemplateMaskDialog::OnInitDialog()
{
	StyleDialog::OnInitDialog();

	m_tBrush.CreateSolidBrush(RGB(0xf0, 0xf0, 0xf0));

	createBg();

	return TRUE;
}

BOOL MakeTemplateMaskDialog::PreTranslateMessage(MSG* ptMsg)
{
	if (ptMsg != nullptr) {
		if (ptMsg->message == WM_KEYUP) {
			if (ptMsg->wParam == VK_DELETE) {
				if (m_iPointCount > 0) {
					m_iPointCount--;

					drawPaint();
				}
			}
		}
	}

	return CDialog::PreTranslateMessage(ptMsg);
}

void MakeTemplateMaskDialog::createBg()
{
	//创建内存缓存DC
	CDC* ptDC = GetDC();
	if (ptDC != nullptr) {
		CRect tWinRect;

		GetClientRect(tWinRect);

		m_tRect.SetRect(0, 0, tWinRect.Width(), 2 * tWinRect.Height() / 3);

		m_tBmpCanvas.CreateCompatibleBitmap(ptDC, m_tRect.Width(), m_tRect.Height());

		m_tMemDC.CreateCompatibleDC(ptDC);
		m_tMemDC.SelectObject(&m_tBmpCanvas);

		m_tMemDC.FillRect(&m_tRect, &m_tBrush);

		ReleaseDC(ptDC);

		if (!m_tTemplateMat.empty()) {
			float fScaleX = (float)m_tRect.Width() / m_tTemplateMat.cols;
			float fScaleY = (float)m_tRect.Height() / m_tTemplateMat.rows;

			m_fScale = (fScaleX < fScaleY) ? fScaleX : fScaleY;

			resize(m_tTemplateMat, m_tDispMat, cv::Size(m_fScale * m_tTemplateMat.cols, m_fScale * m_tTemplateMat.rows), 0, 0, cv::INTER_AREA);

			m_iMatX = (m_tRect.Width() - m_tDispMat.cols) / 2;
			m_iMatY = (m_tRect.Height() - m_tDispMat.rows) / 2;

			m_tMemDC.FillRect(&m_tRect, &m_tBrush);

			int iPixelBytes = m_tDispMat.channels() * (m_tDispMat.depth() + 1);
			BITMAPINFO tBitmapInfo;

			memset((void*)&tBitmapInfo, 0, sizeof(tBitmapInfo));

			tBitmapInfo.bmiHeader.biBitCount = 8 * m_tDispMat.channels();

			tBitmapInfo.bmiHeader.biWidth = m_tDispMat.cols;
			tBitmapInfo.bmiHeader.biHeight = -m_tDispMat.rows;
			tBitmapInfo.bmiHeader.biPlanes = 1;
			tBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			tBitmapInfo.bmiHeader.biCompression = BI_RGB;
			tBitmapInfo.bmiHeader.biClrImportant = 0;
			tBitmapInfo.bmiHeader.biClrUsed = 0;
			tBitmapInfo.bmiHeader.biSizeImage = 0;
			tBitmapInfo.bmiHeader.biXPelsPerMeter = 0;
			tBitmapInfo.bmiHeader.biYPelsPerMeter = 0;

			::StretchDIBits(m_tMemDC, m_iMatX, m_iMatY, m_tDispMat.cols, m_tDispMat.rows,
				0, 0, m_tDispMat.cols, m_tDispMat.rows, m_tDispMat.data,
				&tBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		}

		drawPaint();

		Invalidate();
	}
}

void MakeTemplateMaskDialog::drawPaint(bool bMouseMove)
{
	CClientDC tClientDC(this);

	tClientDC.StretchBlt(m_tRect.left, m_tRect.top, m_tRect.Width(), m_tRect.Height(),
			&m_tMemDC, 0, 0, m_tRect.Width(), m_tRect.Height(), SRCCOPY);

	CPen tNewPen;
	tNewPen.CreatePen(PS_SOLID, 2, RGB(0, 200, 0));
	CPen* ptOldPen = (CPen*)tClientDC.SelectObject(&tNewPen);

	int iPointCount = m_iPointCount;

	if (bMouseMove && m_bDrawEnable) {
		iPointCount++;
	}

	if (iPointCount > 1) {
		if (iPointCount < MAX_POINT_COUNT) {
			m_tPoints[iPointCount] = m_tPoints[0];

			iPointCount++;
		}

		tClientDC.Polyline(m_tPoints, iPointCount);
	}
	else if (1 == iPointCount) {
		tClientDC.SetPixel(m_tPoints[0], RGB(0, 200, 0));
	}

	tClientDC.SelectObject(ptOldPen);
}

void MakeTemplateMaskDialog::OnLButtonDown(UINT uFlags, CPoint tPoint)
{
	m_bMousePress = true;

	drawPaint();
}

void MakeTemplateMaskDialog::OnMouseMove(UINT uFlags, CPoint tPoint)
{
	if (m_iPointCount < MAX_POINT_COUNT) {
		int iRight = m_iMatX + m_tDispMat.cols;

		if (tPoint.x < m_iMatX)					tPoint.x = m_iMatX;
		if (tPoint.x >= iRight)					tPoint.x = iRight - 1;
		if (tPoint.y < 0)						tPoint.y = 0;
		if (tPoint.y > (m_tDispMat.rows - 1))	tPoint.y = m_tDispMat.rows - 1;

		m_tPoints[m_iPointCount] = tPoint;

		if (m_bDrawEnable) {
			drawPaint(true);
		}
	}
}

void MakeTemplateMaskDialog::OnLButtonUp(UINT uFlags, CPoint tPoint)
{
	m_bMousePress = false;

	if (m_iPointCount < MAX_POINT_COUNT) {
		int iRight = m_iMatX + m_tDispMat.cols;

		if (tPoint.x < m_iMatX)					tPoint.x = m_iMatX;
		if (tPoint.x >= iRight)					tPoint.x = iRight - 1;
		if (tPoint.y < 0)						tPoint.y = 0;
		if (tPoint.y > (m_tDispMat.rows - 1))	tPoint.y = m_tDispMat.rows -1;

		m_tPoints[m_iPointCount] = tPoint;

		m_iPointCount++;

		drawPaint();
	}
}

void MakeTemplateMaskDialog::OnRButtonUp(UINT uFlags, CPoint tPoint)
{
	//_cwprintf(L"%s\r\n", __FUNCTIONW__);

	m_bDrawEnable = false;

	drawPaint();
}

//取消按钮事件
void MakeTemplateMaskDialog::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

//重绘按钮事件
void MakeTemplateMaskDialog::OnBnClickedBtnReset()
{
	m_iPointCount = 0;
	m_bDrawEnable = true;

	drawPaint();
}

//确定按钮事件
void MakeTemplateMaskDialog::OnBnClickedOk()
{
	if (m_iPointCount > 0) {
		CPoint tPoint;
		std::vector<cv::Point> tVecPoints;

		for (int i = 0; i < m_iPointCount; i++) {
			tPoint = m_tPoints[i];

			tPoint.x -= m_iMatX;
			tPoint.y -= m_iMatY;

			tPoint.x /= m_fScale;
			tPoint.y /= m_fScale;

			tVecPoints.push_back(cv::Point(tPoint.x, tPoint.y));
		}

		Mat tMaskMat(m_tTemplateMat.size(), CV_8UC1, cv::Scalar(0));

		// 将多边形填充到 Mask 中，填充为 255（白色）
		cv::fillPoly(tMaskMat, std::vector<std::vector<cv::Point>>{tVecPoints}, cv::Scalar(255));

		CString tFileName = FileUtil::getFileNameWithoutSuffix(String2Wstring(m_tTempFile).c_str());

		tFileName += "_mask.png";

		DeleteFile(tFileName);

		if (imwrite(CString2String(tFileName), tMaskMat)){
			MessageBox(TEXT("制作完成！"), TEXT("制作模板掩码"), MB_OK | MB_ICONINFORMATION);

			EndDialog(IDOK);
		}
		else {
			MessageBox(TEXT("保存失败！"), TEXT("制作模板掩码"), MB_OK | MB_ICONWARNING);
		}
	}
	else {
		MessageBox(TEXT("没有选中区域，请选取后再操作！"), TEXT("制作模板掩码"), MB_OK | MB_ICONWARNING);
	}
}
