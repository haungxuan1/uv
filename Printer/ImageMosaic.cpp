#include "pch.h"
#include "ImageMosaic.h"
#include "PrClock/PrClock.h"
#include "Helper.h"
#include "PQMsg.h"

CImageMosaic::CImageMosaic() :
	m_bThread(false), m_bTermination(false)
{
	m_vThParam = make_tuple(
		__FUNCTION__,
		this,
		&m_lLock,
		&m_cImLine,
		&m_cTermination,
		&m_bTermination,
		&m_vParams,
		bind(&CImageMosaic::OnImMosaic, this, placeholders::_1));
}

CImageMosaic::~CImageMosaic()
{

}

bool CImageMosaic::Reg()
{
	if (!CImTransparent::Reg()) return false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	Restart(m_bTermination, m_lLock);

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle);

	return (m_bThread = (!!lpHandle));
}

void CImageMosaic::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CImTransparent::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CImageMosaic::AllocateMmemory()
{
	CImTransparent::AllocateMmemory();

	if (m_uPrinterType == ePT_Calibrator || m_uPrinterType == ePT_Print || m_uPrinterType == ePT_FastPrint) // 临时补丁
	{
		m_bFinish = false;

		m_ptTL = { 0, 0 }; m_nImMosaic = { 0, 0, 0 };

		m_nTempDiameter = sqrt(pow(m_cTemplate.cols, 2) + pow(m_cTemplate.rows, 2)) * 1.1;	// 移动距离 = 视野 - 1.1* Temp.最小外接圆直径

		double dPr2Camera = m_dCameraDPI[1] / m_vPrintDPI[1]; // y方向缩放比例

		double dX2Y = m_dCameraDPI[1] / m_dCameraDPI[0];

		// m_vOffset[1] 被 fleece填充
		m_nImMosaic[0] = m_vPrtHeader[1] * dPr2Camera - m_vOffset[1] + (m_vOffset[1] * (m_uPrinterType == ePT_FastPrint)); /*- m_vOffset[1]*/;

		m_cImSight = Mat(Mat::zeros(m_vSight[2], dX2Y * m_vImWH2[0] * m_nImInLine, CV_8UC1));

		m_cImCache = Mat(Mat::zeros(m_cImSight.rows, m_cImSight.cols, CV_8UC1));
	}
}

void CImageMosaic::DeallocateMemory()
{
	m_cImSign = Mat(); m_cImSight = Mat(); m_cImCache = Mat(); 
	
	m_vParams.clear();

	CImTransparent::DeallocateMemory();
}

void CImageMosaic::ImMosaic(vParam& iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cImLine.signal();
}

void CImageMosaic::OnImMosaic(vParam& iParam)
{
	auto& iFrameId = get<ePI_FrameId>(iParam);

	auto iImLine = get<ePI_ImPtr>(iParam);

	// 打印.高 = (填充.高 + 拼接.高) * dpi转换比例（m_vPrinterRatio[1]）
	// 拼接.高 = 溢出视野(oy) + 拼接.高(m_nImMosaic)
	// 打印.高 <= （填充.高 + 拼接.高 ）, 发送完整视野，停止打印
	if (m_bFinish) return;

	if (InitSign(iImLine)) { return; } // 初始化视野 -> 视野必须大于 图像帧.宽(旋转为图像行.高)

	if (m_nImMosaic[0] <= m_nImMosaic[1])
	{
		int64 nOut = m_nImMosaic[1] - m_nImMosaic[0];

		int64 nCy = max(1ll, m_nImMosaic[2] - nOut);

		get<ePI_UserParam>(iParam) = new array<int64, 3>{ true, m_nImMosaic[3], nCy }; // 完成 | 上移(对齐) | 实际发出

		SendSight(iParam); m_bFinish = true; 
		
		return; // 设置表示位, 不在响应
	}
	ImAlignment(get<ePI_FrameId>(iParam), iImLine);

	if (OutSight(iImLine))
	{
		m_nRerverse = m_nImMosaic[2] - min((int64)m_ptTL.y, m_nImMosaic[2] - m_nTempDiameter);

		get<ePI_UserParam>(iParam) = new array<int64, 3>{ false, m_nImMosaic[3], m_nImMosaic[2] - m_nRerverse}; // 是否完成 | 上移 | 本次发出

		SendSight(iParam);

		SightMove(get<ePI_FrameId>(iParam));	// 超出视野，则移动
	}
	ImMerging(get<ePI_FrameId>(iParam), iImLine);
}

