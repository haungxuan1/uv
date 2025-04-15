#if _MSC_VER >= 1600						//VS2015>VS>VS2010, MSVC VER= 10.0 -14.0
#pragma execution_character_set("utf-8")
#endif
#include "pch.h"
#include "FeatureDlg.h"
#include "PQMsg.h"
#include "Helper.h"
#include <direct.h>

#include <iostream>
#include <fstream>
#include "cJSON.h"
#include "Transform.h"
#include "DrawPng.h"
#include "FastMatch.h"
#include "opencv2/highgui/highgui_c.h"
#include "MatchTestViewDlg.h"

IMPLEMENT_DYNAMIC(CFeatureDlg, CDialog)

BEGIN_MESSAGE_MAP(CFeatureDlg, CDialog)
	ON_BN_CLICKED(IDC_BT_FEATURE_GRAB, OnClickedGrab)
	ON_BN_CLICKED(IDC_BT_FEATURE_FREEZE, OnClickedFreeze)
	ON_BN_CLICKED(IDC_BT_FEATURE_SCAN, OnClickedScan)
	ON_BN_CLICKED(IDC_BT_FEATURE_SEND, OnClickedSend)

	ON_MESSAGE(PQM_CAMERA_CAPTURE, OnCaptureMsg)
	ON_MESSAGE(PQM_FEATURE_CONVERT, OnConvertMsg)
	ON_WM_HSCROLL()
	ON_WM_KEYDOWN()

	ON_MESSAGE(PQM_CAMERA_GRAB, OnGrabMsg)
	ON_MESSAGE(PQM_CAMERA_FREEZE, OnFreezeMsg)
	ON_BN_CLICKED(IDC_BT_FEATURE_TIFF, &CFeatureDlg::OnBnClickedBtFeatureTiff)
	ON_BN_CLICKED(IDC_BT_FEATURE_TEMP, &CFeatureDlg::OnBnClickedBtFeatureTemp)
//	ON_BN_CLICKED(IDC_BT_FEATURE_PARAM, &CFeatureDlg::OnBnClickedBtFeatureParam)
	ON_BN_CLICKED(IDC_BT_FEATURE_LOADTEMP, &CFeatureDlg::OnBnClickedBtFeatureLoadtemp)
	ON_BN_CLICKED(IDC_BT_FEATURE_SAVETEMP, &CFeatureDlg::OnBnClickedBtFeatureSavetemp)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PRE, &CFeatureDlg::OnNMCustomdrawSliderPre)
	ON_BN_CLICKED(IDC_BT_FEATURE_LOADMASK, &CFeatureDlg::OnBnClickedBtFeatureLoadmask)
	ON_BN_CLICKED(IDC_BT_FEATURE_LOADSAMP, &CFeatureDlg::OnBnClickedBtFeatureLoadsamp)
	ON_BN_CLICKED(IDC_BT_FEATURE_MATCHTEST, &CFeatureDlg::OnBnClickedBtFeatureMatchtest)
END_MESSAGE_MAP()

int writeFileString(char* str, std::string filename) {
	std::ofstream outfile(filename);
	if (outfile.is_open()) {
		outfile << str << std::endl;
		outfile.close();
		return 1;
	}
	else {
		return 0;
	}
}

std::string readFileString(std::string filename) {
	std::ifstream infile(filename,ios::in);
	std::string line;
	if (infile.is_open()) {
		while (getline(infile, line)) {
			std::cout << "sss" << line << std::endl;
			break;
		}
		infile.close();
	}
	else {
		std::cerr << "Unable to open file for reading." << std::endl;
	}
	return line;
}

CFeatureDlg::CFeatureDlg(CWnd* pParent, CString cameraPath)
	:CDialog(IDD, pParent), CFeatureTh(this),
	m_nSent(0), m_nFrameIdx(0), m_nExtTriggerFrameCount(0), m_nChart{ 20, 35 }, m_lpChart(nullptr), m_rParam(cameraPath)
{

}

CFeatureDlg::~CFeatureDlg()
{
}

void CFeatureDlg::UpdatePreview()
{
	UpdateData(FALSE);
	if (!m_rSave.IsEmpty())
	{
		Mat cTmp = imread(CString2String(m_rSave), 0);

		CRect rect;

		GetDlgItem(IDC_PIC_IMAGE)->GetClientRect(&rect);

		resize(cTmp, readImage, Size(rect.Width(), rect.Height()));

		imshow("ImageShow", readImage);
	}
}

