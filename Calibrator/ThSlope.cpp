#include "pch.h"
#include "ThSlope.h"
#include "Helper.h"
#include "opencv2/ximgproc.hpp"
#pragma comment(lib, "opencv_ximgproc470.lib")

CThSlope::CThSlope()
{

}

CThSlope::~CThSlope()
{

}

bool CThSlope::Reg()
{
	if (m_uWorkType == eWT_Slope)
	{
		CIm2Mat::SetSignal(bind(&CThSlope::OnImCache, this, placeholders::_1), bind(&CThSlope::IsIdle, this));

		CImPadding::SetThParam(&m_cImCache, bind(&CThSlope::Slope, this));
	}

	return CThROI::Reg();
}

void CThSlope::Unreg()
{
	CThROI::Unreg();

	Clear();
}

void CThSlope::SetSlopeParam(vector<array<wstring, 2>>& vROI, string strSlope)
{
	ASSERT(vROI.size() == 2); // hor & ver

	m_strSlope = strSlope;

	vector<vector<array<string, 2>>> vCache;

	vCache.push_back({ { Wstring2String(vROI[0][0]), Wstring2String(vROI[0][1]) } });

	vCache.push_back({ array<string, 2>{ "", "" } });

	vCache.push_back({ { Wstring2String(vROI[1][0]), Wstring2String(vROI[1][1]) } });

	SetFiles(vCache);
}

inline double CThSlope::Distance(Vec4f& vLine)
{
	return sqrt(pow(vLine[2] - vLine[0], 2) + pow(vLine[3] - vLine[1], 2));
}

inline void CThSlope::Image2Cartesian(Vec4f& vLine, int h)
{
	vLine[1] = h - vLine[1]; vLine[3] = h - vLine[3];
}

template<typename  T>
inline void CThSlope::Image2Cartesian(T& pt, int h)
{
	pt.y = h - pt.y;
}

void CThSlope::PreVertical(Mat& cROI, double& k)
{
	Rect rtFeature(0, 0, 10000, cROI.rows * 0.5);

	Mat cFeature = cROI(rtFeature).clone();

	vector<Vec4f> vLine;

	HoughLinesP(cFeature, vLine, 1, 0.003 * CV_PI / 180.0, 50, 1000, 3000);

	vector<tuple<Vec4f, double>> vDist(vLine.size());

	transform(vLine.begin(), vLine.end(), vDist.begin(), [cFeature, this](Vec4f& vLine)
		{ Image2Cartesian(vLine, cFeature.rows); return make_tuple(vLine, Distance(vLine)); });

	sort(vDist.begin(), vDist.end(), [](tuple<Vec4f, double>& i1, tuple<Vec4f, double>& i2) { return get<1>(i2) < get<1>(i1); });

	auto iFeature = vDist.begin();

	auto iLine = get<0>(*iFeature);

	double x = iLine[2] - iLine[0];

	double y = iLine[3] - iLine[1];

	k = y / x;// 问题点：会不会出现 x = c ？
}

void CThSlope::PreHorizontal(Mat& cROI, double& k)
{
	cout << __FUNCTION__ << endl;

	Rect rtFeature(0, 0, cROI.cols * 0.5, 10000);

	Mat cFeature = cROI(rtFeature).clone();

	vector<Vec4f> vLine;

	HoughLinesP(cFeature, vLine, 1, 0.003 * CV_PI / 180.0, 50, 1000, 3000);

	vector<tuple<Vec4f, double>> vDist(vLine.size());

	transform(vLine.begin(), vLine.end(), vDist.begin(), [cFeature, this](Vec4f& vLine)
		{ Image2Cartesian(vLine, cFeature.rows); return make_tuple(vLine, Distance(vLine)); });

	sort(vDist.begin(), vDist.end(), [](tuple<Vec4f, double>& i1, tuple<Vec4f, double>& i2) { return get<1>(i2) < get<1>(i1); });

	auto iFeature = vDist.begin();

	auto iLine = get<0>(*iFeature);

	double x = iLine[2] - iLine[0];

	double y = iLine[3] - iLine[1];

	k = y / x;// 问题点：会不会出现 y = c ？

	cout << __FUNCTION__ << endl;
}

