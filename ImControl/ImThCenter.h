#pragma once
#include <vector>
#include "ImLayerInfo.h"
#include "SRWLock.h"
#include "Condition.h"

using namespace cv;
using namespace std;

namespace ImThCenter
{
	class CImThCenter
	{
	protected:
		CImThCenter();
		virtual ~CImThCenter();

	public:
		bool Empty();

	private:
		virtual void PrSize(double& w, double& h) = 0;
		virtual void Render(long long& nSign) = 0;

	private:
		static unsigned int __stdcall ScThread(void* lpUserparam);
		static unsigned int __stdcall RoThread(void* lpUserparam);
		static unsigned int __stdcall PrThread(void* lpUserparam);
		static unsigned int __stdcall BgThread(void* lpUserparam);
		static unsigned int __stdcall TeThread(void* lpUserparam);
		static unsigned int __stdcall CnThread(void* lpUserparam);

	private:
		bool IsTermination(int nTh);
		void Termination(int nTh);
		void Termination();

	private:
		bool Image2BGRA(ImLayer::tagLayer& tParam);
		bool OnImage2BGRA(ImLayer::tagLayer& tParam);
		void OnGray2BGRA(ImLayer::tagLayer& tParam);
		void OnBGR2BGRA(ImLayer::tagLayer& tParam);
		void OnCMYK2BGRA(ImLayer::tagLayer& tParam);
		void OnBGRA2BGRA(ImLayer::tagLayer& tParam);
		void OnCMKYW2BGRA(ImLayer::tagLayer& tParam);
		void SyncBGRA(ImLayer::tagLayer& tParam);

	private:
		bool Preview(ImLayer::tagLayer& tParam);
		bool OnPreview(ImLayer::tagLayer& tParam);
		void SyncPreview(ImLayer::tagLayer& tParam);

	private:
		bool Rotating(ImLayer::tagLayer& tParam);
		bool OnRotating(ImLayer::tagLayer& tParam);
		void SyncRotating(ImLayer::tagLayer& tParam);

	private:
		bool Scaling(ImLayer::tagLayer& tParam);
		bool OnScaling(ImLayer::tagLayer& tParam);
		void SyncScaling(ImLayer::tagLayer& tParam);

	private:
		bool Contrast(ImLayer::tagLayer& tParam);
		bool OnContrast(ImLayer::tagLayer& tParam);
		void SyncContrast(ImLayer::tagLayer& tParam);

	protected:
		CCondition m_cBGRA;
		CCondition m_cPreview;
		CCondition m_cScaling;
		CCondition m_cRotating;
		CCondition m_cContrast;
		CSRWLockBase m_lLock;
		vector<ImLayer::tagLayer> m_vLayer;

	private:
		CCondition m_cTermination;
		unsigned char m_uTermination[3]; // 0 ¹¤×÷ | 1 ÍË³ö
	};
}