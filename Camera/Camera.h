#pragma once
#include "DALSACamera.h"

namespace Camera
{
	class CCamera : public DALSACamera::CCamera
	{
	public:
		CCamera();
		virtual ~CCamera();

	public:
		virtual void Reg(HWND lpHandle);
		virtual void Unreg(HWND lpHandle);

	public:
		void Connect(string strBoard, int nCameraIndex, string strConfig, HWND lpHandle);
		void Disconnect(HWND lpHandle);
		void Grab(int nExtTriggerFrameCount, HWND lpHandle);
		void Freeze(HWND lpHandle);

	private:
		virtual void OnConnect(HWND& lpHandle);
		virtual void OnDisconnect(HWND& lpHandle);
		virtual void OnGrab(HWND& lpHandle);
		virtual void OnFreeze(HWND& lpHandle);
		virtual void OnArrival(map<string, vector<string>>& vArrival, HWND& lpHandle);
		virtual void OnRemove(map<string, vector<string>>& vRemove, HWND& lpHandle);
		virtual void OnCapture(ImPtr::CImPtr& cFrame, HWND& lpHandle);
	};
}
