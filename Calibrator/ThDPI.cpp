#include "pch.h"
#include "ThDPI.h"
#include "Transform.h"
#include "Helper.h"
#include <numeric>

CThDPI::CThDPI():
	m_vKernel{0}
{

}

CThDPI::~CThDPI()
{

}

bool CThDPI::Reg()
{
	if (m_uWorkType == eWT_DPI)
	{
		CIm2Mat::SetSignal(bind(&CThDPI::OnImCache, this, placeholders::_1), bind(&CThDPI::IsIdle, this));

		CImPadding::SetThParam(&m_cImCache, bind(&CThDPI::DPI, this));
	}
	return CThMergingEx::Reg();
}

void CThDPI::Unreg()
{
	CThMergingEx::Unreg();

	Clear();
}

void CThDPI::SetDPIParam(vector<array<wstring, 2>>& vROI, array<int, 3> vKernel, string strDPI, string strCacheDir)
{
	m_strDPI = strDPI; 
	
	m_strCacheDir = strCacheDir;

	m_vKernel = vKernel;

	vector<vector<array<string, 2>>> vCache;

	for (auto& iROI : vROI)
	{
		vCache.push_back({ { Wstring2String(iROI[0]), Wstring2String(iROI[1]) } });
	}

	SetFiles(vCache);
}

bool CThDPI::IsIdle()
{
	int nSize = 0; static int g_nTh = 10;
	{
		CSRLock lock(&m_lLock);

		nSize = m_vImCache.size();
	}
	return nSize < g_nTh;
}

void CThDPI::OnImCache(lpIm2Mat& lpCache)
{
	Push(lpCache);

	m_cImCache.signal();
}

void CThDPI::Push(lpIm2Mat& lpCache)
{
	CSWLock lock(&m_lLock);

	m_vImCache.push_back(lpCache);
}

bool CThDPI::Pop(lpIm2Mat& lpCache)
{
	bool bEmpty = true;
	{
		CSWLock lock(&m_lLock);

		if (!(bEmpty = m_vImCache.empty()))
		{
			auto iFront = m_vImCache.begin();

			lpCache = *iFront;

			iFront = m_vImCache.erase(iFront);
		}
	}
	return !bEmpty;
}

void CThDPI::Clear()
{
	CSWLock lock(&m_lLock);

	m_vImCache.clear();
}

void CThDPI::Connected(Mat& cIm, vector<Rect>& vCon)
{
	Mat labels = Mat::zeros(cIm.rows, cIm.cols, CV_32S), stats, centroids;

	connectedComponentsWithStats(cIm, labels, stats, centroids);

	auto lpCon = (int*)stats.data;

	for (int y = 1; y < stats.rows; ++y)
		vCon.push_back({ lpCon[y * 5 + CC_STAT_LEFT], lpCon[y * 5 + CC_STAT_TOP], lpCon[y * 5 + CC_STAT_WIDTH], lpCon[y * 5 + CC_STAT_HEIGHT] });
}

void CThDPI::Hor(Mat& cTmp, Rect& vLRT, Rect& vLRB)
{
	Mat hMask;

	resize(cTmp, hMask, Size(cTmp.cols * 0.1, cTmp.rows * 0.1));

	vector<Rect> vCon; Connected(hMask, vCon);

	Rect rtCache[2] = { Rect() };

	sort(vCon.begin(), vCon.end(), [&hMask](Rect& rt1, Rect& rt2) { return (rt1.x + rt1.y) < (rt2.x + rt2.y); }); rtCache[0] = *vCon.begin();

	sort(vCon.begin(), vCon.end(), [&hMask](Rect& rt1, Rect& rt2) { return (hMask.cols - rt1.x + rt1.y) < (hMask.cols - rt2.x + rt2.y); }); rtCache[1] = *vCon.begin();

	vLRT = rtCache[0] | rtCache[1];

	sort(vCon.begin(), vCon.end(), [&hMask](Rect& rt1, Rect& rt2) { return (rt1.x + hMask.rows - rt1.y) < (rt2.x + hMask.rows - rt2.y); }); rtCache[0] = *vCon.begin();

	sort(vCon.begin(), vCon.end(), [&hMask](Rect& rt1, Rect& rt2) { return (hMask.cols - rt1.x + hMask.rows - rt1.y) < (hMask.cols - rt2.x + hMask.rows - rt2.y); }); rtCache[1] = *vCon.begin();

	vLRB = rtCache[0] | rtCache[1];
}