void CFeatureDlg::UpdatePreValue()
{
	UpdateData(TRUE);

	m_rPreValue.Format(L"%d", m_csPre.GetPos());

	UpdateData(FALSE);

	double s = std::stod(CString2String(m_rPreValue));

	//
	if (!readImage.empty()) {
		Mat tmpImage;

		double dCRotio = (double)(s / 100);

		cv::convertScaleAbs(readImage, tmpImage, dCRotio);

		imshow("ImageShow", tmpImage);
	}
	//发送至
	::PostMessage(m_cImControl, PQM_IM_VIEW_CONTRAST, (WPARAM)s, 0);
}

void CFeatureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ET_FEATURE_FRAME, m_nExtTriggerFrameCount);

	DDX_Text(pDX, IDC_ET_FEATURE_COLS, m_nChart[0]);

	DDX_Text(pDX, IDC_ET_FEATURE_ROWS, m_nChart[1]);

	DDX_Text(pDX, IDC_ET_FEATURE_TIFF, m_rTIFF);

	DDX_Text(pDX, IDC_ET_FEATURE_TEMP, m_rTemp);

	DDX_Text(pDX, IDC_ET_FEATURE_PARAM, m_rParam);

	DDX_Text(pDX, IDC_ET_FEATURE_SAVE, m_rSave);

	DDX_Text(pDX, IDC_ET_FEATURE_MASK, m_rMask);

	DDX_Text(pDX, IDC_ET_FEATURE_SAMP, m_rSamp);

	DDX_Text(pDX, IDC_ET_FEATURE_SCORE, m_rScore);

	DDX_Text(pDX, IDC_CB_FEATURE_SPEED, m_rSpeed);

	//	DDX_Text(pDX, IDC_ET_FEATURE_LINE, m_rLine);

	DDX_Text(pDX, IDC_ET_FEATURE_DISTANCE, m_rDistance);

	DDX_Text(pDX, IDC_ET_FEATURE_CARSPEED, m_rCarSpeed);

	DDX_Text(pDX, IDC_ET_FEATURE_VOLTAGE, m_rVoltage);

	DDX_Text(pDX, IDC_STATIC_PREVALUE, m_rPreValue);

	DDX_Text(pDX, IDC_ET_FEATURE_OFFSET_Y, m_rOffsetY);

	DDX_Text(pDX, IDC_ET_FEATURE_OFFSET_X, m_rOffsetX);


	DDX_Control(pDX, IDC_ET_FEATURE_FRAME, m_etFrame);

	DDX_Control(pDX, IDC_BT_FEATURE_GRAB, m_btGrab);

	DDX_Control(pDX, IDC_BT_FEATURE_FREEZE, m_btFreeze);

	DDX_Control(pDX, IDC_BT_FEATURE_SCAN, m_btScan);

	DDX_Control(pDX, IDC_BT_FEATURE_SEND, m_btSend);

	DDX_Control(pDX, IDC_ET_FEATURE_COLS, m_etChart[0]);

	DDX_Control(pDX, IDC_ET_FEATURE_ROWS, m_etChart[1]);

	DDX_Control(pDX, IDC_ET_FEATURE_TIFF, m_etPrinter[0]);

	DDX_Control(pDX, IDC_ET_FEATURE_TEMP, m_etPrinter[1]);

	DDX_Control(pDX, IDC_ET_FEATURE_PARAM, m_etPrinter[2]);

	DDX_Control(pDX, IDC_CB_FEATURE_SPEED, m_cbSpeed);

	DDX_Control(pDX, IDC_SLIDER_PRE, m_csPre);

}

BOOL CFeatureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rt;

	::GetWindowRect(GetDlgItem(IDC_ST_FEATURE_IM_CONTROL)->GetSafeHwnd(), rt);

	ScreenToClient(rt);

	// 作为图片控件的临时皮肤，暂时以绝对路径
	m_cImControl.SetIcon(L"D:\\ImControl");

	m_cImControl.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(rt.left, rt.top, rt.right, rt.bottom), this, IDC_ST_FEATURE_IM_CONTROL);

	CCalibratorDlg::LoadParam(CString2String(m_rParam), m_tCameraParam);

 	CFeatureTh::Reg();
 
 	CFeatureTh::InitParam(m_tCameraParam.vFill, m_tCameraParam.M, m_tCameraParam.vWH2, m_tCameraParam.vDPI2);


	m_csPre.SetRange(0, 500);

	m_csPre.SetTicFreq(1);

	m_csPre.SetTic(100);

	m_csPre.SetPos(100);

	m_rPreValue = L"100";

	m_cbSpeed.InsertString(0, L"快");

	m_cbSpeed.InsertString(1, L"中");

	m_cbSpeed.InsertString(2, L"慢");
	
	m_rScore.Format(_T("%d"), 60);
	
	m_rOffsetX.Format(_T("%d"), 0);
	
	m_rOffsetY.Format(_T("%d"), 0);
	