void CImageMosaic::SendSight(vParam& iParam)
{
	auto& iFrameId = get<ePI_FrameId>(iParam);

	auto& iImPtr = get<ePI_ImPtr>(iParam);

	iImPtr = ImPtr::CImPtr(new Mat(m_nImMosaic[2], m_vSight[1], CV_8UC1));

	m_cImSight(Rect(m_vSight[0], 0, iImPtr.Get()->cols, iImPtr.Get()->rows)).copyTo(*iImPtr.Get());

	cout << __FUNCTION__ << ": " << iFrameId << endl;

	get<ePI_Timer>(iParam).push_back({ __FUNCTION__, clock() });

	m_uPrinterType == ePT_Calibrator ? CImTransparent::ImTransparent(iParam) : CPosition::Position(iParam);
}

void CImageMosaic::MatchTemplate(ImPtr::CImPtr& cTag, string strTemp, double& dScore, Point& ptTL, Rect& rtTemp)
{
	ImPtr::CImPtr cTemp(new Mat);

	*cTemp.Get() = imread(strTemp, 0);

	int resultImage_cols = cTag.Get()->cols - cTemp.Get()->cols + 1;

	int resultImage_rows = cTag.Get()->rows - cTemp.Get()->rows + 1;

	Mat result(resultImage_cols, resultImage_rows, CV_32FC1);

	matchTemplate(*cTag.Get(), *cTemp.Get(), result, TM_CCOEFF_NORMED);

	double minVal = 0.0; Point minLoc;

	minMaxLoc(result, &minVal, &dScore, &minLoc, &ptTL);

	rtTemp = Rect(ptTL.x, ptTL.y, cTemp.Get()->cols, cTemp.Get()->rows);
}

void CImageMosaic::MatchTemplate(ImPtr::CImPtr& cTag, int& nTemp, double& dScore, Point& ptTL, ImPtr::CImPtr& cTemp)
{
	vector<tuple<int, double, Point, Rect>> vQEncode;

	for (int i = 1; i < 31; ++i)
	{
		tuple<int, double, Point, Rect> iQEncode(i, 0.0, Point(), Rect());

		string strTemp = m_strImBaseDir + "MaQencode\\" + to_string(i) + ".png";

		MatchTemplate(cTag, strTemp, get<1>(iQEncode), get<2>(iQEncode), get<3>(iQEncode));

		vQEncode.push_back(iQEncode);
	}
	sort(vQEncode.begin(), vQEncode.end(), [](tuple<int, double, Point, Rect>& iQEncode1, tuple<int, double, Point, Rect>& iQEncode2) {return get<1>(iQEncode2) < get<1>(iQEncode1);});

	auto iTemp = (*vQEncode.begin());

	nTemp = get<0>(iTemp);

	dScore = get<1>(iTemp);

	ptTL = get<2>(iTemp);

	*cTemp.Get() = (*cTag.Get())(get<3>(iTemp)).clone();
}

