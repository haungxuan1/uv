#include "pch.h"
#include "StyleDialog.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(StyleDialog, CDialog)

/////////////////////////////////////////////////////////////////////////////
// StyleDialog dialog

StyleDialog::StyleDialog(UINT uTemplateId, CWnd* ptParent)
	: CDialog(uTemplateId, ptParent)
{

}

StyleDialog::~StyleDialog()
{
}

void StyleDialog::DoDataExchange(CDataExchange* ptDX)
{
	CDialog::DoDataExchange(ptDX);
}

BEGIN_MESSAGE_MAP(StyleDialog, CDialog)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCMOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL StyleDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}
BOOL StyleDialog::PreTranslateMessage(MSG* ptMsg)
{
	if (ptMsg != nullptr) {
		switch (ptMsg->message) {
			case VK_RETURN:		return TRUE;
			case VK_ESCAPE:		return TRUE;
			default:			break;
		}
	}

	return CDialog::PreTranslateMessage(ptMsg);
}

//�Ի��򴰿�ȱʡ��Ϣ����
LRESULT StyleDialog::DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CDialog::DefWindowProc(uMsg, wParam, lParam);

	if (!::IsWindow(m_hWnd)) {
		return lResult;
	}

	switch (uMsg) {
		case WM_MOVE:
		case WM_PAINT:
		case WM_NCPAINT:
		case WM_NCACTIVATE:
		case WM_NOTIFY:
			try {
				CDC* ptWinDC = GetWindowDC();

				if (ptWinDC != nullptr) {
					drawTitleBar(ptWinDC);

					ReleaseDC(ptWinDC);
				}
			}catch (CException* Exception) {
				Exception->Delete();
			}
			break;

		default:
			break;
	}

	return lResult;
}

void StyleDialog::setMainDialogFlag(bool bIsMainDlg)
{
	m_bIsMainDlg = bIsMainDlg;
}