//	m_rLine.Format(_T("%d"), 100);

	if (m_mModel.size() > 0) {
		jsonItem mj = m_mModel[curID];

		m_rTIFF = mj.picPath.c_str();

		m_rTIFFName = mj.picName.c_str();

		m_rSave = mj.path.c_str();

		m_rSaveName = mj.tempName.c_str();

		m_rScore = mj.socor.c_str();

		m_rOffsetX = mj.offsetX.c_str();

		m_rOffsetY = mj.offsetY.c_str();

//		m_rLine = mj.line.c_str();

		m_csPre.SetPos(std::stoi(mj.pre.c_str()));

		m_rPreValue = mj.pre.c_str();

		m_rDistance = mj.distance.c_str();

		m_rCarSpeed = mj.carspeed.c_str();

		m_rVoltage = mj.voltage.c_str();

		m_rMask = mj.mask.c_str();

		int tempS = std::stoi(mj.speed);

		switch (tempS)
		{
		case 75:
			tempS = 0;
			break;
		case 500:
			tempS = 1;
			break;
		case 1000:
			tempS = 2;
			break;
		default:
			tempS = 0;
			break;
		}
		m_cbSpeed.SetCurSel(tempS);

	}

	m_cbSpeed.SetCurSel(0);


	m_btSend.EnableWindow(FALSE);

	m_btScan.EnableWindow(FALSE);

	UpdateData(FALSE);

	//opencv 窗口初始化
	namedWindow("ImageShow");//创建OpenCV窗口

	HWND hWnd = (HWND)cvGetWindowHandle("ImageShow");//嵌套opencv窗口

	HWND hParent = ::GetParent(hWnd);

	::SetParent(hWnd, GetDlgItem(IDC_PIC_IMAGE)->m_hWnd);

	::ShowWindow(hParent, SW_HIDE);

	UpdatePreview();

	return TRUE;
}

void CFeatureDlg::OnClickedGrab()
{
	UpdateData(TRUE);

	PQCamera().Grab(m_nExtTriggerFrameCount, GetSafeHwnd());
}

void CFeatureDlg::OnClickedFreeze()
{
	PQCamera().Freeze(GetSafeHwnd());
}

void CFeatureDlg::OnClickedScan()
{
	m_btScan.EnableWindow(FALSE);

	UpdateData(TRUE);

	m_nSent = 0;

	PQPrinter().SetPrinterType(CPrinterBase::ePT_Empty);

	PQPrinter().SetInitiator(this);

	///////////////////////////////////////////////////
//	PQPrinter().SetPrinterType(CPrinterBase::ePT_FastPrint);
	///////////////////////////////////////////////////

	PQPrinter().Reg();

	if (!!m_lpChart) delete m_lpChart;

	Chart(m_nChart[1], m_nChart[0], m_lpChart);

	Gray2Cmyk(*m_lpChart, *m_lpChart, {});

	tuple<unsigned int, unsigned char*> vPrtHeader;

	PrtHeader(m_lpChart->cols, m_lpChart->rows, m_lpChart->channels(), 2, vPrtHeader);

	ImPtr::CImPtr cHeader(new Mat(1, get<0>(vPrtHeader), CV_8UC1));

	memcpy(cHeader.Get()->data, get<1>(vPrtHeader), get<0>(vPrtHeader));

	PQPrinter().TCP({ -1, false, {{__FUNCTION__, clock()} }, new array<uint,3>{false, 0,get<0>(vPrtHeader) },cHeader });

	delete get<1>(vPrtHeader); 
}