void CImageMosaic::Tag(ImPtr::CImPtr& cTag, ImPtr::CImPtr& cWin, Rect& rtWin)
{
	ImPtr::CImPtr cTmp(new Mat);

	ImPtr::CImPtr cTmp2(new Mat);

	*cTmp2.Get() = (*cTag.Get())(Rect(0, 0, 5000, cTag.Get()->rows)).clone();

	GaussianBlur(*cTmp2.Get(), *cWin.Get(), cv::Size(7, 7), 1.5, 0);

	threshold(*cWin.Get(), *cWin.Get(), m_nForegroundThreshold, 255, THRESH_BINARY_INV);

	erode(*cWin.Get(), *cTmp.Get(), Mat::ones(10, 10, CV_8UC1)); // 13pixel ≈ 0.1mm

	// 由 CQEncodeDlg::QEncode 可知间隙 = 7mm
	dilate(*cTmp.Get(), *cTmp.Get(), Mat::ones(1000, 300, CV_8UC1));

	Mat labels = Mat::zeros(cTmp.Get()->rows, cTmp.Get()->cols, CV_32S), stats, centroids;

	connectedComponentsWithStats(*cTmp.Get(), labels, stats, centroids);

	rtWin = Rect(0, 0, 1, 1); // 1*1 防止触发 CV_DbgAssert

	auto lpConnected = (int*)stats.data;

	for (int y = 1; y < stats.rows; ++y)
	{
		auto& w = lpConnected[y * 5 + CC_STAT_WIDTH];

		auto& h = lpConnected[y * 5 + CC_STAT_HEIGHT];

		if (w * h <= rtWin.area()) continue;

		rtWin.x = lpConnected[y * 5 + CC_STAT_LEFT];

		rtWin.y = lpConnected[y * 5 + CC_STAT_TOP];

		rtWin.width = w;

		rtWin.height = h;
	}
	
	*cTmp.Get() = (*cWin.Get())(rtWin).clone();

	cWin = cTmp;

	imwrite(m_strImBaseDir + "Win.png", cWin.Get()->clone());
}

void CImageMosaic::ImSight(ImPtr::CImPtr& cTag, int& nLeft, array<int, 2>& vImSize)
{
	Rect rtWin;

	ImPtr::CImPtr cWin(new Mat);

	Tag(cTag, cWin, rtWin);

	int nTemp = 0; double dScore = 0.0; Point ptTL; ImPtr::CImPtr cTemp(new Mat);

	MatchTemplate(cWin, nTemp, dScore, ptTL, cTemp);

	ptTL.x += rtWin.x;

	cout << "Temp: " << nTemp << " | " << dScore << " | " << ptTL << endl;

	imwrite(m_strImBaseDir + "iTemp.png", *cTemp.Get());

	nLeft = ptTL.x;

	vImSize = { cTemp.Get()->cols, cTemp.Get()->rows };
}

bool CImageMosaic::InitSign(ImPtr::CImPtr& cImLine)
{
	if (!m_cImSign.empty()) return false;

	array<int, 2> vImSize{ 1, 1 };
#ifdef PQQEncode
	int nOx = 0; 
	ImSight(cImLine, nOx, vImSize);
	int nCenter = nOx + vImSize[0] * 0.5;
	m_rtSign.x = nCenter - vImSize[0];
	m_vSight[0] = nOx + 2 * vImSize[0];
#else
	ImPtr::CImPtr cTemp(new Mat(1, 1, CV_8UC1));
	m_rtSign.x = 0;
	m_vSight[0] = 0;
#endif // PQSkip

	cout << "s0: " << m_vSight[0] << endl;

	m_rtSign.width = vImSize[0] * 2;

	m_rtSign.y = 0; m_rtSign.height = cImLine.Get()->rows;

	m_vAligParam[1] = sqrt(pow(vImSize[0], 2) + pow(vImSize[1],2)); // 外接圆直径

	m_vAligParam[2] *= m_nSkipLine; // 步进间隔

	m_cImSign = (*cImLine.Get())(m_rtSign).clone();

	imwrite(m_strImBaseDir + "sign.png", m_cImSign);

	Rect rtCopy(0, 0, min(cImLine.Get()->cols, m_cImSight.cols), cImLine.Get()->rows);

	(*cImLine.Get())(rtCopy).copyTo(m_cImSight(rtCopy));

	m_nImMosaic[1] = m_nImMosaic[2] = cImLine.Get()->rows;

	return true;
}