void CThDPI::Hor2Hor(Rect& rtCol)
{
	rtCol.width = (rtCol.width + 2) * 10;

	rtCol.height = (rtCol.height + 2) * 10;

	rtCol.x = (rtCol.x - 1) * 10;

	rtCol.y = (rtCol.y - 1) * 10;
}

void CThDPI::Ver2Ver(Rect& rtRow)
{
	rtRow.width = (rtRow.width + 2) * 10;

	rtRow.height = (rtRow.height + 2) * 10;

	rtRow.x = (rtRow.x - 1) * 10;

	rtRow.y = (rtRow.y - 1) * 10;
}

void CThDPI::Ver(Mat& cTmp, Rect& vTBL, Rect& vTBR)
{
	Mat vMask;

	resize(cTmp, vMask, Size(cTmp.cols * 0.1, cTmp.rows * 0.1));

	vector<Rect> vCon; Connected(vMask, vCon);

	Rect rtCache[2] = { Rect() };

	sort(vCon.begin(), vCon.end(), [&vMask](Rect& rt1, Rect& rt2) { return (rt1.x + rt1.y) < (rt2.x + rt2.y); }); rtCache[0] = *vCon.begin();

	sort(vCon.begin(), vCon.end(), [&vMask](Rect& rt1, Rect& rt2) { return (rt1.x + vMask.rows - rt1.y) < (rt2.x + vMask.rows - rt2.y); }); rtCache[1] = *vCon.begin();

	vTBL = rtCache[0] | rtCache[1];

	sort(vCon.begin(), vCon.end(), [&vMask](Rect& rt1, Rect& rt2) { return (vMask.cols - rt1.x + rt1.y) < (vMask.cols - rt2.x + rt2.y); }); rtCache[0] = *vCon.begin();

	sort(vCon.begin(), vCon.end(), [&vMask](Rect& rt1, Rect& rt2) { return (vMask.cols - rt1.x + vMask.rows - rt1.y) < (vMask.cols - rt2.x + vMask.rows - rt2.y); }); rtCache[1] = *vCon.begin();

	vTBR = rtCache[0] | rtCache[1];
}

void CThDPI::Mask(Mat& cMask, Mat& cImMat)
{
	for (uint64 y = 0; y < cImMat.rows; ++y)
	{
		for (uint64 x = 0; x < cImMat.cols; ++x)
		{
			uint64 idx = y * cImMat.cols + x;

			if (cMask.data[idx] < 0xff) continue;

			cImMat.data[idx] = 0;
		}
	}
}

void CThDPI::Hor(Mat*& cIm, Mat*& cMask, Rect& rtCol, double& c)
{
	Mat& iCol = *cIm, iThCol;

	Mat* lpCol = &iCol, * lpThCol = &iThCol;

	cuThinging(lpCol, lpThCol);

	Mask(*cMask, iThCol); delete cIm, delete cMask;

	int64 pix = 0, size = 0;	// 对iThCol 求y

	for (int64 y = 0; y < iThCol.rows; ++y)
	{
		for (int64 x = 0; x < iThCol.cols; ++x)
		{
			int64 idx = y * iThCol.cols + x;

			auto& iPix = iThCol.data[idx];

			if (!!!iPix) continue;

			pix += y; ++size;
		}
	}
	c = (double)pix / size + rtCol.y;
}