// �� �� ����DrawTitleBar
// �������������Ʊ��������߿���ɫ�����Ʊ������ݡ�ͼ��Ͱ�ť
void StyleDialog::drawTitleBar(CDC* ptDC)
{
	if (m_hWnd && (ptDC != nullptr)) {
		CBrush Brush(RGB(87, 100, 143));
		CBrush* ptOldBrush;
		CRect tWndRect;
		CRect tTitleRect;
		CRect tButtonRect;

		ptOldBrush = ptDC->SelectObject(&Brush);

		GetWindowRect(&tWndRect);

		//ȡ�ñ�������λ��
		  //SM_CXFRAME ���ڱ߿�ı�Ե����
		  //SM_CYFRAME ���ڱ߿�ı�Ե�߶�
		  //SM_CXSIZE  ���ڱ���������
		  //SM_CYSIZE  ���ڱ������߶�
		tTitleRect.left = GetSystemMetrics(SM_CXFRAME);
		tTitleRect.top = GetSystemMetrics(SM_CYFRAME);
		tTitleRect.right = tWndRect.right - tWndRect.left;
		tTitleRect.bottom = tTitleRect.top + GetSystemMetrics(SM_CYSIZE) + 2 * GetSystemMetrics(SM_CYFRAME);

		int iDy;
		int iWid;
		int iHei;

		//��䶥�����
		//ptDC->PatBlt(0, 0, tTitleRect.Width(), tTitleRect.Height(), PATCOPY);
		Gdiplus::Graphics tGraphics(ptDC->m_hDC);
		Gdiplus::Point tStartPoint(0, 0);
		Gdiplus::Point tEndPoint(tTitleRect.right, tTitleRect.top);
		Gdiplus::LinearGradientBrush tGradientBrush(
			tStartPoint,
			tEndPoint,
			Gdiplus::Color(255, 50, 128, 50),  // ��ʼ��ɫ
			Gdiplus::Color(255, 50, 50, 200)    // ������ɫ
		);

		if (m_bIsMainDlg) {
			tGradientBrush.SetLinearColors(Gdiplus::Color(255, 16, 87, 151),
										   Gdiplus::Color(255, 86, 127, 200));
		}

		// ʹ�ý��仭ˢ������
		tGraphics.FillRectangle(&tGradientBrush, 0, 0, tTitleRect.Width(), tTitleRect.Height());
		//��������
		iWid = GetSystemMetrics(SM_CXFRAME) - 1;
		//ptDC->PatBlt(0, 0, iWid, tWndRect.Height() - 1, PATCOPY);
		tGraphics.FillRectangle(&tGradientBrush, 0, 0, iWid, tWndRect.Height() - 1);
		//���ײ����
		iHei = GetSystemMetrics(SM_CYFRAME);
		//ptDC->PatBlt(0, tWndRect.Height() - iHei, tWndRect.Width(), iHei, PATCOPY);
		tGraphics.FillRectangle(&tGradientBrush, 0, tWndRect.Height() - iHei, tWndRect.Width(), iHei);
		//����Ҳ���
		iWid = GetSystemMetrics(SM_CXFRAME);
		//ptDC->PatBlt(tWndRect.Width() - iWid, 0, iWid, tWndRect.Height(), PATCOPY);
		tGraphics.FillRectangle(&tGradientBrush, tWndRect.Width() - iWid, 0, iWid, tWndRect.Height());

		//�ػ�������ͼ��
		iDy = (tTitleRect.Height() - 32) / 2;
		m_tRectIcon.left = tTitleRect.left + 5;
		m_tRectIcon.top = tTitleRect.top + iDy;
		m_tRectIcon.right = m_tRectIcon.left + 32;
		m_tRectIcon.bottom = iDy + 32;
		::DrawIconEx(ptDC->m_hDC, m_tRectIcon.left, m_tRectIcon.top, AfxGetApp()->LoadIcon(IDI_ICON),
			m_tRectIcon.Width(), m_tRectIcon.Height(), 0, NULL, DI_IMAGE);
		m_tRectIcon.OffsetRect(tWndRect.TopLeft());

		CBitmap* ptBitmap = new CBitmap;
		CBitmap* pOldBitmap;
		CDC* ptDisplayMemDC = new CDC;

		ptDisplayMemDC->CreateCompatibleDC(ptDC);

		//�ػ��ر�button
		iDy = 6;
		tButtonRect.left = tTitleRect.right - 32;
		tButtonRect.top = tTitleRect.top + iDy;
		tButtonRect.right = tButtonRect.left + 16;
		tButtonRect.bottom = tButtonRect.top + 15;
		ptBitmap->LoadBitmap(IDB_EXIT_FOCUS);
		pOldBitmap = (CBitmap*)ptDisplayMemDC->SelectObject(ptBitmap);
		ptDC->BitBlt(tButtonRect.left, tButtonRect.top, tButtonRect.Width(), tButtonRect.Height(), ptDisplayMemDC, 0, 0, SRCCOPY);
		ptDisplayMemDC->SelectObject(pOldBitmap);
		m_tRectBtnExit = tButtonRect;
		m_tRectBtnExit.OffsetRect(tWndRect.TopLeft());
		ptBitmap->DeleteObject();

		//�ػ����/�ָ�button
		tButtonRect.right = tButtonRect.left - 3;
		tButtonRect.left = tButtonRect.right - 16;

		if (IsZoomed()) {
			ptBitmap->LoadBitmap(IDB_RESTORE_NORMAL);
		}
		else {
			ptBitmap->LoadBitmap(IDB_MAX_NORMAL);
		}

		pOldBitmap = (CBitmap*)ptDisplayMemDC->SelectObject(ptBitmap);
		ptDC->BitBlt(tButtonRect.left, tButtonRect.top, tButtonRect.Width(), tButtonRect.Height(), ptDisplayMemDC, 0, 0, SRCCOPY);
		ptDisplayMemDC->SelectObject(pOldBitmap);
		m_tRectBtnMax = tButtonRect;
		m_tRectBtnMax.OffsetRect(tWndRect.TopLeft());
		ptBitmap->DeleteObject();

		//�ػ���С��button
		tButtonRect.right = tButtonRect.left - 3;
		tButtonRect.left = tButtonRect.right - 16;
		ptBitmap->LoadBitmap(IDB_MIN_NORMAL);
		pOldBitmap = (CBitmap*)ptDisplayMemDC->SelectObject(ptBitmap);
		ptDC->BitBlt(tButtonRect.left, tButtonRect.top, tButtonRect.Width(), tButtonRect.Height(), ptDisplayMemDC, 0, 0, SRCCOPY);
		ptDisplayMemDC->SelectObject(pOldBitmap);
		m_tRectBtnMin = tButtonRect;
		m_tRectBtnMin.OffsetRect(tWndRect.TopLeft());
		ptBitmap->DeleteObject();

		//�ػ�caption
		int nOldMode = ptDC->SetBkMode(TRANSPARENT);
		COLORREF clOldText = ptDC->SetTextColor(RGB(255, 255, 255));

		CFont m_captionFont;
		m_captionFont.CreateFont(
			18,                        // ����ĸ߶�			
			0,                         // ����Ŀ���
			0,                         // ������ʾ�ĽǶ�
			0,                         // ����ĽǶ�
			FW_BOLD,                   // ����İ���
			FALSE,                     // б������
			FALSE,                     // ���»��ߵ�����
			0,                         // ��ɾ���ߵ�����
			ANSI_CHARSET,              // ������ַ���
			OUT_DEFAULT_PRECIS,        // ����ľ���
			CLIP_DEFAULT_PRECIS,       // �ü��ľ���
			DEFAULT_QUALITY,           // �߼�����������豸��ʵ������֮��ľ���
			DEFAULT_PITCH | FF_SWISS,  // ����������弯
			_T("Arial"));              // ��������

		CFont* pOldFont = NULL;
		pOldFont = ptDC->SelectObject(&m_captionFont);

		CString tStrTitle;
		GetWindowText(tStrTitle);

		tTitleRect.left += m_tRectIcon.Width() + 10;
		tTitleRect.top = 12;
		tTitleRect.right = 1000;
		tTitleRect.bottom = tTitleRect.top + 32;

		//ptDC->DrawText(tStrTitle, &tTitleRect, DT_LEFT);
		//ptDC->SetBkMode(nOldMode);

		tGraphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		tGraphics.FillRectangle(&tGradientBrush, tWndRect.Width() - iWid, 0, iWid, tWndRect.Height());

		Gdiplus::Font tFont(L"Arial", 10);
		Gdiplus::SolidBrush tSolidBrush(Gdiplus::Color(200, 200, 200));

		// ���ƿ�����ı�
		tGraphics.DrawString(tStrTitle, -1, &tFont, Gdiplus::PointF(tTitleRect.left, tTitleRect.top), &tSolidBrush);

		ptDC->SetTextColor(clOldText);

		ptDC->SelectObject(ptOldBrush);

		ReleaseDC(ptDisplayMemDC);

		delete ptDisplayMemDC;
		delete ptBitmap;
	}
}

