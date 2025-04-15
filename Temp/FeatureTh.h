#pragma once
#include "SRWLock.h"
#include "Condition.h"
#include "ImPtr.h"
#include <tuple>
#include <vector>
using namespace cv;
using namespace std;

class CFeatureTh
{
protected:
	CFeatureTh(CWnd* lpInitiator);
	virtual ~CFeatureTh();

protected:
	void Reg();
	void Unreg();

protected:
	void InitParam(array<int, 4> vFill, array<double, 9> M, array<float, 2> vWH2, array<double, 2> vDPI2);
	void OnCameraFrame(ImPtr::CImPtr*& lpFrame);

private:
	static unsigned int __stdcall OnFeaExitThread(void* lpUesrParam);
	static unsigned int __stdcall OnFeatureThread(void* lpUesrParam);

private:
	void SetTermination(bool bTermination);
	bool IsTermination();

private:
	void OnFeature(ImPtr::CImPtr*& lpFrame);

private:
	void Push(ImPtr::CImPtr*& lpFrame);
	bool Pop(ImPtr::CImPtr*& lpFrame);

private:
	CWnd* m_lpInitiator;
	array<int, 4> m_vFill;		// 变换前填充系数
	Mat m_cHomography;			// 变换矩阵M
	array<float, 2> m_vWH2;		// 纠正后图像大小
	array<double, 2> m_vDPI2;	// 相机dpi 
	vector<ImPtr::CImPtr*> m_vFrame;

private:
	bool m_bTermination;
	CSRWLockBase m_lLock;
	CCondition m_cFeature;
	CCondition m_cTermination;
};
