#pragma once
#include "SRWLock.h"
#include "Condition.h"
#include "opencv.hpp"
#include <vector>
#include <tuple>

/*本地静态测试，多线程加载png*/
class CImThread
{
protected:
	CImThread();
	virtual ~CImThread();

protected:
	void Run();

private:
	static unsigned int __stdcall OnReadImage(void* lpUesrParam);
	static unsigned int __stdcall OnScheduling(void* lpUesrParam);

private:
	bool GetImIndex(int &nIndex);
	void SetImMatrix(int &nIndex, cv::Mat*& lpImMatrix);
	bool GetImMatrix(int& nIndex, cv::Mat*& lpImMatrix);

private:
	CCondition m_cScheduling;
	CSRWLockBase m_cLock;
	std::vector<int> m_vImIndex;
	std::vector<std::tuple<int, cv::Mat *>> m_vImMatrixs;
};