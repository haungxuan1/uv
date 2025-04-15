#pragma once
#include "ThMerging.h"

class CThROI : public CThMerging
{
protected:
	CThROI();
	virtual ~CThROI();

protected:
	virtual bool Reg();
	virtual void Unreg();

protected:
	virtual void OnROI(ImPtr::CImPtr cImPtr, array<Point2f, 4> vVertex) = 0;

protected:
	void SetROIParam(vector<array<wstring, 2>>& vMerged, array<int, 3> vKernel, string strCacheDir);

private:
	bool IsIdle();
	void OnImCache(lpIm2Mat& lpCache);
	void Push(lpIm2Mat& lpCache);
	bool Pop(lpIm2Mat& lpCache);
	void Clear();

private:
	void Mask(Mat& cMask, Mat& cImMat);
	void Connected(Mat& cIm, vector<Rect>& vCon);
	void Hor2Hor(Rect& rtCol);
	void Ver2Ver(Rect& rtRow);

	void Ver(Mat& vMask, Rect& vTBL, Rect& vTBR);
	void Ver(Mat*& cIm, Mat*& cMask, Rect& rtRow, array<Point2f, 2>& vRow);

	void Hor(Mat& hMask, Rect& vLRT, Rect& vLRB);
	void Hor(Mat*& cIm, Mat*& cMask, Rect& rtCol, array<Point2f, 2>& vCol);

	void Vertex(Mat& cIm, array<array<Point2f, 2>, 2>& vCol, array<array<Point2f, 2>, 2>& vRow, array<Point2f, 4>& vVertex);

private:
	void ROI();

private:
	string m_strCacheDir;
	array<int, 3> m_vKernel;	// th, erode, dilate
	array<Point2f, 4> m_vVertex; // «„–±4∂•µ„

private:
	CSRWLockBase m_lLock;
	CCondition m_cImCache;
	vector<lpIm2Mat> m_vImCache;
};


