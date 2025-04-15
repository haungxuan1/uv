// MakeTemplateDialog.cpp: 实现文件
//

#include "pch.h"
#include "def.h"
#include "resource.h"
#include "VisionLocalization.h"
#include "afxdialogex.h"
#include "MakeTemplateDialog.h"
#include "../utils/FileUtil.h"
#include "../utils/TiffUtil.h"
#include "../utils/OpenCvUtil.h"
#include "../Local Setting/Transform.h"
#include "./Temp/TemplateManager.h"
#include <tiffio.h>

#ifdef USE_HALCON
#include "FastHalMatch.h"
#else
#include "FastMatch.h"
#endif

using namespace Transform;

IMPLEMENT_DYNAMIC(MakeTemplateDialog, StyleDialog)

MakeTemplateDialog::MakeTemplateDialog(CWnd* ptParent, Mat& tTemplateMat, Mat& tProductMat, CString tProductPath, bool bParentIsManager)
	: StyleDialog(IDD_MAKE_TEMPLATE_DIALOG, ptParent)
{
	m_tProductMat  = tProductMat;
	m_tTemplateMat = tTemplateMat;

	//m_tProductPath = CString2String(tProductPath);
	m_tSavePath    = CString2String(FileUtil::getPath(tProductPath));

	m_bParentIsManager = bParentIsManager;
}

MakeTemplateDialog::~MakeTemplateDialog()
{
	if (!m_tTemplateMat.empty())		m_tTemplateMat.release();
	if (!m_tProductMat.empty())			m_tProductMat.release();
}

void MakeTemplateDialog::DoDataExchange(CDataExchange* pDX)
{
	StyleDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SLIDER_ALPHA, m_tSliderAlpha);
	DDX_Control(pDX, IDC_SLIDER_CONTRAST_RATIO, m_tSliderContrastRatio);

	DDX_Control(pDX, IDC_CMB_SCALE, m_tCmbScale);
	DDX_Control(pDX, IDC_CMB_MOVE_STEP, m_tCmbMoveStep);
	DDX_Control(pDX, IDC_CMB_SCALE_STEP, m_tCmbScaleStep);
	DDX_Control(pDX, IDC_CMB_ROTATE_STEP, m_tCmbRotateStep);
	DDX_Control(pDX, IDC_BTN_AUTO_POSITION, m_tAutoPositionBtn);
	DDX_Control(pDX, IDC_BTN_RESET, m_tResetBtn);
	DDX_Control(pDX, IDC_BTN_MAKE, m_tMakeBtn);
	DDX_Control(pDX, IDCANCEL, m_tCancelBtn);
	DDX_Control(pDX, IDC_BTN_ROTATE_INC, m_tRotateIncBtn);
	DDX_Control(pDX, IDC_BTN_ROTATE_DEC, m_tRotateDecBtn);
	DDX_Control(pDX, IDC_BTN_SCALE_INC, m_tScaleIncBtn);
	DDX_Control(pDX, IDC_BTN_SCALE_DEC, m_tScaleDecBtn);
	DDX_Control(pDX, IDC_BTN_MOVE_UP, m_tMoveUpBtn);
	DDX_Control(pDX, IDC_BTN_MOVE_DOWN, m_tMoveDownBtn);
	DDX_Control(pDX, IDC_BTN_MOVE_RIGHT, m_tMoveRightBtn);
	DDX_Control(pDX, IDC_BTN_MOVE_LEFT, m_tMoveLeftBtn);
	DDX_Control(pDX, IDC_EDIT_NAME, m_tTemplateName);
}

