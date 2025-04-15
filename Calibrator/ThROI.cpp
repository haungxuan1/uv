#include "pch.h"
#include "ThROI.h"
#include "Transform.h"
#include "Helper.h"

CThROI::CThROI()
{

}

CThROI::~CThROI()
{

}

bool CThROI::Reg()
{
	if (m_uWorkType == eWT_ROI)
	{
		CIm2Mat::SetSignal(bind(&CThROI::OnImCache, this, placeholders::_1), bind(&CThROI::IsIdle, this));

		CImPadding::SetThParam(&m_cImCache, bind(&CThROI::ROI, this));
	}

	return CThMerging::Reg();
}

void CThROI::Unreg()
{
	CThMerging::Unreg();

	Clear();
}

void CThROI::SetROIParam(vector<array<wstring, 2>>& vMerged, array<int, 3> vKernel, string strCacheDir)
{
	ASSERT(vMerged.size() == 1);

	m_strCacheDir = strCacheDir;
		
	m_vKernel = vKernel;

	vector<vector<array<string, 2>>> vCache;

	for (auto& iMerged : vMerged) vCache.push_back({ {Wstring2String(iMerged[0]), Wstring2String(iMerged[1])} });

	SetFiles(vCache);
}

bool CThROI::IsIdle()
{
	int nSize = 0; static int g_nTh = 1;
	{
		CSRLock lock(&m_lLock);

		nSize = m_vImCache.size();
	}
	return nSize < g_nTh;
}

void CThROI::OnImCache(lpIm2Mat& lpCache)
{
	Push(lpCache);

	m_cImCache.signal();
}

void CThROI::Push(lpIm2Mat& lpCache)
{
	CSWLock lock(&m_lLock);

	m_vImCache.push_back(lpCache);
}

bool CThROI::Pop(lpIm2Mat& lpCache)
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

void CThROI::Clear()
{
	CSWLock lock(&m_lLock);

	m_vImCache.clear();
}

void CThROI::Mask(Mat& cMask, Mat& cImMat)
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

void CThROI::Connected(Mat& cIm, vector<Rect>& vCon)
{
	Mat labels = Mat::zeros(cIm.rows, cIm.cols, CV_32S), stats, centroids;

	connectedComponentsWithStats(cIm, labels, stats, centroids);

	auto lpCon = (int*)stats.data; 

	for (int y = 1; y < stats.rows; ++y) 
		vCon.push_back({ lpCon[y * 5 + CC_STAT_LEFT], lpCon[y * 5 + CC_STAT_TOP], lpCon[y * 5 + CC_STAT_WIDTH], lpCon[y * 5 + CC_STAT_HEIGHT] });
}

void CThROI::Ver(Mat& cTmp, Rect& vTBL, Rect& vTBR)
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

void CThROI::Ver(Mat*& cIm, Mat*& cMask, Rect& rtRow, array<Point2f,2>& vRow)
{
	Mat& iRow = *cIm, iThRow;

	Mat* lpRow = &iRow, * lpThRow = &iThRow;

	cuThinging(lpRow, lpThRow);

	Mask(*cMask, iThRow); delete cIm, delete cMask;

	vector<Vec4f> vLine;

	HoughLinesP(iThRow, vLine, 1, 0.003 * CV_PI / 180.0, 300, 500, iThRow.rows);

	sort(vLine.begin(), vLine.end(), 
		[](Vec4f& v1, Vec4f& v2) {return sqrt(pow(v1[0] - v1[2], 2) + pow(v1[1] - v1[3], 2)) < sqrt(pow(v2[0] - v2[2], 2) + pow(v2[1] - v2[3], 2)); });

	auto iEnd = vLine[vLine.size() - 1];

	vRow[0] = { iEnd[0] + rtRow.x, iEnd[1] + rtRow.y }; vRow[1] = { iEnd[2] + rtRow.x, iEnd[3] + rtRow.y };
}
/****************************************************************************/
void CThROI::Hor(Mat& cTmp, Rect& vLRT, Rect& vLRB)
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

void CThROI::Hor(Mat*& cIm, Mat*& cMask, Rect& rtCol, array<Point2f, 2>& vCol)
{
	Mat& iRow = *cIm, iThRow;

	Mat* lpRow = &iRow, *lpThRow = &iThRow;

	cuThinging(lpRow, lpThRow);

	Mask(*cMask, iThRow); delete cIm, delete cMask;

	vector<Vec4f> vLine;

	HoughLinesP(iThRow, vLine, 1, 0.003 * CV_PI / 180.0, 300, 500, iThRow.rows);

	sort(vLine.begin(), vLine.end(), [](Vec4f& v1, Vec4f& v2) {return sqrt(pow(v1[0] - v1[2], 2) + pow(v1[1] - v1[3], 2)) < sqrt(pow(v2[0] - v2[2], 2) + pow(v2[1] - v2[3], 2)); });

	auto iEnd = vLine[vLine.size() - 1];

	vCol[0] = { iEnd[0] + rtCol.x, iEnd[1] + rtCol.y }; vCol[1] = { iEnd[2] + rtCol.x, iEnd[3] + rtCol.y };
}

void CThROI::Hor2Hor(Rect& rtCol)
{
	rtCol.width = (rtCol.width + 2) * 10;

	rtCol.height = (rtCol.height + 2) * 10;

	rtCol.x = (rtCol.x - 1) * 10;

	rtCol.y = (rtCol.y - 1) * 10;
}

void CThROI::Ver2Ver(Rect& rtRow)
{
	rtRow.width = (rtRow.width + 2) * 10;

	rtRow.height = (rtRow.height + 2) * 10;

	rtRow.x = (rtRow.x - 1) * 10;

	rtRow.y = (rtRow.y - 1) * 10;
}