void CFeatureDlg::OnClickedSend()
{
	UpdateData(TRUE);

	m_btSend.EnableWindow(FALSE);

	m_cImControl.ClearLayer();

//	m_cImControl.CreateLayer(false, false, true, CString2String(m_rTIFF), true, 1.0, L"TIFF", m_cTIFF, true, 0.0, 0, 0);//,nullptr,std::stod(CString2String(m_rPreValue))

	++m_nSent;

	bool bFinish = (m_nSent == (m_nChart[0] - 1));

	ImPtr::CImPtr cSub(new Mat(m_lpChart->rows / m_nChart[0], m_lpChart->cols, CV_8UC4));

	(*m_lpChart)(Rect(0, m_nSent * cSub.Get()->rows, cSub.Get()->cols, cSub.Get()->rows)).copyTo(*cSub.Get());

	PQPrinter().Mat2Prt({ 0, false, { {__FUNCTION__, clock()} },new bool(bFinish),  cSub });

	m_btSend.EnableWindow(bFinish ? FALSE : TRUE);

	m_btScan.EnableWindow(bFinish ? TRUE : FALSE);
}

LRESULT CFeatureDlg::OnGrabMsg(WPARAM wparam, LPARAM lparam)
{
	m_nFrameIdx = 0;

	m_etFrame.EnableWindow(FALSE);

	m_btGrab.EnableWindow(FALSE);

	m_btFreeze.EnableWindow(TRUE);

	return 0;
}

LRESULT CFeatureDlg::OnFreezeMsg(WPARAM wparam, LPARAM lparam)
{
	m_etFrame.EnableWindow(TRUE);

	m_btGrab.EnableWindow(TRUE);

	m_btFreeze.EnableWindow(FALSE);

//	PQPrinter().StopSend();
	return 0;
}

BOOL CFeatureDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		// 检查按键是否是回车
		if (pMsg->wParam == VK_RETURN)
		{
			// 如果是回车键，不进行任何操作，返回TRUE阻止消息继续传递
			return TRUE;
		}
	}

	// 对于其他消息，让消息继续传递
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CFeatureDlg::OnCaptureMsg(WPARAM wparam, LPARAM lparam)
{
	ImPtr::CImPtr* lpFrame = (ImPtr::CImPtr*)lparam;

	OnCameraFrame(lpFrame);

	return 0;
}

LRESULT CFeatureDlg::OnConvertMsg(WPARAM wparam, LPARAM lparam)
{
	ImPtr::CImPtr* lpFrame = (ImPtr::CImPtr*)lparam;

	++m_nFrameIdx;

	m_cImControl.CreateLayer(true, true, false, "", true, 1.0, to_wstring(m_nFrameIdx).c_str(), *lpFrame, true, 0.0, 0, 0);

	delete lpFrame;

	return 0;
}

void CFeatureDlg::InitParam(std::map<int, jsonItem> json, int id)
{
	m_mModel.clear();
	m_mModel = json;
	curID = id;
}

void CFeatureDlg::UpdateParam()
{
	if(!m_rTIFF.IsEmpty())
	{
//		m_cTIFF = ImPtr::CImPtr(new Mat());

//		ReadTif(CString2String(m_rTIFF), *m_cTIFF.Get());

	}
	if (!m_rTemp.IsEmpty())
	{
		m_cImControl.SetLayerDir(CString2String(m_rTemp));

	}


	if (!m_rTemp.IsEmpty() && !m_rParam.IsEmpty()) {
		m_btSend.EnableWindow(TRUE);

		m_btScan.EnableWindow(TRUE);
	}
/*
	if(!m_rParam.IsEmpty())
	{
		CCalibratorDlg::LoadParam(CString2String(m_rParam), m_tCameraParam);

		CFeatureTh::Reg();

		CFeatureTh::InitParam(m_tCameraParam.vFill, m_tCameraParam.M, m_tCameraParam.vWH2, m_tCameraParam.vDPI2);
	}
*/
}

void CFeatureDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (GetDlgItem(IDC_SLIDER_PRE) == pScrollBar) {
		if (nPos == 0) {
			return;
		}
		UpdatePreValue();
	}
}

void CFeatureDlg::OnBnClickedBtFeatureTiff()
{
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, NULL, L"F:\\f\\VisionLocalization\\x64\\Release\\temp\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Print (*.tif)|*.tif||");

	if (dlg.DoModal() != IDOK) return;
	CString s = dlg.GetPathName();

	m_rTIFF = dlg.GetPathName();

	m_rTIFFName = dlg.GetFileName();

	UpdateData(FALSE);

	UpdateParam();
}