BEGIN_MESSAGE_MAP(MakeTemplateDialog, StyleDialog)
	ON_BN_CLICKED(IDC_BTN_MAKE,       &MakeTemplateDialog::OnBnClickedBtnMake)
	ON_BN_CLICKED(IDC_BTN_ROTATE_INC, &MakeTemplateDialog::OnBnClickedBtnRotateInc)
	ON_BN_CLICKED(IDC_BTN_ROTATE_DEC, &MakeTemplateDialog::OnBnClickedBtnRotateDec)
	ON_BN_CLICKED(IDC_BTN_SCALE_INC,  &MakeTemplateDialog::OnBnClickedBtnScaleInc)
	ON_BN_CLICKED(IDC_BTN_SCALE_DEC,  &MakeTemplateDialog::OnBnClickedBtnScaleDec)
	ON_BN_CLICKED(IDC_BTN_RESET,      &MakeTemplateDialog::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_MOVE_LEFT,  &MakeTemplateDialog::OnBnClickedBtnMoveLeft)
	ON_BN_CLICKED(IDC_BTN_MOVE_RIGHT, &MakeTemplateDialog::OnBnClickedBtnMoveRight)
	ON_BN_CLICKED(IDC_BTN_MOVE_UP,    &MakeTemplateDialog::OnBnClickedBtnMoveUp)
	ON_BN_CLICKED(IDC_BTN_MOVE_DOWN,  &MakeTemplateDialog::OnBnClickedBtnMoveDown)
	ON_CBN_SELCHANGE(IDC_CMB_SCALE,   &MakeTemplateDialog::OnCbnSelchangeCmbScale)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ALPHA,          &MakeTemplateDialog::OnNMCustomdrawSliderAlpha)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CONTRAST_RATIO, &MakeTemplateDialog::OnNMCustomdrawSliderContrastRatio)
	ON_BN_CLICKED(IDC_BTN_AUTO_POSITION, &MakeTemplateDialog::OnBnClickedBtnAutoPosition)
END_MESSAGE_MAP()