void CThROI::Vertex(Mat& cIm, array<array<Point2f, 2>, 2>& vCol, array<array<Point2f, 2>, 2>& vRow, array<Point2f, 4>& vVertex)
{
	auto param = [&cIm](array<Point2f, 2>& vP)->array<double, 2>
	{
		double x1= vP[0].x, x2 = vP[1].x;
		double y1 = (double)cIm.rows - vP[0].y, y2 = (double)cIm.rows - vP[1].y;
		double k = (y2 - y1) / (x2 - x1);
		double b = y1 - k * x1;
		return { k, b };
	};
	auto point = [&cIm](array<double, 2>&p1, array<double, 2>&p2)->Point2f
	{
		double k1 = p1[0], k2 = p2[0], b1 = p1[1], b2 = p2[1];
		float x = (b2 - b1) / (k1 - k2);
		float y = cIm.rows - (k1 * x + b1);
		return { x, y };
	};
	auto vLRT = vCol[0], vLRB = vCol[1], vTBL = vRow[0], vTBR = vRow[1];

	array<double, 2> pLRT = param(vLRT), pLRB = param(vLRB), pTBL = param(vTBL), pTBR = param(vTBR);

	Point2f ptTL = point(pLRT, pTBL), ptTR = point(pLRT, pTBR), ptBL = point(pLRB, pTBL), ptBR = point(pLRB, pTBR);

	vVertex = { ptTL, ptTR, ptBR, ptBL };
}

void CThROI::ROI()
{
	lpIm2Mat lpCache = nullptr;

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

	Mat* vMask = new Mat, *hMask = new Mat, *vMask2 = new Mat, *hMask2 = new Mat, * cMask = new Mat;

	erode(iImMat, *vMask, vErode); 

	erode(iImMat, *hMask, hErode);

	dilate(*vMask, *vMask2, vDilate);

	dilate(*hMask, *hMask2, hDilate);

	addWeighted(*vMask2, 0.5, *hMask2, 0.5, 0.0, *cMask); delete vMask2; delete hMask2;

	threshold(*cMask, *cMask, 200, 255, THRESH_BINARY);

	Mat cDilate = Mat::ones(100, 100, CV_8UC1);

	dilate(*cMask, *cMask, cDilate);

	Rect vLRT, vLRB, vTBL, vTBR;

	Hor(*hMask, vLRT, vLRB); 

	Hor2Hor(vLRT); Hor2Hor(vLRB);

	Mat* cLRT = new Mat, * cLRB = new Mat, * cLRTMask = new Mat, * cLRBMask = new Mat;

	*cLRT = (*hMask)(vLRT).clone();

	*cLRB = (*hMask)(vLRB).clone(); delete hMask;

	*cLRTMask = (*cMask)(vLRT).clone();

	*cLRBMask = (*cMask)(vLRB).clone();

	Ver(*vMask, vTBL, vTBR); 

	Ver2Ver(vTBL); Ver2Ver(vTBR); 

	vTBL.height = min(vTBL.height, vMask->rows - vTBL.y);

	vTBR.height = min(vTBR.height, vMask->rows - vTBR.y); // 缩放倍数引发的临时补丁

	Mat* cTBL = new Mat, * cTBR = new Mat, * cTBLMask = new Mat, * cTBRMask = new Mat;

	*cTBL = (*vMask)(vTBL).clone();

	*cTBR = (*vMask)(vTBR).clone(); delete vMask;

	*cTBLMask = (*cMask)(vTBL).clone();

	*cTBRMask = (*cMask)(vTBR).clone(); delete cMask;

	imwrite(m_strCacheDir + "\\cache.png", iImMat); delete lpCache->vIm2Mat[0];

	Ver(cTBL, cTBLMask, vTBL, vRow[0]);

	Ver(cTBR, cTBRMask, vTBR, vRow[1]); 

	Hor(cLRT, cLRTMask, vLRT, vCol[0]);

	Hor(cLRB, cLRBMask, vLRB, vCol[1]);

	lpCache->vIm2Mat[0] = new Mat; auto& iImMat2 = *lpCache->vIm2Mat[0]; iImMat2 = imread(m_strCacheDir + "\\cache.png", 0);

	Vertex(iImMat2, vCol, vRow, m_vVertex); 

	line(iImMat2, m_vVertex[0], m_vVertex[1], Scalar(0xf0),10);

	line(iImMat2, m_vVertex[1], m_vVertex[2], Scalar(0xf0),10);

	line(iImMat2, m_vVertex[2], m_vVertex[3], Scalar(0xf0),10);

	line(iImMat2, m_vVertex[3], m_vVertex[0], Scalar(0xf0),10);

	ImPtr::CImPtr cImPtr(new Mat);

	resize(iImMat2, *cImPtr.Get(), Size(0.1 * iImMat2.cols, 0.1 * iImMat2.rows));

	OnROI(cImPtr, m_vVertex);

	cout << "c: " << vCol[0][0] << " | " << vCol[0][1] << " | " << vCol[1][0] << " | " << vCol[1][1] << endl;

	cout << "r: " << vRow[0][0] << " | " << vRow[0][1] << " | " << vRow[1][0] << " | " << vRow[1][1] << endl;

	cout << "v: " << m_vVertex[0] << " | " << m_vVertex[1] << " | " << m_vVertex[2] << " | " << m_vVertex[3] << endl;

	imwrite(m_strCacheDir + "\\ROI.png", iImMat2);

	delete lpCache;
}