//��䱳��
BOOL StyleDialog::OnEraseBkgnd(CDC* ptDC)
{
	BOOL bRet = CDialog::OnEraseBkgnd(ptDC);

	CRect tRect;
	GetClientRect(&tRect);
	//ptDC->FillSolidRect(&tRect, RGB(50, 100, 143));

	if (m_bIsMainDlg) {
		ptDC->FillSolidRect(&tRect, RGB(112, 146, 190));
	}
	else {
		ptDC->FillSolidRect(&tRect, RGB(200, 200, 230));
	}

	return bRet;
}

//�ı䴰�ڱ߿�ΪԲ��
int StyleDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rtWnd;
	GetWindowRect(&rtWnd);

	CRgn rgn;
	rgn.CreateRoundRectRgn(0, 0, rtWnd.Width(), rtWnd.Height(), 25, 25);
	SetWindowRgn((HRGN)rgn, true);

	return 0;
}

HBRUSH StyleDialog::OnCtlColor(CDC* ptDC, CWnd* ptWnd, UINT uCtlColor)
{
	HBRUSH hBrush = CDialog::OnCtlColor(ptDC, ptWnd, uCtlColor);

	//static��ɫ
	/*if (ptWnd->GetDlgCtrlID() == IDC_STATIC) {
		ptDC->SetTextColor(RGB(0, 100, 0));
		ptDC->SetBkMode(TRANSPARENT);

		hBrush = CreateSolidBrush(RGB(236, 233, 216));
	}*/

	switch (uCtlColor){
		case CTLCOLOR_STATIC:
			if (!dynamic_cast<CEdit*>(ptWnd) && !dynamic_cast<CSliderCtrl*>(ptWnd) && !dynamic_cast<CButton*>(ptWnd)) {
			//if (ptWnd->GetDlgCtrlID() == IDC_STATIC){
				ptDC->SetBkMode(TRANSPARENT);

				return (HBRUSH)GetStockObject(NULL_BRUSH);
				//return CreateSolidBrush(RGB(236, 233, 216));
			}
			break;

		default:
			break;
	}

	return hBrush;
}