BOOL MakeTemplateDialog::OnInitDialog()
{
	StyleDialog::OnInitDialog();

	//产品透明度
	m_tSliderAlpha.SetRange(0, 255);
	m_tSliderAlpha.SetTicFreq(1);
	m_tSliderAlpha.SetTic(60);
	m_tSliderAlpha.SetPos(128);

	//模板对比度
	m_tSliderContrastRatio.SetRange(0, 500);
	m_tSliderContrastRatio.SetTicFreq(1);
	m_tSliderContrastRatio.SetTic(100);
	m_tSliderContrastRatio.SetPos(100);

	//缩放比例
	CString tScaleStr;
	vector<float> tScaleList = { 1.0f, 0.5f, 0.25f, 0.125f, 0.0625f, 0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f, 0.08f, 0.05f, 0.03f, 0.01f };

	for (size_t i = 0; i < tScaleList.size(); i++) {
		tScaleStr.Format(L"%g%%", tScaleList[i] * 100);

		m_tCmbScale.InsertString(static_cast<int>(i), tScaleStr);
	}
	m_tCmbScale.SetCurSel(-1);
	/*m_tCmbScale.InsertString(0, L"100%");
	m_tCmbScale.InsertString(1, L"50%");
	m_tCmbScale.InsertString(2, L"25%");
	m_tCmbScale.InsertString(3, L"12.5%");
	m_tCmbScale.InsertString(4, L"6.25%");
	m_tCmbScale.InsertString(5, L"90%");
	m_tCmbScale.InsertString(6, L"80%");
	m_tCmbScale.InsertString(7, L"70%");
	m_tCmbScale.InsertString(8, L"60%");
	m_tCmbScale.InsertString(9, L"50%");
	m_tCmbScale.InsertString(10, L"40%");
	m_tCmbScale.InsertString(11, L"30%");
	m_tCmbScale.InsertString(12, L"20%");
	m_tCmbScale.InsertString(13, L"10%");
	m_tCmbScale.InsertString(14, L"8%");
	m_tCmbScale.InsertString(15, L"5%");
	m_tCmbScale.InsertString(16, L"3%");
	m_tCmbScale.InsertString(17, L"1%");
	m_tCmbScale.SetCurSel(-1);*/

	CWnd* pWnd = this->GetDlgItem(IDC_IMAGE_STATIC);
	if (pWnd != nullptr) {
		CRect tRect;

		pWnd->GetWindowRect(&tRect);
		this->ScreenToClient(tRect);

		float fScale = std::min({ 1.0f * tRect.Width() / m_tTemplateMat.cols, 1.0f * tRect.Height() / m_tTemplateMat.rows, 
								  1.0f * tRect.Width() / m_tProductMat.cols, 1.0f * tRect.Height() / m_tProductMat.rows });

		fScale = std::floor(fScale * 1000.0f) / 1000.0f;

		vector<float> tTempList = { 0.01f, 0.03f, 0.05f, 0.0625f, 0.08f, 0.1f, 0.125f, 0.2f, 0.25f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f };
		auto fItem = std::upper_bound(tTempList.begin(), tTempList.end(), fScale);
		
		if (fItem == tTempList.end()) {
			fScale = 1.0f;
		} else if (fItem != tTempList.begin()) {
			--fItem;
			fScale = *fItem;
		} else {
			fScale = 0.01f;
		}

		auto fSelItem = std::find(tScaleList.begin(), tScaleList.end(), fScale);
		int iSelIndex = std::distance(tScaleList.begin(), fSelItem);
		m_tCmbScale.SetCurSel(iSelIndex);

		m_ptMakeTemplateView = (MakeTemplateView*)RUNTIME_CLASS(MakeTemplateView)->CreateObject();

		m_ptMakeTemplateView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW | WS_VSCROLL | WS_HSCROLL, tRect, this, IDC_IMAGE_STATIC);
		m_ptMakeTemplateView->init(m_tTemplateMat, m_tProductMat, fScale);
		m_ptMakeTemplateView->setAlpha(128);
	}

	//旋转步长
	m_tCmbRotateStep.InsertString(0, L"0.01");
	m_tCmbRotateStep.InsertString(1, L"0.1");
	m_tCmbRotateStep.InsertString(2, L"1");
	m_tCmbRotateStep.InsertString(3, L"5");
	m_tCmbRotateStep.InsertString(4, L"10");
	m_tCmbRotateStep.SetCurSel(2);

	//移动步长
	m_tCmbMoveStep.InsertString(0, L"0.1");
	m_tCmbMoveStep.InsertString(1, L"1");
	m_tCmbMoveStep.InsertString(2, L"5");
	m_tCmbMoveStep.InsertString(3, L"10");
	m_tCmbMoveStep.SetCurSel(1);

	//缩放步长
	m_tCmbScaleStep.InsertString(0, L"0.01");
	m_tCmbScaleStep.InsertString(1, L"0.1");
	m_tCmbScaleStep.InsertString(2, L"0.2");
	m_tCmbScaleStep.InsertString(3, L"0.3");
	m_tCmbScaleStep.InsertString(4, L"0.4");
	m_tCmbScaleStep.InsertString(5, L"0.5");
	m_tCmbScaleStep.SetCurSel(1);

	initStyle();

	return TRUE;
}

//初始化样式
void MakeTemplateDialog::initStyle()
{
	StyleButton* ptBtns[] = {
		&m_tMoveUpBtn,    &m_tMoveDownBtn,  &m_tMoveRightBtn, &m_tMoveLeftBtn,
		&m_tRotateIncBtn, &m_tRotateDecBtn, &m_tScaleIncBtn,  &m_tScaleDecBtn
	};

	for (int i = 0; i < sizeof(ptBtns) / sizeof(ptBtns[0]); i++) {
		ptBtns[i]->setRadius(8);

		ptBtns[i]->setBeginColor(Gdiplus::Color(255, 0, 30, 128));
		ptBtns[i]->setEndColor(Gdiplus::Color(255, 64, 200, 255));
		ptBtns[i]->setBorderColor(Gdiplus::Color(255, 50, 50, 50));
		ptBtns[i]->setTextColor(Gdiplus::Color(255, 250, 250, 250));
	}
}