void CThSlope::SetOfPoints(Mat& cROI, vector<Point2f>& vPoint)
{
	for (uint64 y = 0; y < cROI.rows; ++y)
	{
		for (uint64 x = 0; x < cROI.cols; ++x)
		{
			uint64 idx = y * cROI.cols + x;

			if (!cROI.data[idx]) continue;

			vPoint.push_back(Point2f(x, y));
		}
	}
}

void CThSlope::Vertical(Mat& cROI, double& k)
{
	map<int64, vector<Point2f> > vPoints;

	vector<Point2f> vPoint;

	SetOfPoints(cROI, vPoint); // 参与计算的点集

	double dCenter = 0.5 * cROI.rows;

	for (int i = 0; i < vPoint.size(); ++i)
	{
		auto& pt = vPoint[i];

		Image2Cartesian(pt, cROI.rows);

		double b = pt.y - k * pt.x;

		int64 idx = round((dCenter - b) / k * 0.01);

		vPoints[idx].push_back(pt);
	} // 线段分离

	k = 0.0; int nK = 0; vector<Point2f> vkLine;

	for (auto& iLine : vPoints) // vPoints = 卡迪尔坐标系
	{
		if (iLine.second.size() < cROI.rows * 0.125) continue;

		Vec4f vLine;

		fitLine(iLine.second, vLine, DIST_L1, 0, 0.0001, 0.0001);

		double ki = vLine[1] / vLine[0]; k += ki; ++nK;

		vkLine = (vkLine.empty() ? iLine.second : vkLine);
	}
	k /= nK;

	{
		// vkLine = 卡迪尔坐标系

		sort(vkLine.begin(), vkLine.end(), [](Point2f i1, Point2f i2) {return i1.y < i2.y; });

		auto iPt = vkLine[vkLine.size() - 1];

		double b = iPt.y - k * iPt.x;

		Point pt1(0, 0), pt2(0, cROI.rows);

		pt1.x = (pt1.y - b) / k;  pt1.y = cROI.rows - pt1.y; // h

		pt2.x = (pt2.y - b) / k;  pt2.y = cROI.rows - pt2.y; // 0

		{
			pt1.y = cROI.rows - pt1.y;
			pt2.y = cROI.rows - pt2.y;
			pt1.x -= 100; // 卡迪尔坐标系 （11->100）
			k = (pt1.y - pt2.y)/(pt1.x  - pt2.x);
			b = pt1.y - pt1.x * k;

			pt1.y = cROI.rows - pt1.y;
			pt2.y = cROI.rows - pt2.y; // 图像坐标系
		}
	
		cout << "ver: " << pt1.x << ", " << pt1.y << " -> " << pt2.x << ", " << pt2.y << " | " << iPt.x << ", " << (cROI.rows - iPt.y) << endl;

		line(cROI, pt1, pt2, Scalar(0xf0));

		imwrite("E:\\1.0\\verEx.png", cROI);
	}
}

