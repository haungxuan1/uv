#pragma once
#include "CImCtrl.h"
#include "ImCenter.h"
#include "Condition.h"
#include <tuple>

using namespace cv;
using namespace std;

namespace ImView2 { class CImView; }

namespace ImRender
{
	class CImRender : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CImRender)

	private:
		friend class ImView2::CImView;

	protected:
		CImRender();
		virtual ~CImRender();

	protected:
		void SetCenter(ImCenter::CImCenter* lpImCenter) { m_lpImCenter = lpImCenter; }
	
	protected:
		void OnSyncImThumb(tuple<int, double, int, double> vImThumb);
		void GetRender(Mat& cImRender);

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		virtual void OnDestroyEx();

	private:
		static unsigned int __stdcall OnThExit(void* lpUesrParam);
		static unsigned int __stdcall OnThRender(void* lpUesrParam);

	private:
		void OnRender();
		void OnRender(long long& nSign, tuple<int, double, int, double>& vImThumb, Mat& cImRender);
		void OnRender(ImLayer::tagLayer& tParam, tuple<int, double, int, double>& vImThumb, Mat& cImRender);
		void OnRender(Mat& cImRender, Mat& cImCache);

	private:
		void Termination();
		bool IsTermination();
		long long Timer();
		bool IsTimeOut();
		void ResetTimer(int nTimer, long long nTime);
		void SetImThumb(tuple<int, double, int, double>& vImThumb);
		void GetImThumb(tuple<int, double, int, double>& vImThumb);

	private:
		Mat m_cImRender;	// ‰÷»æÀ˘”√ƒ⁄¥Ê
		Mat m_cImCache;		// ªÊÕºª∫¥Ê
		ImCenter::CImCenter* m_lpImCenter;
		tuple<int, double, int, double> m_vImThumb;

	private:
		long long m_nTimer[2];
		bool m_bTermination;
		CSRWLockBase m_lLock;
		CCondition m_cRender;
		CCondition m_cTermination;

	private:
		DECLARE_MESSAGE_MAP()
	};
}