void CFeatureDlg::OnBnClickedBtFeatureTemp()
{
	UpdateData(TRUE);

	BROWSEINFO bi = { 0 };

	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	bi.hwndOwner = this->GetSafeHwnd();

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (!!!pidl) return;

	TCHAR szPath[MAX_PATH] = { 0 };

	SHGetPathFromIDList(pidl, szPath);

	CoTaskMemFree(pidl);

	m_rTemp = szPath;

	UpdateData(FALSE);

	UpdateParam();
}

void CFeatureDlg::OnBnClickedBtFeatureLoadtemp()
{
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, NULL, L"F:\\f\\VisionLocalization\\x64\\Release\\param\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"camera param (*.png)|*.png||");

	if (dlg.DoModal() != IDOK) return;

	m_rSave = dlg.GetPathName();

	m_rSaveName = dlg.GetFileName();

	UpdateData(FALSE);

	UpdatePreview();
}


void CFeatureDlg::OnBnClickedBtFeatureSavetemp()
{
	UpdateData(TRUE);
	//判断是否存在未设置选项，存在则返回
	if (m_rTIFF.IsEmpty() || m_rSave.IsEmpty() || m_rDistance.IsEmpty() || m_rCarSpeed.IsEmpty() || m_rVoltage.IsEmpty()) {// || m_rMask.IsEmpty()
		AfxMessageBox(L"存在未填项");

		return;
	}
		//获取ini中的保存字段
		//创建需要保存的json对象
		cJSON* itemO = cJSON_CreateObject();

		cJSON_AddStringToObject(itemO, "path", CString2String(m_rSave).c_str());

		cJSON_AddStringToObject(itemO, "temp_name", CString2String(m_rSaveName).c_str());

		cJSON_AddStringToObject(itemO, "match_score", CString2String(m_rScore).c_str());

		cJSON_AddStringToObject(itemO, "match_contrast_ratio", CString2String(m_rPreValue).c_str());

		cJSON_AddStringToObject(itemO, "match_center_x_offset", CString2String(m_rOffsetX).c_str());

		cJSON_AddStringToObject(itemO, "match_center_y_offset", CString2String(m_rOffsetY).c_str());

		cJSON_AddStringToObject(itemO, "pic_path", CString2String(m_rTIFF).c_str());

		cJSON_AddStringToObject(itemO, "pic_name", CString2String(m_rTIFFName).c_str());

		cJSON_AddStringToObject(itemO, "carspeed", CString2String(m_rCarSpeed).c_str());

		cJSON_AddStringToObject(itemO, "distance", CString2String(m_rDistance).c_str());

		cJSON_AddStringToObject(itemO, "voltage", CString2String(m_rVoltage).c_str());

		cJSON_AddStringToObject(itemO, "mask", CString2String(m_rMask).c_str());

		int tempI = m_cbSpeed.GetCurSel();
		switch (tempI)
		{
		case 0:
			tempI = 75;
			break;
		case 1:
			tempI = 500;
			break;
		case 2:
			tempI = 1000;
			break;
		default:
			tempI = 500;
			break;
		}
		cJSON_AddStringToObject(itemO, "match_max_pos", to_string(tempI).c_str());

		//若存在json对象
		std::string line = readFileString("config.txt");

		cJSON* json = cJSON_Parse(line.c_str());
		if (json != NULL) {
			cJSON* arr = cJSON_GetObjectItem(json, "arr");
			if (arr == NULL) {
				arr = cJSON_CreateArray();
				cJSON_AddItemToArray(arr, itemO);
				cJSON_AddItemToObject(json, "arr", arr);
			}
			else
			{
				if (m_mModel.size() > 0){
					cJSON* item = cJSON_GetArrayItem(arr, curID);
					cJSON_ReplaceItemInObject(item, "path", cJSON_CreateString(CString2String(m_rSave).c_str()));
					cJSON_ReplaceItemInObject(item, "temp_name", cJSON_CreateString(CString2String(m_rSaveName).c_str()));
					cJSON_ReplaceItemInObject(item, "match_score", cJSON_CreateString(CString2String(m_rScore).c_str()));
					cJSON_ReplaceItemInObject(item, "match_contrast_ratio", cJSON_CreateString(CString2String(m_rPreValue).c_str()));
					cJSON_ReplaceItemInObject(item, "match_center_x_offset", cJSON_CreateString(CString2String(m_rOffsetX).c_str()));
					cJSON_ReplaceItemInObject(item, "match_center_y_offset", cJSON_CreateString(CString2String(m_rOffsetY).c_str()));
					cJSON_ReplaceItemInObject(item, "pic_path", cJSON_CreateString(CString2String(m_rTIFF).c_str()));
					cJSON_ReplaceItemInObject(item, "pic_name", cJSON_CreateString(CString2String(m_rTIFFName).c_str()));
					cJSON_ReplaceItemInObject(item, "carspeed", cJSON_CreateString(CString2String(m_rCarSpeed).c_str()));
					cJSON_ReplaceItemInObject(item, "distance", cJSON_CreateString(CString2String(m_rDistance).c_str()));
					cJSON_ReplaceItemInObject(item, "voltage", cJSON_CreateString(CString2String(m_rVoltage).c_str()));
					cJSON_ReplaceItemInObject(item, "mask", cJSON_CreateString(CString2String(m_rMask).c_str()));
					cJSON_ReplaceItemInObject(item, "match_max_pos", cJSON_CreateString(to_string(tempI).c_str()));


	//				cJSON_DeleteItemFromArray(arr, curID);
				}
				else
					cJSON_AddItemToArray(arr, itemO);

			}
		}
		else {
			json = cJSON_CreateObject();
			cJSON* arr = cJSON_CreateArray();
			cJSON_AddItemToArray(arr, itemO);
			cJSON_AddItemToObject(json, "arr", arr);
		}
		char* tempStr = cJSON_PrintUnformatted(json);
		writeFileString(tempStr, "config.txt");

		printf("obj: %s/n", tempStr);
		AfxMessageBox(L"保存成功");

}