BOOL MakeTemplateDialog::PreTranslateMessage(MSG* ptMsg)
{
	if ((ptMsg != nullptr) && (m_ptMakeTemplateView != nullptr)) {
		if (WM_KEYDOWN == ptMsg->message) {
			switch (ptMsg->wParam) {
				case VK_LEFT:	m_ptMakeTemplateView->setOffsetX(-getCmbMoveStep());	return TRUE;
				case VK_RIGHT:	m_ptMakeTemplateView->setOffsetX(getCmbMoveStep());		return TRUE;
				case VK_UP:		m_ptMakeTemplateView->setOffsetY(-getCmbMoveStep());	return TRUE;
				case VK_DOWN:	m_ptMakeTemplateView->setOffsetY(getCmbMoveStep());		return TRUE;

				case 'J':	m_ptMakeTemplateView->addAngle(getCmbRotateStep());				return TRUE;		//正向旋转
				case 'K':	m_ptMakeTemplateView->addAngle(getCmbRotateStep(true));			return TRUE;		//反向旋转
				case 'N':	m_ptMakeTemplateView->addProductScale(getCmbScaleStep());		return TRUE;		//放大
				case 'M':	m_ptMakeTemplateView->addProductScale(getCmbScaleStep(true));	return TRUE;		//缩小
				default:																	break;
			}
		}
	}

	return CDialog::PreTranslateMessage(ptMsg);
}

void MakeTemplateDialog::OnNMCustomdrawSliderAlpha(NMHDR* ptNMHDR, LRESULT* ptResult)
{
	LPNMCUSTOMDRAW ptNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(ptNMHDR);

	if (ptNMCD != nullptr) {
		m_ptMakeTemplateView->setAlpha(m_tSliderAlpha.GetPos());
	}

	*ptResult = 0;
}

