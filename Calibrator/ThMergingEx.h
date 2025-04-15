#pragma once
#include "ThHomography.h"

class CThMergingEx : public CThHomography
{
protected:
	CThMergingEx();
	virtual ~CThMergingEx();

protected:
	virtual bool Reg();
	virtual void Unreg();

protected:
	virtual void OnMergingEx(ImPtr::CImPtr cImPtr) = 0;
	virtual void OnMergeEx(array<double, 2>& vAlign, ImPtr::CImPtr cImPtr) = 0;

protected:
	void SetMergingExParam(vector<array<wstring, 2>>& vFile, 
		int nFrameInLine, 
		int nPassInStride, 
		array<int, 4> vReference,
		array<float, 2> vImWH1,
		array<float, 2> vImWH2,
		array<int, 2> vImWH2Ex,
		array<Point2f, 4> vVertex1,
		array<Point2f, 4> vVertex2,
		array<int, 4> vFill,
		array<double, 9> vM,
		string strAligned, 
		string strMerged,
		string strROIMerged);

private:
	void OnImMerging();

private:
	void Merging(array<int, 2> vTL, vector<Mat*>& vLine, Mat*& lpMerging);
	void OnInstallAlignedParam(int nImWidth, int nImHeight, int nFrameInLine, string& strAligned, int& nPassInStride, vector<array<int, 2>>& vImAligned);
	Point2f Target(Point2f ptSrc, Mat& M);
	void Mixed(vector<Mat*>& vLine);
	void Mixed(Mat& m1, Mat& m2);
	void Line(vector<Mat*>& vLine, Mat& cLine);

private:
	bool IsIdle();
	void OnImCache(lpIm2Mat& lpCache);
	void Push(lpIm2Mat& lpCache);
	bool Pop(lpIm2Mat& lpCache);
	void Pop(array<int, 2>& vTL);
	void Clear();

private: // 透视变换参数
	Mat m_cM;
	array<float, 2> m_vWH1;
	array<float, 2> m_vWH2;
	array<int, 2> m_vWH2Ex;
	array<int, 4> m_vFill; //l,t,r,b
	array<Point2f, 4> m_vVertex;
	array<Point2f, 4> m_vVertex2;
	array<int, 4> m_vReference; // 参照物位置

private:
	int m_nOx; // 对齐补偿
	int m_nOy; // 透视变换
	Point m_ptTL;
	Mat* m_lpImMerging;
	string m_strMerged;
	string m_strROIMerged;
	vector<array<int, 2>> m_vImAligned;
	array<double, 2> m_vImAlign;

private:
	CSRWLockBase m_lLock;
	CCondition m_cImCache;
	vector<lpIm2Mat> m_vImCache;
};