void CFeatureDlg::OnNMCustomdrawSliderPre(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	UpdatePreValue();
	*pResult = 0;
}


void CFeatureDlg::OnBnClickedBtFeatureLoadmask()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, NULL, L"F:\\f\\VisionLocalization\\x64\\Release\\param\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"camera param (*.png)|*.png||");

	if (dlg.DoModal() != IDOK) return;

	m_rMask = dlg.GetPathName();

	UpdateData(FALSE);

//	UpdatePreview();
}


void CFeatureDlg::OnBnClickedBtFeatureLoadsamp()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, NULL, L"F:\\f\\VisionLocalization\\x64\\Release\\param\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"camera param (*.png)|*.png||");

	if (dlg.DoModal() != IDOK) return;

	m_rSamp = dlg.GetPathName();

	UpdateData(FALSE);
}


void CFeatureDlg::OnBnClickedBtFeatureMatchtest()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_rTIFF.IsEmpty() && !m_rSave.IsEmpty() && !m_rSamp.IsEmpty()) 
	{
		//设置匹配参数相关
		int tempI = m_cbSpeed.GetCurSel();
		switch (tempI)
		{
		case 0:
			tempI = 75;
			break;
		case 1:
			tempI = 500;
			break;
		case 2:
			tempI = 1000;
			break;
		default:
			tempI = 500;
			break;
		}

		MatchSetParam("match_score", CString2String(m_rScore).c_str(), true);

		MatchSetParam("match_max_pos", to_string(tempI).c_str(), true);

		MatchSetParam("match_center_x_offset", CString2String(m_rOffsetX).c_str(), true);

		MatchSetParam("match_center_y_offset", CString2String(m_rOffsetY).c_str(), true);

		MatchSetParam("match_contrast_ratio", CString2String(m_rPreValue).c_str(), true);

		Mat cTmp = imread(CString2String(m_rSave), 0);

		Mat cMask = imread(CString2String(m_rMask), 0);

		Mat cSamp = imread(CString2String(m_rSamp), 0);

		TIFF* cMold = TIFFOpen(CString2String(m_rTIFF).c_str(), "rb");

		if (!cTmp.empty() && !cSamp.empty())
		{
			Mat Result;

			MatchCheck(Result, cSamp, cTmp, cMask, cMold);

			if (!Result.empty()) {
				MatchTestViewDlg dlg(this, &Result);

				dlg.DoModal();
//				imshow("MatchTestImage", Result);
			}
			else AfxMessageBox(L"生成失败");
		}
/*
		MatchTestViewDlg dlg(this, m_rTIFF, m_rSave, m_rMask, m_rSamp);

		dlg.DoModal();
*/

	}
}
