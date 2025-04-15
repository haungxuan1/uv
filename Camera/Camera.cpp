#include "pch.h"
#include "Camera.h"
#include "PQMsg.h"

namespace Camera
{
	CCamera::CCamera()
	{

	}

	CCamera::~CCamera()
	{

	}

	void CCamera::Reg(HWND lpHandle)
	{
		DALSACamera::CCamera::Reg(lpHandle);
	}

	void CCamera::Unreg(HWND lpHandle)
	{
		DALSACamera::CCamera::Unreg(lpHandle);
	}

	void CCamera::Connect(string strBoard, int nCameraIndex, string strConfig, HWND lpHandle)
	{
		DALSACamera::CCamera::Connect(strBoard, nCameraIndex, strConfig, lpHandle);
	}

	void CCamera::Disconnect(HWND lpHandle)
	{
		DALSACamera::CCamera::Disconnect(lpHandle);
	}

	void CCamera::Grab(int nExtTriggerFrameCount, HWND lpHandle)
	{
		DALSACamera::CCamera::Grab(nExtTriggerFrameCount, lpHandle);
	}

	void CCamera::Freeze(HWND lpHandle)
	{
		DALSACamera::CCamera::Freeze(lpHandle);
	}

	void CCamera::OnConnect(HWND& lpHandle)
	{
		::PostMessage(lpHandle, PQM_CAMERA_CONNECT, 0, 0);
	}

	void CCamera::OnDisconnect(HWND& lpHandle)
	{
		::PostMessage(lpHandle, PQM_CAMERA_DISCONNECT, 0, 0);
	}

	void CCamera::OnGrab(HWND& lpHandle)
	{
		SetInitiator(lpHandle);

		::PostMessage(lpHandle, PQM_CAMERA_GRAB, 0, 0);
	}

	void CCamera::OnFreeze(HWND& lpHandle)
	{
		::PostMessage(lpHandle, PQM_CAMERA_FREEZE, 0, 0);
	}

	void CCamera::OnArrival(map<string, vector<string>>& vArrival, HWND& lpHandle)
	{
		::PostMessage(lpHandle, PQM_CAMERA_ARRIVAL, 0, (LPARAM)new map<string, vector<string>>(vArrival));
	}

	void CCamera::OnRemove(map<string, vector<string>>& vRemove, HWND& lpHandle)
	{
		::PostMessage(lpHandle, PQM_CAMERA_REMOVE, 0, (LPARAM)new map<string, vector<string>>(vRemove));
	}

	void CCamera::OnCapture(ImPtr::CImPtr& cFrame, HWND& lpHandle)
	{
		::PostMessage(lpHandle, PQM_CAMERA_CAPTURE, 0, (LPARAM)new ImPtr::CImPtr(cFrame));
	}
}