void CImageMosaic::ImAlignment(int& nFrame, ImPtr::CImPtr& cImLine)
{
	Mat cSign = (*cImLine.Get())(m_rtSign).clone();

#ifdef PQQEncode
	imwrite(m_strImBaseDir + to_string(nFrame) + "_sign.png", cSign);
	Mat cBin1, cBin2, cImAlign1, cImAlign2, cImUnite1, cImUnite2;
	std::array<cv::Rect, 2> rtImUnite; std::array<int, 2> vAlign;
	double dScore = 0.0, dTime = 0.0; array<int, 2> vTL{ 0, 0 }; // 上负 | 左负 
	cuAlign(m_cImSign, cSign, cBin1, cBin2, cImAlign1, cImAlign2, cImUnite1, cImUnite2, rtImUnite, vAlign, dScore, dTime, m_vAligParam, vTL);
	m_ptTL.x += vTL[0]; m_ptTL.y += vTL[1];
	cout << "TL: " << dec << nFrame << " | " << m_ptTL.x << ", " << m_ptTL.y << " | " << dScore << " | " << vTL[0] << ", " << vTL[1] << endl;
#else
	int oy = 0;	FleeceParam(oy); // 跳白数据
	m_ptTL.x = 0; m_ptTL.y += oy;
	cout << "oy: " << oy << endl;
	cout << "TL: " << dec << nFrame << " | " << m_ptTL.x << ", " << m_ptTL.y << endl;
#endif // PQSkip

	memcpy(m_cImSign.data, cSign.data, (uint64)((uint64)m_rtSign.width * m_rtSign.height));
}

bool CImageMosaic::OutSight(ImPtr::CImPtr& cImLine)
{
	cout << __FUNCTION__ << ": " << m_ptTL.y + cImLine.Get()->rows << " | " << m_cImSight.rows << endl;

	return (m_cImSight.rows < (m_ptTL.y + cImLine.Get()->rows));
}

void CImageMosaic::SightMove(int& nFrame)
{
	// 移动距离 = 视野 - 1.1* Temp.最小外接圆直径
	m_nImMosaic[3] = m_nImMosaic[2] - m_nRerverse; m_nImMosaic[2] = m_nRerverse;

	Rect rtBack, rtFront;

	rtFront.x = 0, rtFront.width = m_cImSight.cols;

	rtFront.y = 0, rtFront.height = m_cImSight.rows - m_nImMosaic[3];

	rtBack.x = 0, rtBack.width = rtFront.width;

	rtBack.y = m_nImMosaic[3], rtBack.height = rtFront.height;

	auto iBack = m_cImSight(rtBack);

	auto iFront = m_cImSight(rtFront);

	iBack.copyTo(m_cImCache(rtFront));

	m_cImSight.setTo(Scalar(0));

	m_cImCache(rtFront).copyTo(iFront);

	m_ptTL.y -= m_nImMosaic[3];

	cout << __FUNCTION__ << ": " << m_nImMosaic[2] << " | " << m_ptTL.y << endl;
}

void CImageMosaic::ImMerging(int& nFrame, ImPtr::CImPtr& cImLine)
{
	Rect rtLine, rtMosaic;

	rtLine.x = max(0, -m_ptTL.x); rtLine.y = 0;

	rtMosaic.x = max(0, m_ptTL.x); rtMosaic.y = m_ptTL.y;

	rtLine.width = min(cImLine.Get()->cols - abs(m_ptTL.x), (m_cImSight.cols - rtMosaic.x));

	rtLine.height = cImLine.Get()->rows;

	rtMosaic.width = rtLine.width; rtMosaic.height = rtLine.height;

	(*cImLine.Get())(rtLine).copyTo(m_cImSight(rtMosaic));

	m_nImMosaic[1] = m_nImMosaic[1] + rtMosaic.y + rtMosaic.height - m_nImMosaic[2];

	m_nImMosaic[2] = (uint64)rtMosaic.y + rtMosaic.height;

	cout << __FUNCTION__ << ": " << m_nImMosaic[2] << endl;
}