void CThDPI::Ver(Mat*& cIm, Mat*& cMask, Rect& rtRow, double& c)
{
	Mat& iRow = *cIm, iThRow;

	Mat* lpRow = &iRow, * lpThRow = &iThRow;

	cuThinging(lpRow, lpThRow);

	Mask(*cMask, iThRow); delete cIm, delete cMask;

	int64 pix = 0, size = 0;	// 对iThRow 求x

	for (int64 y = 0; y < iThRow.rows; ++y)
	{
		for (int64 x = 0; x < iThRow.cols; ++x)
		{
			int64 idx = y * iThRow.cols + x;

			auto& iPix = iThRow.data[idx];

			if (!!!iPix) continue;

			pix += x; ++size;
		}
	}
	c = (double)pix / size + rtRow.x;
}

void CThDPI::Vertex(array<double, 4>& c, array<Point, 4>& v)
{
	auto& ptTL = v[0], & ptTR = v[1], & ptBL = v[2], & ptBR = v[3];

	ptTL.x = c[2], ptTL.y = c[0];

	ptTR.x = c[3], ptTR.y = c[0];

	ptBL.x = c[2], ptBL.y = c[1];

	ptBR.x = c[3], ptBR.y = c[1];
}

void CThDPI::DrawVertex(array<Point, 4>& v, Mat& cImage, int& nRemaining)
{
	auto& ptTL = v[0], & ptTR = v[1], & ptBL = v[2], & ptBR = v[3];

	Mat cTmp;

	cvtColor(cImage, cTmp, COLOR_GRAY2BGR);

	line(cTmp, ptTL, ptTR, Scalar(255, 0, 0), 5);

	line(cTmp, ptBL, ptBR, Scalar(0, 255, 0), 5);

	line(cTmp, ptTL, ptBL, Scalar(0, 0, 255), 5);

	line(cTmp, ptTR, ptBR, Scalar(255, 255, 0), 5);

	imwrite(m_strCacheDir + "\\DPI_" + to_string(!!!nRemaining) + ".png", cTmp);
}

void CThDPI::Scale(array<Point, 4>& v1, array<Point, 4>& v2, double& sx, double& sy)
{
	auto& ptTL1 = v1[0], & ptBR1 = v1[3];

	auto& ptTL2 = v2[0], & ptBR2 = v2[3];

	double dx1 = abs(ptBR1.x - ptTL1.x), dy1 = abs(ptBR1.y - ptTL1.y);

	double dx2 = abs(ptBR2.x - ptTL2.x), dy2 = abs(ptBR2.y - ptTL2.y);

	sx = dx1 / dx2, sy = dy1 / dy2;
}

void CThDPI::Combine(array<Point, 4>& v1, array<Point, 4>& v2, double& sx, double& sy)
{
	Mat* lpIm1 = new Mat, * lpIm2 = new Mat;

	*lpIm1 = imread(m_strCacheDir + "\\DPI_0.png", IMREAD_COLOR);

	*lpIm2 = imread(m_strCacheDir + "\\DPI_1.png", IMREAD_COLOR);

	resize(*lpIm2, *lpIm2, Size(sx * lpIm2->cols, sy * lpIm2->rows));

	for (auto& iv : v2) { iv.x = sx * iv.x; iv.y = sy * iv.y; }

	auto& ptTL1 = v1[0], & ptTL2 = v2[0];

	auto ox = ptTL2.x - ptTL1.x, oy = ptTL2.y - ptTL1.y;

	Rect rt1(0, 0, lpIm1->cols, lpIm1->rows);

	Rect rt2(0, 0, lpIm2->cols, lpIm2->rows);

	rt2.x -= ox, rt2.y -= oy;

	rt1 = rt1 & rt2;

	rt2.x = rt1.x + ox;

	rt2.y = rt1.y + oy;

	rt2.width = rt1.width, rt2.height = rt1.height;

	Mat cImROI1 = (*lpIm1)(rt1).clone(); delete lpIm1;

	Mat cImROI2 = (*lpIm2)(rt2).clone(); delete lpIm2;

	addWeighted(cImROI1, 0.5, cImROI2, 0.5, 0.0, cImROI1);

	imwrite(m_strCacheDir + "\\DPI.png", cImROI1);
}

