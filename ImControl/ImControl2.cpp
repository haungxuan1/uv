#include "pch.h"
#include "ImControl2.h"
#include "PQMsg.h"
#include "Helper.h"
#include "Transform.h"

using namespace Transform;

namespace ImControl2
{
	IMPLEMENT_DYNAMIC(CLayouter, ImCtrl::CImCtrl)
	BEGIN_MESSAGE_MAP(CLayouter, ImCtrl::CImCtrl)

	END_MESSAGE_MAP()

	CLayouter::CLayouter()
	{

	}

	CLayouter::~CLayouter()
	{

	}

	void CLayouter::SetIcon(wstring strIconDir)
	{
		m_cImView.SetIcon(strIconDir + L"\\View");

		m_cImParam.SetIcon(strIconDir + L"\\Param");

		m_cImLayers.SetIcon(strIconDir + L"\\Layers");
	}

	void CLayouter::SetPic(wstring strPicPath)
	{
		m_cImParam.SetTempPicPath(strPicPath);
	}

	/*void CLayouter::SaveBackground()
	{
		PostMessage(PQM_IM_CONTROL_SAVE_BACKGROUND, (WPARAM)0, (LPARAM)0);
	}*/

	void CLayouter::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		static int nImParamHight = 330;

		static int nImParamWidth = 499;

		auto& cx = lpCreateStruct->cx, & cy = lpCreateStruct->cy;

		SetCtrlParam(&m_cImView, { 0, 0 }, { cx - nImParamWidth - 1, cy });

		SetCtrlParam(&m_cImParam, { m_cImView.GetResolution().x + 1, 0 }, { nImParamWidth, nImParamHight });

		SetCtrlParam(&m_cImLayers, { 0, m_cImParam.GetResolution().y + 1 }, { nImParamWidth, cy - nImParamHight - 1 });

		CreateCtrl(&m_cImView, nullptr, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 1);

		CreateCtrl(&m_cImParam, &m_cImView, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 2);

