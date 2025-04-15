#include "pch.h"
#include "DALSACamera.h"
#include "PQMsg.h"

namespace DALSACamera
{
	CCamera::CCamera():
		m_lpBuffer(nullptr), m_lpTransfer(nullptr), m_lpAcquisition(nullptr)
	{

	}

	CCamera::~CCamera()
	{

	}

	void CCamera::Reg(HWND& lpHandle)
	{
		CHPD::Reg(lpHandle);
	}

	void CCamera::Unreg(HWND& lpHandle)
	{
		CHPD::Unreg(lpHandle);
	}

	void CCamera::Connect(string strBoard, int nCameraIndex, string strConfig, HWND& lpHandle)
	{
		SapLocation loc(strBoard.c_str(), nCameraIndex);

		m_lpAcquisition = new SapAcquisition(loc, strConfig.c_str());

		m_lpBuffer = new SapBuffer(2, m_lpAcquisition);

		m_lpTransfer = new SapAcqToBuf(m_lpAcquisition, m_lpBuffer, OnAcquisition, this);

		if (!!m_lpAcquisition && !!m_lpBuffer && !!m_lpTransfer &&
			!!m_lpAcquisition->Create() && !!m_lpBuffer->Create() && !!m_lpTransfer->Create()) { OnConnect(lpHandle); return; }

		if (!!m_lpTransfer) { m_lpTransfer->Destroy(); delete m_lpTransfer; }

		if (!!m_lpBuffer) { m_lpBuffer->Destroy(); delete m_lpBuffer; }

		if (!!m_lpAcquisition) { m_lpAcquisition->Destroy(); delete m_lpAcquisition; }

		m_lpTransfer = nullptr; m_lpBuffer = nullptr; m_lpAcquisition = nullptr;
	}

	void CCamera::Disconnect(HWND& lpHandle)
	{
		if (!!m_lpTransfer)
		{
			if (m_lpTransfer->IsGrabbing())
			{
				if (!m_lpTransfer->Wait(5000)) m_lpTransfer->Abort();
			}
			m_lpTransfer->Destroy(); delete m_lpTransfer;
		}
		if (!!m_lpBuffer) { m_lpBuffer->Destroy(); delete m_lpBuffer;}

		if (!!m_lpAcquisition) { m_lpAcquisition->Destroy(); delete m_lpAcquisition; }

		m_lpTransfer = nullptr; m_lpBuffer = nullptr; m_lpAcquisition = nullptr;

		OnDisconnect(lpHandle);
	}

	void CCamera::Grab(const int& nExtTriggerFrameCount, HWND& lpHandle)
	{
		if (!!!m_lpAcquisition) return;

		m_lpAcquisition->SetParameter(CORACQ_PRM_EXT_TRIGGER_FRAME_COUNT, nExtTriggerFrameCount, true);

		if (!!!m_lpTransfer) return;

		if (!!!m_lpTransfer->Grab()) return;

		OnGrab(lpHandle);
	}

	void CCamera::Freeze(HWND& lpHandle)
	{
		if (!!!m_lpTransfer) { return; }

		m_lpTransfer->Freeze();

		if(!m_lpTransfer->Wait(5000)) m_lpTransfer->Abort();

		OnFreeze(lpHandle);
	}
}