//生成模板按钮事件
void MakeTemplateDialog::OnBnClickedBtnMake()
{
	try {
		CString tTemplateName;

		m_tTemplateName.GetWindowText(tTemplateName);

		TemplateManager* ptTemplateManager = TemplateManager::instance();

		if (tTemplateName.IsEmpty()) {
			MessageBox(L"模版名称不能为空!", L"生成模板", MB_OK | MB_ICONWARNING);
			m_tTemplateName.SetFocus();

			return;
		}
		else if (ptTemplateManager->isExist(CString2String(tTemplateName))) {
			MessageBox(L"模版名称已存在，请重新命名!", L"生成模板", MB_OK | MB_ICONWARNING);
			m_tTemplateName.SetFocus();

			return;
		}

		if (m_tTemplateMat.empty()) {
			MessageBox(L"模板为空，无法生成模板!", L"生成模板", MB_OK | MB_ICONWARNING);

			return;
		}

		if (m_tProductMat.empty()) {
			MessageBox(L"产品为空，无法生成模板!", L"生成模板", MB_OK | MB_ICONWARNING);

			return;
		}

		if (m_ptMakeTemplateView != nullptr) {
			cv::Rect tTemplateRect = m_ptMakeTemplateView->getTemplateSelectRect();
			if (tTemplateRect.empty()) {
				MessageBox(L"模板选定区域为空，无法生成模板!", L"生成模板", MB_OK | MB_ICONWARNING);

				return;
			}

			cv::Rect tProductRect = m_ptMakeTemplateView->getProductSelectRect();
			if (tProductRect.empty()) {
				MessageBox(L"产品选定区域为空，无法生成模板!", L"生成模板", MB_OK | MB_ICONWARNING);

				return;
			}

			//重新生成模板
			Rect tBgRect(0, 0, m_tTemplateMat.cols, m_tTemplateMat.rows);
			tTemplateRect &= tBgRect;
			Mat tTemplateMat = m_tTemplateMat(tTemplateRect);

			std::string tTemplateFile = m_tSavePath + "\\" + CString2String(tTemplateName) + "_temp.png";

			imwrite(tTemplateFile, tTemplateMat);

			//重新生成产品
			Mat tTmpProductMat;

			OpenCvUtil::rotate(m_tProductMat, tTmpProductMat, m_ptMakeTemplateView->getAngle(), 0x00);

			Rect tDstRect(0, 0, tTmpProductMat.cols, tTmpProductMat.rows);

			//保证模板与产品尺寸一致
			tProductRect.width  = tTemplateRect.width;
			tProductRect.height = tTemplateRect.height;

			tProductRect &= tDstRect;

			Mat tProductMat = tTmpProductMat(tProductRect);

			std::string tProductFile = m_tSavePath + "\\" + CString2String(tTemplateName) + "_product.tif";

			bool bRet = TiffUtil::writeMatToTiffFile(tProductFile, tProductMat);

			if (bRet) {
				MessageBox(L"生成成功!", L"生成模板", MB_OK | MB_ICONINFORMATION);

				if (m_bParentIsManager) {
					::SendMessage(GetParent()->m_hWnd, WM_RESET_TEMPLATE, (WPARAM)&tTemplateFile, (LPARAM)&tProductFile);
				}
				else {
					TemplateManager::T_Template tTemplate;

					tTemplate.tTemplateFile = tTemplateFile;
					tTemplate.tProductFile = tProductFile;
					tTemplate.tName = CString2String(tTemplateName);
					tTemplate.iMethod = 0;
					tTemplate.iLayer = 2;
					tTemplate.iSearchType = 0;
					tTemplate.iScore = 60;
					tTemplate.iCenterOffsetX = 0;
					tTemplate.iCenterOffsetY = 0;
					tTemplate.iMaxOverLap = 50;
					tTemplate.iContrastRatio = 100;
					tTemplate.iModelFeature = 0;
					tTemplate.iPrintSpeed = 0;
					tTemplate.iPrintHei = 0;
					tTemplate.iLightVoltage = 0;

					ptTemplateManager->addTemplate(&tTemplate);
					ptTemplateManager = nullptr;
				}

				EndDialog(IDOK);
			}
			else {
				MessageBox(L"生成失败!", L"生成模板", MB_OK | MB_ICONWARNING);
			}
		}
	}
	catch (const cv::Exception& e) {
		printf("opencv exception: %s\n", e.what());

		MessageBox(L"生成异常!", L"生成模板", MB_OK | MB_ICONERROR);
	}
	catch (const std::exception& e) {
		printf("std exception: %s\n", e.what());

		MessageBox(L"生成异常!", L"生成模板", MB_OK | MB_ICONERROR);
	}
	catch (...) {
		printf("exception: unknown\n");

		MessageBox(L"生成异常!", L"生成模板", MB_OK | MB_ICONERROR);
	}
}

//从ComboBox控件中获取当前缩放值
float MakeTemplateDialog::getCmbScaleStep(bool bFlag)
{
	float fScale = 0;

	switch (m_tCmbScaleStep.GetCurSel()) {
		case 0:		fScale = 0.001;	break;
		case 1:		fScale = 0.01;	break;
		case 2:		fScale = 0.02;	break;
		case 3:		fScale = 0.03;	break;
		case 4:		fScale = 0.04;	break;
		case 5:		fScale = 0.05;	break;
		default:					break;
	}

	if (bFlag)	fScale += 1.0f;
	else        fScale = 1.0f - fScale;

	return fScale;
}

//从ComboBox控件中获取当前旋转值
float MakeTemplateDialog::getCmbRotateStep(bool bFlag)
{
	float fAngle = 0;

	switch (m_tCmbRotateStep.GetCurSel()) {
		case 0:		fAngle = 0.01;	break;
		case 1:		fAngle = 0.1;	break;
		case 2:		fAngle = 1;		break;
		case 3:		fAngle = 5;		break;
		case 4:		fAngle = 10;	break;
		default:					break;
	}

	if (bFlag) {
		fAngle = -fAngle;
	}

	return fAngle;
}