// �� �� ����OnNcLButtonDown
// �����������ڷǿͻ������� ������Ϣ
void StyleDialog::OnNcLButtonDown(UINT uHitTest, CPoint tPoint)
{
	if (m_tRectBtnExit.PtInRect(tPoint)) {						//�ر�
		SendMessage(WM_CLOSE);
	}

	else if (m_tRectBtnMin.PtInRect(tPoint)) {					//��С��
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, MAKELPARAM(tPoint.x, tPoint.y));
	}

	else if (m_tRectBtnMax.PtInRect(tPoint)) {
		CRgn tRgn;
		CRect tWndRect;

		if (IsZoomed()) { //���
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, MAKELPARAM(tPoint.x, tPoint.y));
			
			GetWindowRect(&tWndRect);
			
			tRgn.CreateRoundRectRgn(0, 0, tWndRect.Width(), tWndRect.Height(), 25, 25);

			SetWindowRgn((HRGN)tRgn, true);
			Invalidate();
		}
		else {
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, MAKELPARAM(tPoint.x, tPoint.y));

			GetWindowRect(&tWndRect);
			tRgn.CreateRoundRectRgn(0, 0, tWndRect.Width(), tWndRect.Height(), 25, 25);

			SetWindowRgn((HRGN)tRgn, true);
			Invalidate();
		}
	}
	else if (!IsZoomed()) {
		Default();
	}
}

// �� �� ����OnNcMouseMove
// �����������ڷǿͻ��� ����ƶ� ������Ϣ
void StyleDialog::OnNcMouseMove(UINT uHitTest, CPoint tPoint)
{
	CWindowDC tWindowDC(this);
	CDC* ptDisplayMemDC = new CDC;

	if(ptDisplayMemDC != nullptr){
		CBitmap* ptBitmap = new CBitmap;

		if (ptBitmap != nullptr) {
			CRect tBtnRect;
			CRect tWndRect;
			CBitmap* ptOldBitmap;

			ptDisplayMemDC->CreateCompatibleDC(&tWindowDC);

			GetWindowRect(&tWndRect);

			//�ر�button
			if (m_tRectBtnExit.PtInRect(tPoint))
				ptBitmap->LoadBitmap(IDB_EXIT_NORMAL);
			else
				ptBitmap->LoadBitmap(IDB_EXIT_FOCUS);

			tBtnRect = m_tRectBtnExit;
			tBtnRect.OffsetRect(-tWndRect.left, -tWndRect.top);
			ptOldBitmap = (CBitmap*)ptDisplayMemDC->SelectObject(ptBitmap);
			tWindowDC.BitBlt(tBtnRect.left, tBtnRect.top, tBtnRect.Width(), tBtnRect.Height(), ptDisplayMemDC, 0, 0, SRCCOPY);
			ptDisplayMemDC->SelectObject(ptOldBitmap);
			ptBitmap->DeleteObject();

			//���/�ָ�button
			if (m_tRectBtnMax.PtInRect(tPoint)){
				ptBitmap->LoadBitmap(IsZoomed() ? IDB_RESTORE_FOCUS : IDB_MAX_FOCUS);
			}
			else{
				ptBitmap->LoadBitmap(IsZoomed() ? IDB_RESTORE_NORMAL : IDB_MAX_NORMAL);
			}

			tBtnRect = m_tRectBtnMax;
			tBtnRect.OffsetRect(-tWndRect.left, -tWndRect.top);
			ptOldBitmap = (CBitmap*)ptDisplayMemDC->SelectObject(ptBitmap);
			tWindowDC.BitBlt(tBtnRect.left, tBtnRect.top, tBtnRect.Width(), tBtnRect.Height(), ptDisplayMemDC, 0, 0, SRCCOPY);
			ptDisplayMemDC->SelectObject(ptOldBitmap);
			ptBitmap->DeleteObject();

			//��С��button
			if (m_tRectBtnMin.PtInRect(tPoint)) {
				ptBitmap->LoadBitmap(IDB_MIN_FOCUS);
			}
			else {
				ptBitmap->LoadBitmap(IDB_MIN_NORMAL);
			}
			tBtnRect = m_tRectBtnMin;
			tBtnRect.OffsetRect(-tWndRect.left, -tWndRect.top);
			ptOldBitmap = (CBitmap*)ptDisplayMemDC->SelectObject(ptBitmap);
			tWindowDC.BitBlt(tBtnRect.left, tBtnRect.top, tBtnRect.Width(), tBtnRect.Height(), ptDisplayMemDC, 0, 0, SRCCOPY);
			ptDisplayMemDC->SelectObject(ptOldBitmap);
			ptBitmap->DeleteObject();

			delete ptBitmap;
		}

		ptDisplayMemDC->DeleteDC();
		delete ptDisplayMemDC;
	}

	CDialog::OnNcMouseMove(uHitTest, tPoint);
}