void CThSlope::Horizontal(Mat& cROI, double& k)
{
	cout << __FUNCTION__ << endl;

	map<int64, vector<Point2f> > vPoints;

	vector<Point2f> vPoint;

	SetOfPoints(cROI, vPoint);

	double dCenter = 0.5 * cROI.cols;

	for (int i = 0; i < vPoint.size(); ++i)
	{
		auto& pt = vPoint[i];

		Image2Cartesian(pt, cROI.rows);

		double b = pt.y - k * pt.x;

		int64 idx = round((dCenter * k + b) * 0.01);

		vPoints[idx].push_back(pt);
	} // 线段分离

	k = 0.0; int nK = 0; vector<Point2f> vkLine; double dSum[2] = { 0.0, 0.0 };

	for (auto& iLine : vPoints) // vPoints = 卡迪尔坐标系
	{
		if (iLine.second.size() < cROI.cols * 0.125) continue;

		Vec4f vLine;

		fitLine(iLine.second, vLine, DIST_L1, 0, 0.0001, 0.0001);

		double ki = vLine[1] / vLine[0]; k += ki; ++nK;

		dSum[0] = 0.0;

		for (auto& ipt : iLine.second) dSum[0] += ipt.y;

		if (dSum[1] < dSum[0])
		{
			dSum[1] = dSum[0];

			vkLine = iLine.second;
		}
//		vkLine = (vkLine.empty() ? iLine.second : vkLine);
	}
	k /= nK;

	{
		// vkLine = 卡迪尔坐标系
		cout << "vLine.size: " << vkLine.size() << endl;

		sort(vkLine.begin(), vkLine.end(), [](Point2f i1, Point2f i2) {return i1.x < i2.x; });

		auto iPt = vkLine[0];

		double b = iPt.y - k * iPt.x;

		Point2d pt1(0, 0), pt2(cROI.cols, 0);

		pt1.y = k * pt1.x + b; pt1.y = cROI.rows - pt1.y;

		pt2.y = k * pt2.x + b; pt2.y = cROI.rows - pt2.y;

		{
			pt1.y = cROI.rows - pt1.y;
			pt2.y = cROI.rows - pt2.y;
			pt2.y += 40; // 卡迪尔坐标系
			k = (pt1.y - pt2.y) / (pt1.x - pt2.x);
			b = pt1.y - pt1.x * k;
			pt1.y = cROI.rows - pt1.y;
			pt2.y = cROI.rows - pt2.y; // 图像坐标系
		}

		cout << "hor: " << pt1.x << ", " << pt1.y << " -> " << pt2.x << ", " << pt2.y << " | " << iPt.x << ", " << (cROI.rows - iPt.y) << endl;

		line(cROI, pt1, pt2, Scalar(0xf0));

		imwrite("E:\\1.0\\horEx.png", cROI);
	}

	cout << __FUNCTION__ << endl;
}

bool CThSlope::IsIdle()
{
	int nSize = 0; static int g_nTh = 1;
	{
		CSRLock lock(&m_lLock);

		nSize = m_vImCache.size();
	}
	return nSize < g_nTh;
}

void CThSlope::OnImCache(lpIm2Mat& lpCache)
{
	Push(lpCache);

	m_cImCache.signal();
}

void CThSlope::Push(lpIm2Mat& lpCache)
{
	CSWLock lock(&m_lLock);

	m_vImCache.push_back(lpCache);
}

bool CThSlope::Pop(lpIm2Mat& lpCache)
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

void CThSlope::Clear()
{
	CSWLock lock(&m_lLock);

	m_vImCache.clear();
}

void CThSlope::OnInstallSlopeParam(array<double, 2>& vK, string& strSlope)
{
	Mat cSlope(1, sizeof(double) * 2, CV_8UC1);

	memcpy(cSlope.data, vK.data(), cSlope.cols);

	imwrite(strSlope.c_str(), cSlope);
}

void CThSlope::Slope()
{
	lpIm2Mat lpCache = nullptr;

	if (!Pop(lpCache)) { m_cImCache.wait(); return; }

	cout << __FUNCTION__ << ": " << lpCache->strFirst.c_str() << " | " << lpCache->nRemaining << endl;

	ASSERT(lpCache->vIm2Mat.size() == 1);

	auto& lpImage = lpCache->vIm2Mat[0];

	switch (lpCache->nRemaining)
	{
	case 2: // hor
		PreHorizontal(*lpImage, m_vK[0]);
		Horizontal(*lpImage, m_vK[0]);
		break;

	case 0: // ver
		PreVertical(*lpImage, m_vK[1]);
		Vertical(*lpImage, m_vK[1]);
		OnInstallSlopeParam(m_vK, m_strSlope);
	//	OnSlopeEnd(m_vK[0], m_vK[1]);
		break;

	default: // empty
		break;
	}
	delete lpCache;
}
