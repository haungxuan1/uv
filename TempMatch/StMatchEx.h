#pragma once
#include "StMatch.h"
#include "Condition.h"
#include "SRWLock.h"
#include <tuple>
#include <vector>

class CStMatchEx : public CStMatch
{
	DECLARE_DYNAMIC(CStMatchEx)

private:
	typedef std::tuple<int, int, int, int, cv::Mat*, cv::Mat*, cv::Mat*, cv::Mat*> Task;
	typedef std::vector<Task> Tasks;

public:
	CStMatchEx();
	virtual ~CStMatchEx();

public:
	void InitCanvas(CWnd* lpWnd, CRect rtCanvas);

private:
	virtual void OnTask(
		int ox, int oy, int ox2, int oy2,
		cv::Mat* lpMoldTemp, cv::Mat* lpMoldView, 
		cv::Mat* lpTempTemp, cv::Mat* lpTempView);

private:
	static unsigned int __stdcall MatchThread(void* lpParam);
	static unsigned int __stdcall TermiThread(void* lpParam);
	void OnMathcing();
	void OnTermination();
	bool IsTermination();
	void PushBack(
		int ox, int oy, int ox2, int oy2,
		cv::Mat* lpMoldTemp, cv::Mat* lpMoldView, 
		cv::Mat* lpTempTemp, cv::Mat* lpTempView);
	bool PopFront(
		int &ox, int &oy, int& ox2, int& oy2,
		cv::Mat*& lpMoldTemp, cv::Mat*& lpMoldView, 
		cv::Mat*& lpTempTemp, cv::Mat*& lpTempView);
	void OnSyncTask(
		int& ox, int& oy, int& ox2, int& oy2,
		cv::Mat* lpMoldTemp, cv::Mat* lpMoldView, 
		cv::Mat* lpTempTemp, cv::Mat* lpTempView, double dSame);

private:
	bool m_bTermination;
	CSRWLockBase m_lLock;
	CCondition m_cMatch;
	CCondition m_cTermination;
	Tasks m_vTask;

private:

	DECLARE_MESSAGE_MAP()
};

