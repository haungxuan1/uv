#include "pch.h"
#include "XLPCamera.h"
#include "DVPCamera.h"
#include "PQMsg.h"

namespace XLPCamera
{
	namespace HPD
	{
		extern void Reg();

		extern void Unreg();
	}

	dvpHandle g_vHandle = 0;

	int OnCameraCallback(
		dvpHandle handle,
		dvpStreamEvent event,
		void* pContext,
		dvpFrame* pFrame,
		void* pBuffer)
	{
		//cv::Mat* lpIm = new cv::Mat(pFrame->iHeight, pFrame->iWidth, CV_8UC1);

		//memcpy(lpIm->data, pBuffer, pFrame->uBytes);

		//PQPrinter().ImCapture(std::make_tuple(pFrame->uFrameID, pFrame->uTimestamp, lpIm));




		////cv::Mat *lpIm= new cv::Mat(pFrame->iHeight, pFrame->iWidth, CV_8UC1);

		////memcpy(lpIm->data, pBuffer, pFrame->uBytes);

		////PostMessage(
		////	AfxGetApp()->GetMainWnd()->GetSafeHwnd(),
		////	PQM_CAMERA_CAPTURE,
		////	pFrame->uFrameID,
		////	0/*(LPARAM)lpIm*/);
		return 0;
	}

	bool RegCallbackFunc()
	{
		return (dvpRegisterStreamCallback(
			g_vHandle,
			OnCameraCallback, 
			STREAM_EVENT_ARRIVED, 
			nullptr) == DVP_STATUS_OK);
	}

	bool Close()
	{
		if (!!!g_vHandle) return true;

		dvpStatus eRes = DVP_STATUS_UNKNOW;

		eRes = dvpClose(g_vHandle);

		g_vHandle = (eRes != DVP_STATUS_OK ? g_vHandle : 0);

		return (!!!g_vHandle);
	}

	bool OpenByName(const char* lpFriendlyName) // 由原厂固化，原则上永不重名
	{
		dvpStatus eStreamState = DVP_STATUS_OK;

#ifdef DEBUG
		eStreamState =(dvpOpenByName(lpFriendlyName, OPEN_DEBUG, &g_vHandle));
#else
		eStreamState = (dvpOpenByName(lpFriendlyName, OPEN_NORMAL, &g_vHandle));
#endif

		return (eStreamState == DVP_STATUS_OK);
	}

	bool Stop()
	{
		dvpStreamState eStreamState = STATE_STOPED;

		if (dvpGetStreamState(g_vHandle, &eStreamState) != DVP_STATUS_OK) return false;

		if (eStreamState == STATE_STOPED) return true;

		return (dvpStop(g_vHandle) == DVP_STATUS_OK);
	}

	bool Start()
	{
		if (!Stop()) return false;

		if (!RegCallbackFunc()) return false;

		dvpStatus eStreamState = dvpStart(g_vHandle);

		return eStreamState == DVP_STATUS_OK;
	}

	bool Open(const char* lpFriendlyName)
	{
		return (Close() && OpenByName(lpFriendlyName) && Start());
	}

	void Reg()
	{
		HPD::Reg();
	}

	void Unreg()
	{
		HPD::Unreg();
	}
}