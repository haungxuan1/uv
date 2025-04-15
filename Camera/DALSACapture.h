#pragma once
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include "ImPtr.h"
#include "SRWLock.h"
#include "Condition.h"
#include "SapClassBasic.h"
using namespace cv;
using namespace std;

namespace DALSACamera
{
	class CCapture
	{
	protected:
		CCapture();
		virtual ~CCapture();

	protected:
		virtual void Reg(HWND& lpHandle);
		virtual void Unreg(HWND& lpHandle);

	private:
		void Push(ImPtr::CImPtr& cFrame);
		bool Pop(ImPtr::CImPtr& cFrame);

	private:
		void Termination();
		bool IsTermination();

	protected:
		void SetInitiator(HWND& lpHandle);
		void GetInitiator(HWND& lpHandle);

	protected:
		static void OnAcquisition(SapXferCallbackInfo* pInfo);

	private:
		static unsigned int __stdcall OnCapture(void* lpParam);
		static unsigned int __stdcall OnTermination(void* lpParam);

	private:
		virtual void OnCapture(ImPtr::CImPtr& cFrame, HWND& lpHandle) = 0;

	private:
		CCondition m_cCapture;
		vector<ImPtr::CImPtr> m_vFrame;
		HWND m_lpInitiator;

	private:
		bool m_bTermination;
		CSRWLockBase m_lLock;
		CCondition m_cTermination;
	};
}