		CreateCtrl(&m_cImLayers, &m_cImParam, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 3);
	}
	/**************************************************************************************************/
	IMPLEMENT_DYNAMIC(CImLayerMsg, CLayouter)
	BEGIN_MESSAGE_MAP(CImLayerMsg, CLayouter)
		ON_MESSAGE(PQM_IM_CENTER_CREATE, OnCreateLayerMsg)
		ON_MESSAGE(PQM_IM_CENTER_DESTROY, OnDestroyLayerMsg)
		ON_MESSAGE(PQM_IM_LAYER_CLEAR, OnClearLayerMsg)

		ON_MESSAGE(PQM_IM_LAYER_CLICKED_ENABLE, OnClickedEnableMsg)
		ON_MESSAGE(PQM_IM_LAYER_CLICKED_BACKGROUND, OnClickedBackGroundMsg)
		ON_MESSAGE(PQM_IM_LAYER_NAME_CHANGED, OnNameChangedMsg)
		ON_MESSAGE(PQM_IM_LAYER_FOCUS_CHANGED, OnFocusChangedMsg)

		ON_MESSAGE(PQM_IM_CONTROL_COPY_LAYER, OnCopyLayerMsg)
		ON_MESSAGE(PQM_IM_CONTROL_SAVE_LAYER, OnSaveLayerMsg)
		//ON_MESSAGE(PQM_IM_CONTROL_SAVE_BACKGROUND, OnSaveBackgroundMsg)
	END_MESSAGE_MAP()

	CImLayerMsg::CImLayerMsg()
	{

	}

	CImLayerMsg::~CImLayerMsg()
	{

	}

	void CImLayerMsg::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		CLayouter::OnCreateEx(lpCreateStruct);

		m_cImLayers.Bind(this);

		m_cImCenter.PreviewSize(46, 46);
	}

	void CImLayerMsg::ClearLayer()
	{
		m_cImLayers.OnClearLayer();
	}

	void CImLayerMsg::SelectAll()
	{
		m_cImLayers.OnSelectAll();
	}

	void CImLayerMsg::CreateLayer(ImLayer::tagLayer tParam)
	{
		m_cImCenter.CreateLayer(tParam);
	}

	void CImLayerMsg::CreateLayer(
		bool bBackground, 
		bool bFocus,
		bool bCMYK, string strTIFF,
		bool bEnable, double dOpacity, CString rName, ImPtr::CImPtr cImage,
		bool bAuto, double dScal, int x, int y, void* lpUserParam, double contrast)
	{
		contrast = contrast;

		ImLayer::tagLayer tParam;

		if (bAuto) Auto(cImage, dScal, x, y);

		tParam.bTIFF = bCMYK; tParam.strTIFF = strTIFF;

		tParam.bBackground = bBackground, tParam.bFocus = bFocus;

		tParam.bEnable = bEnable, tParam.dOpacity = dOpacity, tParam.rName = rName; tParam.cImage = cImage;

		tParam.dScaling[0] = tParam.dScaling[1] = dScal;

		tParam.x[0] = x, tParam.y[0] = y;

		tParam.lpUserParam = lpUserParam;

		tParam.dContrastRotio = contrast;

		CreateLayer(tParam);
	}

	void CImLayerMsg::Auto(ImPtr::CImPtr& cImage, double& dScal, int& x, int& y)
	{
		int* lpSize = new int[2];

		SendMessage(PQM_IM_CONTROL_SYNC_RENDER_SIZE, 0, (LPARAM)lpSize);

		dScal = AutoScal(cImage, { lpSize[0], lpSize[1] });

		tie(x, y) = AutoCenter(cImage, { lpSize[0], lpSize[1] }, dScal);
	}

	double CImLayerMsg::AutoScal(ImPtr::CImPtr& cImage, Size cRenderSize)
	{
		auto& w1 = cRenderSize.width, & h1 = cRenderSize.height;

		auto w2 = cImage.Get()->cols, h2 = cImage.Get()->rows;

		double dx = (double)w1 / w2, dy = (double)h1 / h2;

		return min(dx, dy);
	}

	tuple<int, int> CImLayerMsg::AutoCenter(ImPtr::CImPtr& cImage, Size cRenderSize, double dScal)
	{
		auto w1 = cImage.Get()->cols, h1 = cImage.Get()->rows;

		auto w2 = dScal * w1, h2 = dScal * h1;

		auto cx = 0.5 * w2, cy = 0.5 * h2;

		return make_tuple(ceil(cx), ceil(cy));
	}

	LRESULT CImLayerMsg::OnCreateLayerMsg(WPARAM wparam, LPARAM lparam)
	{
		long long nSign = (long long)lparam;

		m_cImLayers.OnCreateLayer(nSign);

		return 0;
	}

	LRESULT CImLayerMsg::OnDestroyLayerMsg(WPARAM wparam, LPARAM lparam)
	{
		long long nSign = (long long)lparam;

		return 0;
	}

	LRESULT CImLayerMsg::OnClearLayerMsg(WPARAM wparam, LPARAM lparam)
	{
		m_cImCenter.ClearLayer();

		return 0;
	}

	void CImLayerMsg::OnSync(ImLayer::tagLayer& tBackground, ImLayer::tagLayer& tFocus, ImLayer::tagLayer& tSign)
	{
		m_cImLayers.OnSyncParam(tBackground, tFocus, tSign); // 更新背景图层 // 更新焦点图层 // 更新当前标记对应图层
	}

	LRESULT CImLayerMsg::OnFocusChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		long long nSign = (long long)lparam;

		m_cImCenter.Focus(nSign);

		return 0;
	}

	LRESULT CImLayerMsg::OnClickedEnableMsg(WPARAM wparam, LPARAM lparam)
	{
		bool bEnable = (bool)wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Enable(nSign, bEnable);

		return 0;
	}

	LRESULT CImLayerMsg::OnClickedBackGroundMsg(WPARAM wparam, LPARAM lparam)
	{
		long long nSign = (long long)lparam;

		m_cImCenter.Background(nSign);

		return 0;
	}
	
	LRESULT CImLayerMsg::OnNameChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		ImLayer::tagLayer tParam;

		wchar_t* lpName = (wchar_t*)wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Name(nSign, lpName);

		delete lpName;

		return 0;
	}

	LRESULT CImLayerMsg::OnCopyLayerMsg(WPARAM wparam, LPARAM lparam)
	{
		Rect* lpWin = new Rect(); // 选择框在背景中位置

		SendMessage(PQM_IM_CONTROL_SYNC_SELECTED, 0, (LPARAM)lpWin);

		vector<long long> vSign;

		m_cImCenter.GetSign(vSign);

		for (auto& iSign : vSign)CopyLayer(iSign, *lpWin);

		delete lpWin;
	
		return 0;
	}

	void CImLayerMsg::CopyLayer(long long& nSign, Rect& rtWin)
	{
		ImLayer::tagLayer tParam;

		if (!m_cImCenter.Get(nSign, tParam)) return;

		if (!tParam.bEnable) return;

		CopyLayer(tParam, rtWin);
	}

	void CImLayerMsg::CopyLayer(ImLayer::tagLayer& tLayer, Rect& rtWin)
	{
		auto iUnite = Unite(tLayer, rtWin);// 图片与窗口相交部分则为新图层 // 完全不想交，则不进行拷贝

		if (iUnite.empty()) return;

		auto iLayer = Layer(tLayer, iUnite);

		auto os = Offset(tLayer, iLayer); // 中心偏移

		iLayer = RtS2R(iLayer, tLayer.dScaled[0], tLayer.dScaled[1]); // 缩放 -> 旋转 // 该步包含小数运算，因此产生的值可能大于原图层

		Size szRotated; szRotated.width = tLayer.cRotated.Get()->cols, szRotated.height = tLayer.cRotated.Get()->rows;

		int r = iLayer.x + iLayer.width, b = iLayer.y + iLayer.height;

		r = min(r, szRotated.width), b = min(b, szRotated.height);

		iLayer.width = r - iLayer.x, iLayer.height = b - iLayer.y; // 重组

		CopyLayer(tLayer, iLayer, os);
	}

	void CImLayerMsg::CopyLayer(ImLayer::tagLayer& tLayer, Rect& rtCopy, Point2f& ptOs)
	{
		ImLayer::tagLayer tCopy;

		tCopy.cImage = ImPtr::CImPtr(new Mat(rtCopy.height, rtCopy.width, CV_8UC(tLayer.cRotated.Get()->channels())));

		auto iImage1 = (*tLayer.cRotated.Get())(rtCopy); // 选中图层 // 保持旋转角度

		auto iImage2 = *tCopy.cImage.Get();

		iImage1.copyTo(iImage2);

		tCopy.dRotating[0] = tCopy.dRotating[1] = 0.0;

		tCopy.dScaling = tLayer.dScaling;

		tCopy.x[0] = tLayer.Center().x + ptOs.x;

		tCopy.y[0] = tLayer.Center().y + ptOs.y;

		tCopy.bEnable = true;

		tCopy.bTIFF = tLayer.bTIFF;

		tCopy.strTIFF = tLayer.strTIFF;

		tCopy.rName = tLayer.rName + L"_copy";

		tCopy.dOpacity = tLayer.dOpacity;

		CreateLayer(tCopy);
	}

	Rect CImLayerMsg::Unite(ImLayer::tagLayer& tLayer, Rect& rtWin)
	{
		Rect rtImage(tLayer.nLeft, tLayer.nTop, tLayer.nWidth, tLayer.nHeight);// 缩放后图片位于背景中位置

		return rtImage & rtWin;
	}

	Rect CImLayerMsg::Layer(ImLayer::tagLayer& tLayer, Rect& rtUnite)
	{
		Rect rtImage(tLayer.nLeft, tLayer.nTop, tLayer.nWidth, tLayer.nHeight), rtLayer;

		rtLayer.x = rtUnite.x - rtImage.x;

		rtLayer.y = rtUnite.y - rtImage.y;

		rtLayer.width = rtUnite.width; 
		
		rtLayer.height = rtUnite.height; // 新图层位于原图层位置 

		return rtLayer;
	}

	Point2f CImLayerMsg::Offset(ImLayer::tagLayer& tLayer, Rect& rtLayer)
	{
		Point2f c1, c2;

		c1.x = 0.5 * tLayer.nWidth;

		c1.y = 0.5 * tLayer.nHeight;

		c2.x = rtLayer.x + 0.5 * rtLayer.width;

		c2.y = rtLayer.y + 0.5 * rtLayer.height;

		return { c2.x - c1.x, c2.y - c1.y }; // 中心点偏移
	}

	Point2d CImLayerMsg::PtS2R(Point2d pt, double dSx, double dSy)
	{
		Mat M(Mat::zeros(2, 3, CV_64F));

		M.at<double>(0, 0) = 1.0 / dSx;

		M.at<double>(1, 1) = 1.0 / dSy;

		Mat cSc(Mat::ones(3, 1, CV_64F));

		cSc.at<double>(0, 0) = pt.x;

		cSc.at<double>(1, 0) = pt.y;

		Mat cRo = M * cSc; // 2*1 矩阵

		return Point2d(cRo.at<double>(0, 0), cRo.at<double>(1, 0));
	}

	Rect CImLayerMsg::RtS2R(Rect rt, double dSx, double dSy)
	{
		auto l = rt.x, t = rt.y, r = rt.x + rt.width, b = rt.y + rt.height;

		Point2d ptTL = PtS2R(Point2d(l, t), dSx, dSy);

		Point2d ptBR = PtS2R(Point2d(r, b), dSx, dSy);

		return  Rect(ptTL.x, ptTL.y, ptBR.x - ptTL.x, ptBR.y - ptTL.y);
	}

	LRESULT CImLayerMsg::OnSaveLayerMsg(WPARAM wparam, LPARAM lparam)
	{
		ImLayer::tagLayer tReference;

		long long nBackground = m_cImCenter.GetBackground();

		if (!m_cImCenter.Get(nBackground, tReference)) return 0; // 以背景图层为参考

		vector<long long> vSign;

		m_cImCenter.GetSign(vSign);

		for (auto& iSign : vSign) SaveLayer(iSign, tReference);

		AfxMessageBox(L"Finish");

		return 0;
	}

	/*LRESULT CImLayerMsg::OnSaveBackgroundMsg(WPARAM wparam, LPARAM lparam)
	{
		SaveBackgroundLayer();
		return 0;
	}*/

	void CImLayerMsg::SaveLayer(long long& nSign, ImLayer::tagLayer& tReference)
	{
		ImLayer::tagLayer tLayer;

		if (!m_cImCenter.Get(nSign, tLayer)) return;

		if (!tLayer.bEnable) return;

		SaveLayer(tLayer, tReference);
	}

	void CImLayerMsg::SaveLayer(ImLayer::tagLayer& tLayer, ImLayer::tagLayer& tReference)
	{
		if (tLayer.cRotated.Empty()) return;

		Mat cImage = tLayer.cRotated.Get()->clone();

		auto iS1 = tLayer.ScalingFactor(); // 已参照物缩放倍率为基准

		auto iS2 = tReference.ScalingFactor();

		double dSx = iS1[0] / iS2[0], dSy = iS1[1] / iS2[1];

		Size2f szRotate(cImage.cols, cImage.rows);

		Size2f szScale(szRotate.width * dSx, szRotate.height * dSy);

		Mat M(Mat::zeros(2, 3, CV_64F));

		M.at<double>(0, 0) = dSx;

		M.at<double>(1, 1) = dSy;

		warpAffine(cImage, cImage, M, szScale, INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0, 0));

		string strFile = m_strLayerDir + "\\" + to_string(tLayer.nSign) + (tLayer.bTIFF ? ".tif": ".png");

		tLayer.bTIFF ? WriteTif(tLayer.strTIFF, strFile, cImage) : [&strFile, &cImage]() {imwrite(strFile, cImage); }();
	}

	/*void CImLayerMsg::SaveBackgroundLayer()
	{
		TCHAR szPath[MAX_PATH];

		DWORD dwResult = GetCurrentDirectory(MAX_PATH, szPath);

		CString strFile = szPath;

		CTime t = CTime::GetCurrentTime();

		CString str = t.Format(L"\\image\\%Y%m%d%H%M%S.png");

		strFile = strFile + str;

		ImLayer::tagLayer tReference;

		long long nBackground = m_cImCenter.GetBackground();

		if (!m_cImCenter.Get(nBackground, tReference)) return ; 

		if (tReference.cRotated.Empty()) return;

		CString temps = szPath;

		Mat cImage = tReference.cRotated.Get()->clone();
		Mat cImage1 = tReference.cPreview.Get()->clone();
		Mat cImage2 = tReference.cBRGA.Get()->clone();
		Mat cImage3 = tReference.cScaled.Get()->clone();
		Mat cImage4 = tReference.cContrast.Get()->clone();
		Mat cImage5 = tReference.cImage.Get()->clone();

		imwrite(CString2String(strFile).c_str(), cImage);
	}*/
	/*********************************************************************************************/
	IMPLEMENT_DYNAMIC(CImParamMsg, CImLayerMsg)
	BEGIN_MESSAGE_MAP(CImParamMsg, CImLayerMsg)

		ON_MESSAGE(PQM_IM_PARAM_LEFT_CHANGED, OnLeftChangedMsg)
		ON_MESSAGE(PQM_IM_PARAM_TOP_CHANGED, OnTopChangedMsg)
		ON_MESSAGE(PQM_IM_PARAM_WIDTH_CHANGED, OnWidthChangedMsg)
		ON_MESSAGE(PQM_IM_PARAM_HEIGHT_CHANGED, OnHeightChangedMsg)
		ON_MESSAGE(PQM_IM_PARAM_OPACITY_CHANGED, OnOpacityChangedMsg)
		ON_MESSAGE(PQM_IM_PARAM_HOR_SCAL_CHANGED, OnHorScalChangedMsg)
		ON_MESSAGE(PQM_IM_PARAM_VER_SCAL_CHANGED, OnVerScalChangedMsg)
		ON_MESSAGE(PQM_IM_PARAM_ANGLE_CHANGED, OnAngleChangedMsg)

		ON_MESSAGE(PQM_IM_PARAM_CLICKED_ASSOCIATED, OnClickedAssociatedMsg)
		ON_MESSAGE(PQM_IM_PARAM_CLICKED_MOVING, OnClickedMovingMsg)
		ON_MESSAGE(PQM_IM_PARAM_CLICKED_SCALING, OnClickedScalingMsg)
		ON_MESSAGE(PQM_IM_PARAM_CLICKED_ROTATING, OnClickedRotatingMsg)
		ON_MESSAGE(PQM_IM_PARAM_CLICKED_SELECTING, OnClickedSelectingMsg)
		ON_MESSAGE(PQM_IM_PARAM_CLICKED_COPY, OnClickedCopyMsg)
		ON_MESSAGE(PQM_IM_PARAM_CLICKED_SAVE, OnClickedSaveMsg)
		ON_MESSAGE(PQM_IM_PARAM_CLICKED_SELECTALL, OnClickedSelectAllMsg)

	END_MESSAGE_MAP()

	CImParamMsg::CImParamMsg()
	{
		
	}

	CImParamMsg::~CImParamMsg()
	{

	}

	void CImParamMsg::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		CImLayerMsg::OnCreateEx(lpCreateStruct);

		m_cImParam.Bind(this);
	}

	void CImParamMsg::OnSync(ImLayer::tagLayer& tBackground, ImLayer::tagLayer& tFocus, ImLayer::tagLayer& tSign)
	{
		CImLayerMsg::OnSync(tBackground, tFocus, tSign);

		m_cImParam.OnSyncParam(tFocus); // 参数跟随焦点图层
	}

	LRESULT CImParamMsg::OnLeftChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uLeft;

		uLeft.w = wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Left(nSign, uLeft.n);

		return 0;
	}

	LRESULT CImParamMsg::OnTopChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uTop;

		uTop.w = wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Top(nSign, uTop.n);

		return 0;
	}

	LRESULT CImParamMsg::OnWidthChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uWidth;

		uWidth.w = wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Width(nSign, uWidth.n);

		return 0;
	}

	LRESULT CImParamMsg::OnHeightChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uHeight;

		uHeight.w = wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Height(nSign, uHeight.n);

		return 0;
	}

	LRESULT CImParamMsg::OnOpacityChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uOpacity;

		uOpacity.w = wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Opacity(nSign, uOpacity.d);

		return 0;
	}

	LRESULT CImParamMsg::OnHorScalChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uHor;

		uHor.w = wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.HorScal(nSign, uHor.d);

		return 0;
	}

	LRESULT CImParamMsg::OnVerScalChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uVer;

		uVer.w = wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.VerScal(nSign, uVer.d);

		return 0;
	}

	LRESULT CImParamMsg::OnAngleChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uAngle;

		uAngle.w = wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Angle(nSign, uAngle.d);

		return 0;
	}

	LRESULT CImParamMsg::OnClickedAssociatedMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uAssociate;

		uAssociate.w = wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Associate(nSign, uAssociate.w);

		return 0;
	}

	LRESULT CImParamMsg::OnClickedMovingMsg(WPARAM wparam, LPARAM lparam)
	{
		long long nSign = (long long)lparam;

		m_cImCenter.Moving(nSign);

		return 0;
	}

	LRESULT CImParamMsg::OnClickedScalingMsg(WPARAM wparam, LPARAM lparam)
	{
		long long nSign = (long long)lparam;

		m_cImCenter.Scaling(nSign);

		return 0;
	}

	LRESULT CImParamMsg::OnClickedRotatingMsg(WPARAM wparam, LPARAM lparam)
	{
		long long nSign = (long long)lparam;

		m_cImCenter.Rotating(nSign);

		return 0;
	}

	LRESULT CImParamMsg::OnClickedSelectingMsg(WPARAM wparam, LPARAM lparam)
	{
		long long nSign = (long long)lparam;

		m_cImCenter.Selecting(nSign);

		return 0;
	}

	LRESULT CImParamMsg::OnClickedCopyMsg(WPARAM wparam, LPARAM lparam)
	{
		PostMessage(PQM_IM_CONTROL_COPY_LAYER);

		return 0;
	}

	LRESULT CImParamMsg::OnClickedSaveMsg(WPARAM wparam, LPARAM lparam)
	{
		PostMessage(PQM_IM_CONTROL_SAVE_LAYER, wparam, lparam);

		return 0;
	}
	LRESULT CImParamMsg::OnClickedSelectAllMsg(WPARAM wparam, LPARAM lparam)
	{
		m_cImLayers.OnSelectAll();

		return 0; 
	}
	/*********************************************************************************************/
	IMPLEMENT_DYNAMIC(CImViewMsg, CImParamMsg)
	BEGIN_MESSAGE_MAP(CImViewMsg, CImParamMsg)

		ON_MESSAGE(PQM_IM_VIEW_CENTER, OnImCenterMsg)
		ON_MESSAGE(PQM_IM_VIEW_MOVING, OnImMovingMsg)
		ON_MESSAGE(PQM_IM_VIEW_MOVED, OnImMovedMsg)
		ON_MESSAGE(PQM_IM_VIEW_SCALING, OnImScalingMsg)
		ON_MESSAGE(PQM_IM_VIEW_SCALED, OnImScaledMsg)
		ON_MESSAGE(PQM_IM_VIEW_SCALE, OnImScaleMsg)
		ON_MESSAGE(PQM_IM_VIEW_ROTATING, OnImRotatingMsg)
		ON_MESSAGE(PQM_IM_VIEW_ROTATED, OnImRotatedMsg)
		ON_MESSAGE(PQM_IM_VIEW_CONTRAST, OnImContrastMsg)
	END_MESSAGE_MAP()

	CImViewMsg::CImViewMsg()
	{

	}

	CImViewMsg::~CImViewMsg()
	{

	}

	array<int, 2> CImViewMsg::GetImControlRenderSize()
	{
		int* lpSize = new int[2];

		SendMessage(PQM_IM_CONTROL_SYNC_RENDER_SIZE, 0, (LPARAM)lpSize);

		array<int, 2> vSize{ lpSize[0], lpSize[1] };

		delete lpSize;

		return vSize;
	}

	void CImViewMsg::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		CImParamMsg::OnCreateEx(lpCreateStruct);

		m_cImView.Bind(this);

		m_cImView.SetCenter(&m_cImCenter);
	}

	void CImViewMsg::OnSync(ImLayer::tagLayer& tBackground, ImLayer::tagLayer& tFocus, ImLayer::tagLayer& tSign)
	{
		CImParamMsg::OnSync(tBackground, tFocus, tSign);

		m_cImView.OnSyncParam(tFocus, tBackground); // 鼠标icon跟随焦点图层 // 绘图应当在背景完成之后
	}

	LRESULT CImViewMsg::OnImCenterMsg(WPARAM wparam, LPARAM lparam)
	{
		//Point2f* lpPtCenter = (Point2f*)lparam;

		//ImLayer::tagLayer tFocus;

		//long long nFocus = m_cImCenter.GetFocusLayer();

		//if (!m_cImCenter.GetViewParam(nFocus, tFocus)) return 0;

		//auto ptCenter = tFocus.Center();

		//lpPtCenter->x = ptCenter.x;

		//lpPtCenter->y = ptCenter.y;

		return 0;
	}

	LRESULT CImViewMsg::OnImMovingMsg(WPARAM wparam, LPARAM lparam)
	{
		POINT* lpPt = (POINT*)wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Moving(nSign, lpPt->x, lpPt->y);

		delete lpPt;

		return 0;
	}

	LRESULT CImViewMsg::OnImMovedMsg(WPARAM wparam, LPARAM lparam)
	{
		POINT* lpPt = (POINT*)wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Move(nSign, lpPt->x, lpPt->y);

		delete lpPt;

		return 0;
	}

	LRESULT CImViewMsg::OnImScalingMsg(WPARAM wparam, LPARAM lparam)
	{
		POINT* lpPt = (POINT*)wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Scaling(nSign, lpPt->x, lpPt->y);

		delete lpPt;

		return 0;
	}

	LRESULT CImViewMsg::OnImScaledMsg(WPARAM wparam, LPARAM lparam)
	{
		POINT* lpPt = (POINT*)wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Scale(nSign, lpPt->x, lpPt->y);

		delete lpPt;

		return 0;
	}

	LRESULT CImViewMsg::OnImScaleMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uScale;

		uScale.w = wparam;

		long long nSign = (long long)lparam;

		long long nBackground = m_cImCenter.GetBackground();

		nSign != nBackground ? m_cImCenter.Scale(nSign, uScale.d) : m_cImCenter.Scale(uScale.d);

		return 0;
	}

	LRESULT CImViewMsg::OnImRotatingMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uAngle;

		uAngle.w = wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Rotating(nSign, uAngle.d);

		return 0;
	}

	LRESULT CImViewMsg::OnImRotatedMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::uWParam uAngle;

		uAngle.w = wparam;

		long long nSign = (long long)lparam;

		m_cImCenter.Rotate(nSign, uAngle.d);

		return 0;
	}
	LRESULT CImViewMsg::OnImContrastMsg(WPARAM wparam, LPARAM lparam)
	{
		m_cImCenter.Contrast(wparam);


		return 0;
	}
	/*********************************************************************************************/
	IMPLEMENT_DYNAMIC(CImControl, CImViewMsg)
	BEGIN_MESSAGE_MAP(CImControl, CImViewMsg)
		//ON_WM_PAINT()
		//ON_WM_MOUSEMOVE()
		//ON_WM_LBUTTONDOWN()
		//ON_WM_LBUTTONUP()
		//ON_WM_RBUTTONUP()
		ON_MESSAGE(PQM_IM_CENTER_SYNC, OnSyncMsg)
		ON_MESSAGE(PQM_IM_CONTROL_SYNC_RENDER_SIZE, OnSyncRenderSizeMsg)
		ON_MESSAGE(PQM_IM_CONTROL_SYNC_SELECTED, OnSyncSelectedMsg)
	END_MESSAGE_MAP()

	CImControl::CImControl()
		:m_lpUserParam(nullptr)
	{
		//m_tPointBegin = CPoint(0, 0);
		//m_tPointEnd = m_tPointBegin;
	}

	CImControl::~CImControl()
	{

	}

	void CImControl::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		CImViewMsg::OnCreateEx(lpCreateStruct);

		m_cImCenter.Bind(this);
	}

	LRESULT CImControl::OnSyncMsg(WPARAM wparam, LPARAM lparam)
	{
		ImLayer::tagLayer tSign, tFocus, tBackground;

		long long nSign = (long long)lparam;

		long long nFocus = m_cImCenter.GetFocusLayer();

		long long nBackground = m_cImCenter.GetBackground();

		if (!m_cImCenter.GetSyncInfo(nBackground, tBackground) || 
			!m_cImCenter.GetSyncInfo(nFocus, tFocus) ||
			!m_cImCenter.GetSyncInfo(nSign, tSign)) return 0;

		CImViewMsg::OnSync(tBackground, tFocus, tSign);

		return 0;
	}

	LRESULT CImControl::OnSyncRenderSizeMsg(WPARAM wparam, LPARAM lparam)
	{
		int* lpSize = (int*)lparam;

		m_cImView.GetRenderSize(lpSize[0], lpSize[1]);

		return 0;
	}

	LRESULT CImControl::OnSyncSelectedMsg(WPARAM wparam, LPARAM lparam)
	{
		Rect* lpWin = (Rect*)lparam;

		*lpWin = m_cImView.GetFocusRect(); // 选择框在背景中位置

		return 0;
	}

	/*void CImControl::OnPaint()
	{
		CPaintDC tPaintDC(this);

		CRect tRect(m_tPointBegin, m_tPointEnd);

		CBrush tBrush;
		tBrush.CreateStockObject(NULL_BRUSH);
		CBrush* ptOldBrush = tPaintDC.SelectObject(&tBrush);

		CPen tPen(PS_SOLID, 1, RGB(0, 200, 0));
		CPen* ptOldPen = tPaintDC.SelectObject(&tPen);

		tPaintDC.Rectangle(&tRect);

		tPaintDC.SelectObject(ptOldPen);
		tPaintDC.SelectObject(ptOldBrush);
	}

	void CImControl::OnMouseMove(UINT uFlags, CPoint tPoint)
	{
		if (m_bMousePressed) {
			m_tPointEnd = tPoint;

			Invalidate(NULL);
		}
	}

	void CImControl::OnLButtonDown(UINT uFlags, CPoint tPoint)
	{
		this->SetCapture();

		m_tPointBegin = tPoint;
		m_tPointEnd = tPoint;
		m_bMousePressed = true;
	}

	void CImControl::OnLButtonUp(UINT uFlags, CPoint tPoint)
	{
		m_bMousePressed = false;

		ReleaseCapture();
	}

	void CImControl::OnRButtonUp(UINT uFlags, CPoint tPoint)
	{
		m_tPointBegin = CPoint(0, 0);
		m_tPointEnd = m_tPointBegin;

		Invalidate(NULL);
	}*/
}