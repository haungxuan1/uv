#include "pch.h"
#include "SRWLock.h"
#include "Condition.h"
#include "PQMsg.h"
#include <vector>
#include <tuple>
#include "opencv.hpp"
#include "Log.h"
#include "PrClock/PrClock.h"

namespace XLPCamera
{
	namespace Capture
	{
		typedef std::tuple<
			unsigned long long,
			unsigned long long,
			cv::Mat *> vCapture;
		typedef std::vector<vCapture *> vCaptures;

		bool g_bTermination = false; // 注销标识
		CSRWLockBase g_lLock;
		CCondition g_cCapture; // 捕获信号到达
		CCondition g_cTermination; // 线程终止信号
		vCaptures g_vCaptures;

		bool IsTermination()
		{
			bool bTermination = true;
			{
				CSRLock lock(&g_lLock);

				bTermination = g_bTermination;
			}
			return bTermination;
		}

		void Termination()
		{
			CSWLock lock(&g_lLock);

			g_bTermination = true;
		}

		void Push_back(
			unsigned long long& uFrameID,
			unsigned long long& uTimestamp,
			int& nImWidth,
			int& nImHeight,
			unsigned int& uImBytes,
			void*& lpImage)
		{
			vCapture *lpCapture = new vCapture(uFrameID, uTimestamp, new cv::Mat(nImHeight, nImWidth, CV_8UC1)); // 相机采集灰度图

			memcpy(std::get<2>(*lpCapture)->data, lpImage, uImBytes);
			{
				CSWLock lock(&g_lLock);

				g_vCaptures.push_back(lpCapture);
			}
		}

		bool Pop_Front(vCapture*& lpImage)
		{
			// <1ms // 优化时可忽略
			bool bEmpty = true;
			{
				CSWLock lock(&g_lLock);

				if (!(bEmpty = g_vCaptures.empty()))
				{
					auto iFront = g_vCaptures.begin();

					lpImage = (*iFront);

					iFront = g_vCaptures.erase(iFront);
				}
			}
			return (!bEmpty);
		}

		unsigned int __stdcall OnCapture(void* lpParam)
		{
			//vCapture* lpCapture = nullptr;

			//while (!IsTermination())
			//{
			//	if (Pop_Front(lpCapture))
			//	{
			//		PostMessage(
			//			AfxGetApp()->GetMainWnd()->GetSafeHwnd(),
			//			PQM_DO3THINK_CAPTURE,
			//			0, 
			//			(LPARAM)lpCapture);
			//		continue;
			//	}
			//	g_cCapture.wait();
			//}
			//g_cTermination.signal();

			return 0;
		}

		unsigned int __stdcall OnTermination(void* lpParam)
		{
			Termination();

			g_cCapture.signal();

			return 0;
		}

		void OnCapture(
			unsigned long long& uFrameID,
			unsigned long long& uTimestamp,
			int& nImWidth,
			int& nImHeight,
			unsigned int& uImBytes,
			void*& lpImage)
		{
			Push_back(uFrameID, uTimestamp, nImWidth, nImHeight, uImBytes, lpImage);

			g_cCapture.signal();
		}

		void Reg()
		{
			CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnCapture, nullptr, 0, NULL));
		}

		void Unreg()
		{
			CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnTermination, nullptr, 0, NULL));

			g_cTermination.wait();
		}
	}
}

