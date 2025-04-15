#include "pch.h"
#include "DVPCamera.h"
#include "PQMsg.h"
#include "SRWLock.h"
#include "Condition.h"
#include <vector>
#include <string>
#include <tuple>

namespace XLPCamera
{
	namespace HPD
	{
		bool g_bTermination = false;
		CSRWLockBase g_lLock; // 线程锁
		CCondition g_cTermination; // 线程终止信号
		std::vector<std::tuple<std::string, std::string>> g_vCamera[2]; // 相机原始序号 + 设备友好名称 // 0 history | 1 current

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

		bool RefreshCameraList(UINT32& uCamera)
		{
			dvpCameraInfo tCameraInfo;

			g_vCamera[1].clear();

			for (UINT32 uIndex = 0; uIndex < uCamera; ++uIndex)
			{
				if (dvpEnum(uIndex, &tCameraInfo) != DVP_STATUS_OK) return false;

				g_vCamera[1].push_back(std::make_tuple(tCameraInfo.OriginalSerialNumber, tCameraInfo.FriendlyName));
			}
			return true;
		}

		void ArrivalEvent() // 发现相机
		{
			//for (auto& iCur : g_vCamera[1])
			//{
			//	auto iHis = std::find_if(g_vCamera[0].begin(), g_vCamera[0].end(),
			//		[iCur](const std::tuple<std::string, std::string>& iHis)
			//		{
			//			return std::get<0>(iHis) == std::get<0>(iCur);
			//		});
			//	if (iHis != g_vCamera[0].end()) continue;

			//	::PostMessage(
			//		AfxGetApp()->GetMainWnd()->GetSafeHwnd(),
			//		PQM_DO3THINK_ARRIVAL,
			//		0,
			//		(LPARAM)new std::string(std::get<1>(iCur)));

			//	g_vCamera[0].push_back(iCur);
			//}
		}

		void RemoveEvent() // 相机移除
		{
			//for (auto iHis = g_vCamera[0].begin(); iHis != g_vCamera[0].end();)
			//{
			//	auto iCur = std::find_if(g_vCamera[1].begin(), g_vCamera[1].end(),
			//		[iHis](const std::tuple<std::string, std::string>& iCur)
			//		{
			//			return std::get<0>(iCur) == std::get<0>(*iHis);
			//		});
			//	if (iCur != g_vCamera[1].end()) { ++iHis; continue; }

			//	PostMessage(
			//		AfxGetApp()->GetMainWnd()->GetSafeHwnd(), 
			//		PQM_DO3THINK_REMOVE,
			//		0, 
			//		(LPARAM)new std::string(std::get<1>(*iHis)));

			//	iHis = g_vCamera[0].erase(iHis);
			//}
		}

		bool RefreshCameraInfo(UINT32& uCamera) // 刷新相机列表
		{
			if (!RefreshCameraList(uCamera)) return false;

			ArrivalEvent(); RemoveEvent();

			return true;
		}

		unsigned int __stdcall OnHPD(void* lpParam)
		{
			UINT32 uCamera = 0;

			while (!IsTermination())
			{
				if ((dvpRefresh(&uCamera) != DVP_STATUS_OK) || (!RefreshCameraInfo(uCamera)))
				{
					// postmessage
					continue;
				}
				Sleep(100);
			}
			g_cTermination.signal();

			return 0;
		}

		unsigned int __stdcall OnTermination(void* lpParam)
		{
			Termination();

			return 0;
		}

		void Reg()
		{
			CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnHPD, nullptr, 0, NULL));
		}

		void Unreg()
		{
			CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnTermination, nullptr, 0, NULL));

			g_cTermination.wait();
		}
	}
}
