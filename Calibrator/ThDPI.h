#pragma once
#include "ThMergingEx.h"
#include <map>

using namespace cv;
using namespace std;

class CThDPI : public CThMergingEx
{
protected:
	CThDPI();
	virtual ~CThDPI();

protected:
	virtual bool Reg();
	virtual void Unreg();

protected:
	virtual void OnDPI(double hor, double ver) = 0;

protected:
	void SetDPIParam(vector<array<wstring, 2>>& vROI, array<int, 3> vKernel, string strDPI, string strCacheDir);

private:
	bool IsIdle();
	void OnImCache(lpIm2Mat& lpCache);
	void Push(lpIm2Mat& lpCache);
	bool Pop(lpIm2Mat& lpCache);
	void Clear();

private:
	void Mask(Mat& cMask, Mat& cImMat);
	void Connected(Mat& cIm, vector<Rect>& vCon);
	void Hor(Mat& cTmp, Rect& vLRT, Rect& vLRB);
	void Hor2Hor(Rect& rtCol);
	void Ver2Ver(Rect& rtRow);
	void Ver(Mat& vMask, Rect& vTBL, Rect& vTBR);
	void Hor(Mat*& cIm, Mat*& cMask, Rect& rtCol, double& c);
	void Ver(Mat*& cIm, Mat*& cMask, Rect& rtRow, double& c);
	void Vertex(array<double, 4>& c, array<Point, 4>& v);
	void DrawVertex(array<Point, 4>& v, Mat& cImage, int& nRemaining);
	void Scale(array<Point, 4>& v1, array<Point, 4>& v2, double& sx, double& sy);
	void Combine(array<Point, 4>& v1, array<Point, 4>& v2, double& sx, double& sy);
	
private:
	void InstallDPIParam(string strDpi, array<double, 2> vDPI);

private:
	void DPI();

private:
	string m_strDPI;
	string m_strCacheDir;
	array<int, 3> m_vKernel;	// th, erode, dilate
	array<Point, 4> m_ptVertex[2];

private:
	CSRWLockBase m_lLock;
	CCondition m_cImCache;
	vector<lpIm2Mat> m_vImCache;
};