//从ComboBox控件中获取当前移动值
float MakeTemplateDialog::getCmbMoveStep()
{
	float fStep = 0;

	switch (m_tCmbMoveStep.GetCurSel()) {
		case 0:		fStep = 0.1;	break;
		case 1:		fStep = 1;		break;
		case 2:		fStep = 5;		break;
		case 3:		fStep = 10;		break;
		default:					break;
	}

	return fStep;
}

//正向旋转
void MakeTemplateDialog::OnBnClickedBtnRotateInc()
{
	if (m_ptMakeTemplateView != nullptr) {
		m_ptMakeTemplateView->addAngle(getCmbRotateStep());
	}
}

//反向旋转
void MakeTemplateDialog::OnBnClickedBtnRotateDec()
{
	if (m_ptMakeTemplateView != nullptr) {
		m_ptMakeTemplateView->addAngle(getCmbRotateStep(true));
	}
}

//放大
void MakeTemplateDialog::OnBnClickedBtnScaleInc()
{
	if (m_ptMakeTemplateView != nullptr) {
		m_ptMakeTemplateView->addProductScale(getCmbScaleStep());
	}
}

//缩小
void MakeTemplateDialog::OnBnClickedBtnScaleDec()
{
	if (m_ptMakeTemplateView != nullptr) {
		m_ptMakeTemplateView->addProductScale(getCmbScaleStep(true));
	}
}

//复位
void MakeTemplateDialog::OnBnClickedBtnReset()
{
	m_tCmbScale.SetCurSel(2);

	if (m_ptMakeTemplateView != nullptr) {
		m_ptMakeTemplateView->resetAll();
	}
}

//左移按钮事件
void MakeTemplateDialog::OnBnClickedBtnMoveLeft()
{
	if (m_ptMakeTemplateView != nullptr) {
		m_ptMakeTemplateView->setOffsetX(-getCmbMoveStep());
	}
}

//右移按钮事件
void MakeTemplateDialog::OnBnClickedBtnMoveRight()
{
	if (m_ptMakeTemplateView != nullptr) {
		m_ptMakeTemplateView->setOffsetX(getCmbMoveStep());
	}
}

//上移按钮事件
void MakeTemplateDialog::OnBnClickedBtnMoveUp()
{
	if (m_ptMakeTemplateView != nullptr) {
		m_ptMakeTemplateView->setOffsetY(-getCmbMoveStep());
	}
}

//下移按钮事件
void MakeTemplateDialog::OnBnClickedBtnMoveDown()
{
	if (m_ptMakeTemplateView != nullptr) {
		m_ptMakeTemplateView->setOffsetY(getCmbMoveStep());
	}
}


void MakeTemplateDialog::OnCbnSelchangeCmbScale()
{
	if (m_ptMakeTemplateView != nullptr) {
		float fScale = 1.0f;

		switch (m_tCmbScale.GetCurSel()) {
			case 0:		fScale = 1.0f;		break;
			case 1:		fScale = 0.5f;		break;
			case 2:		fScale = 0.25f;		break;
			case 3:		fScale = 0.125f;	break;
			case 4:		fScale = 0.0625f;	break;
			case 5:		fScale = 0.9f;		break;
			case 6:		fScale = 0.8f;		break;
			case 7:		fScale = 0.7f;		break;
			case 8:		fScale = 0.6f;		break;
			case 9:		fScale = 0.5f;		break;
			case 10:	fScale = 0.4f;		break;
			case 11:	fScale = 0.3f;		break;
			case 12:	fScale = 0.2f;		break;
			case 13:	fScale = 0.1f;		break;
			case 14:	fScale = 0.08f;		break;
			case 15:	fScale = 0.05f;		break;
			case 16:	fScale = 0.03f;		break;
			case 17:	fScale = 0.01f;		break;
			default:						break;
		}

		m_ptMakeTemplateView->setScale(fScale);
	}
}

