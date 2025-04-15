#pragma once
#include <string>
#include <vector>
#include <functional>
#include "opencv.hpp"
#include "SRWLock.h"
#include "Condition.h"
#include "Transform.h"

using namespace cv;
using namespace std;
using namespace Transform;

class CIm2Mat
{
protected:
	struct tagIm2Mat
	{
		int nRemaining;
		string strFirst;
		vector<Mat*> vIm2Mat;
		tagIm2Mat():nRemaining(0){}
		~tagIm2Mat() { for (auto& iIm2Mat : vIm2Mat) { if (iIm2Mat) { delete iIm2Mat; iIm2Mat = nullptr; } } }
	};
	typedef tagIm2Mat* lpIm2Mat;

protected:
	CIm2Mat();
	virtual ~CIm2Mat();

protected:
	virtual bool Reg();
	virtual void Unreg();

protected:
	void SetFiles(vector<vector<array<string,2>>>& vFile);
	void SetSignal(function<void(lpIm2Mat&)> fSignal, function<bool()> fIsIdle);

private:
	static unsigned int __stdcall OnExitThread(void* lpUesrParam);
	static unsigned int __stdcall OnReadThread(void* lpUesrParam);

private:
	void Termination();
	bool IsTermination();

private:
	bool PopFile(vector<array<string,2>>& vFile, int& nRemaining);
	void Im2Mat(vector<array<string, 2>>& vFile, tagIm2Mat& vIm2Mat);
	void Clear();

private:
	function<bool()> m_fIsIdle;
	function<void(lpIm2Mat&)> m_fSignal;
	vector<vector<array<string,2>>> m_vFile;

private:
	bool m_bThread;
	bool m_bTermination;
	CSRWLockBase m_lLock;
	CCondition m_cTermination;
};