#pragma once
#include "DALSACameraHPD.h"

namespace DALSACamera
{
	class CCamera : public CHPD
	{
	public:
		CCamera();
		virtual ~CCamera();

	public:
		virtual void Reg(HWND& lpHandle);
		virtual void Unreg(HWND& lpHandle);

	public:
		void Connect(string strBoard, int nCameraIndex, string strConfig, HWND& lpHandle);
		void Disconnect(HWND& lpHandle);
		void Grab(const int& nExtTriggerFrameCount, HWND& lpHandle);
		void Freeze(HWND& lpHandle);

	private:
		virtual void OnConnect(HWND& lpHandle) = 0;
		virtual void OnDisconnect(HWND& lpHandle) = 0;
		virtual void OnGrab(HWND& lpHandle) = 0;
		virtual void OnFreeze(HWND& lpHandle) = 0;

	private:
		SapBuffer* m_lpBuffer;
		SapTransfer* m_lpTransfer;
		SapAcquisition* m_lpAcquisition;
	};
}
