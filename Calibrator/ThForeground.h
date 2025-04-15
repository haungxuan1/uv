#pragma once
#include <array>
#include "ImPadding.h"

using namespace cv;
using namespace std;

class CThForeground : public CImPadding
{
protected:
	CThForeground();
	virtual ~CThForeground();

protected:
	virtual bool Reg();
	virtual void Unreg();

protected:
	virtual void OnHistogram(int nRemaining, ImPtr::CImPtr& cImage, ImPtr::CImPtr& cHistogram) = 0;
	virtual void OnForeground(int nForeground) = 0;


protected:
	void SetForegroundParam(vector<array<wstring, 2>>& vFile, int nFrameInLine, int nBlock);

private:
	void OnThForeground();

private:
	bool IsIdle();
	void OnImCache(lpIm2Mat& lpCache);
	void Push(lpIm2Mat& lpCache);
	bool Pop(lpIm2Mat& lpCache);
	void Clear();

private:
	int m_nBlock;
	CCondition m_cImCache;
	vector<lpIm2Mat> m_vImCache;
	vector<int> m_vTh;

private:
	CSRWLockBase m_lLock;
};