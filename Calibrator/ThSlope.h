#pragma once
#include "ThROI.h"
#include <map>

class CThSlope : public CThROI
{
protected:
	CThSlope();
	virtual ~CThSlope();

protected:
	virtual bool Reg();
	virtual void Unreg();

protected:
	//virtual void OnSlopeEnd(double dHor, double dVer) = 0;

protected:
	void SetSlopeParam(vector<array<wstring, 2>>& vROI, string strSlope);

private:
	bool IsIdle();
	void OnImCache(lpIm2Mat& lpCache);
	void Push(lpIm2Mat& lpCache);
	bool Pop(lpIm2Mat& lpCache);
	void Clear();

private:
	void PreVertical(Mat& cROI, double& k);
	void PreHorizontal(Mat& cROI, double& k);
	void Vertical(Mat& cROI, double& k);
	void Horizontal(Mat& cROI, double& k);
	void SetOfPoints(Mat& cROI, vector<Point2f>& vPoint);
	void Image2Cartesian(Vec4f& vLine, int h);
	template<typename  T>
	void Image2Cartesian(T& pt, int h);
	double Distance(Vec4f &vLine);
	
	void OnInstallSlopeParam(array<double, 2>& vK, string& strSlope);

private:
	void Slope();

private:
	string m_strSlope;
	array<double, 2> m_vK;
	vector<lpIm2Mat> m_vImCache;
	CCondition m_cImCache;

private:
	CSRWLockBase m_lLock;
};