void CThDPI::InstallDPIParam(string strDpi, array<double, 2> vDPI)
{
	Mat cParam(Mat::zeros(1, 2 * sizeof(double), CV_8UC1));

	memcpy(cParam.data, vDPI.data(), cParam.cols);

	imwrite(strDpi.c_str(), cParam);
}

void CThDPI::DPI()
{
	lpIm2Mat lpCache = nullptr; // 求出TL, BR两点

	if (!Pop(lpCache)) { m_cImCache.wait(); return; }

	if (lpCache->vIm2Mat.empty()) return;

	auto& iImMat = *lpCache->vIm2Mat[0];

	GaussianBlur(iImMat, iImMat, cv::Size(7, 7), 1.5, 0);

	threshold(iImMat, iImMat, m_vKernel[0], 255, THRESH_BINARY_INV);

	array<array<Point2f, 2>, 2> vCol, vRow;

	Mat vErode = Mat::ones(m_vKernel[1], 1, CV_8UC1); // ver

	Mat hErode = Mat::ones(1, m_vKernel[1], CV_8UC1); // hor

	Mat vDilate = Mat::ones(m_vKernel[2], 1, CV_8UC1); // ver

	Mat hDilate = Mat::ones(1, m_vKernel[2], CV_8UC1); // hor

	Mat* vMask = new Mat, * hMask = new Mat, * vMask2 = new Mat, * hMask2 = new Mat, * cMask = new Mat;

	erode(iImMat, *vMask, vErode);

	erode(iImMat, *hMask, hErode);

	dilate(*vMask, *vMask2, vDilate);

	dilate(*hMask, *hMask2, hDilate);

	addWeighted(*vMask2, 0.5, *hMask2, 0.5, 0.0, *cMask); delete vMask2; delete hMask2;

	threshold(*cMask, *cMask, 200, 255, THRESH_BINARY); // 求出mask

	Mat cDilate = Mat::ones(100, 100, CV_8UC1);

	dilate(*cMask, *cMask, cDilate);

	Rect vLRT, vLRB, vTBL, vTBR;

	Hor(*hMask, vLRT, vLRB);

	Hor2Hor(vLRT); Hor2Hor(vLRB);

	Mat* cLRT = new Mat, * cLRB = new Mat, * cLRTMask = new Mat, * cLRBMask = new Mat;

	*cLRT = (*hMask)(vLRT).clone();

	*cLRB = (*hMask)(vLRB).clone();	delete hMask;

	*cLRTMask = (*cMask)(vLRT).clone();

	*cLRBMask = (*cMask)(vLRB).clone();

	Ver(*vMask, vTBL, vTBR);

	Ver2Ver(vTBL); Ver2Ver(vTBR);

	Mat* cTBL = new Mat, * cTBR = new Mat, * cTBLMask = new Mat, * cTBRMask = new Mat;

	*cTBL = (*vMask)(vTBL).clone();

	*cTBR = (*vMask)(vTBR).clone(); delete vMask;

	*cTBLMask = (*cMask)(vTBL).clone();

	*cTBRMask = (*cMask)(vTBR).clone(); delete cMask;

	array<double,4> c = { 0.0 };

	Hor(cLRT, cLRTMask, vLRT, c[0]);

	Hor(cLRB, cLRBMask, vLRB, c[1]);

	Ver(cTBL, cTBLMask, vTBL, c[2]);

	Ver(cTBR, cTBRMask, vTBR, c[3]);

	auto&v = m_ptVertex[!!!lpCache->nRemaining];

	Vertex(c, v);

	DrawVertex(v, iImMat, lpCache->nRemaining);

	if (!!lpCache->nRemaining) { delete lpCache; return; }

	double sx = 0.0, sy = 0.0;

	Scale(m_ptVertex[0], m_ptVertex[1], sx, sy);

	Combine(m_ptVertex[0], m_ptVertex[1], sx, sy);

	InstallDPIParam(m_strDPI, { sx, sy });

	OnDPI(sx, sy);

	delete lpCache;
}