//模板对比度修改
void MakeTemplateDialog::OnNMCustomdrawSliderContrastRatio(NMHDR* ptNMHDR, LRESULT* ptResult)
{
	LPNMCUSTOMDRAW ptNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(ptNMHDR);

	if (ptResult != nullptr) {
		*ptResult = 0;
	}

	if (m_ptMakeTemplateView != nullptr) {
		m_ptMakeTemplateView->setContrastRatio(m_tSliderContrastRatio.GetPos() / 100.0f);
	}
}

// 自动定位
void MakeTemplateDialog::OnBnClickedBtnAutoPosition()
{
	if (m_tTemplateMat.empty()) {
		MessageBox(L"模板为空，无法定位!", L"生成模板", MB_OK | MB_ICONWARNING);

		return;
	}

	if (m_tProductMat.empty()) {
		MessageBox(L"产品为空，无法定位!", L"生成模板", MB_OK | MB_ICONWARNING);

		return;
	}

#ifndef USE_HALCON
	pq_matchSetParam("match_method", "1", false);

	int iLayer = 5;
	pq_matchSetParam("match_layer", std::to_string(5).c_str(), false);
	
	std::vector<cv::Mat> tVecProductChannels;
	cv::split(m_tProductMat, tVecProductChannels);

	/*cv::Mat tProductMat1 = tVecProductChannels[0];
	cv::imwrite("./product1.png", tProductMat1);
	cv::Mat tProductMat2 = tVecProductChannels[1];
	cv::imwrite("./product2.png", tProductMat2);
	cv::Mat tProductMat3 = tVecProductChannels[2];
	cv::imwrite("./product3.png", tProductMat3);
	cv::Mat tProductMat4 = tVecProductChannels[3];
	cv::imwrite("./product4.png", tProductMat4);*/
	
	cv::Mat tProductMat = tVecProductChannels[2];
	cv::threshold(tProductMat, tProductMat, 8, 255, cv::THRESH_BINARY_INV);
	//cv::imwrite("./product.png", tProductMat);

	int iPyrWidth = tProductMat.cols / pow(2, iLayer - 1);
	int iPyrHeight = tProductMat.rows / pow(2, iLayer - 1);
	int iMinLenth = std::min(iPyrWidth, iPyrHeight);
	int iModelFeature = std::max(iMinLenth / 3, 50);
	pq_matchSetParam("match_features", to_string(iModelFeature).c_str(), true);
	
	string tModeName = CString2String(FileUtil::getAppPath(TEMPLATE_MODEL)).c_str() + string("\\") + string("temp");
	Mat tMaskMat = Mat();

	pq_createTemplate(tProductMat, tMaskMat, tModeName.c_str());

	pq_matchSetParam("match_score", "60", false);
	pq_matchSetParam("match_max_over_lap", "50", false);
	pq_matchSetParam("match_model", tModeName.c_str(), true);

	cv::Mat tTempMat;
	int iTempMeanValue = cv::mean(m_tTemplateMat).val[0];
	cv::threshold(m_tTemplateMat, tTempMat, iTempMeanValue, 255, cv::THRESH_BINARY_INV);
	//cv::imwrite("./bg.png", tTempMat);

	vector<T_MatchInfo> tVecResult;

	if (pq_match_V2(tVecResult, tTempMat, tProductMat, tMaskMat)) {
		if (tVecResult.size() > 1) {
			sort(tVecResult.begin(), tVecResult.end(), 
				[](const T_MatchInfo& i1, const T_MatchInfo& i2) { return i1.fScore > i2.fScore; });
		}

		m_ptMakeTemplateView->autoProductPosition(tVecResult[0].fAngle, tVecResult[0].iCenterX, tVecResult[0].iCenterY);
	}
	else {
		MessageBox(L"自动定位失败!", L"自动定位", MB_OK);
	}
#else
	MessageBox(L"自动定位失败!", L"自动定位", MB_OK);
#endif

}
