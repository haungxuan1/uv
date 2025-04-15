#include "pch.h"
#include "DALSACameraHPD.h"

namespace DALSACamera
{
	CHPD::CHPD() :
		m_bTermination(false)
	{

	}

	CHPD::~CHPD()
	{

	}

	void CHPD::Reg(HWND& lpHandle)
	{
		CCapture::Reg(lpHandle);

		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnHPD, (void*)new tuple<CHPD*, HWND>(this, lpHandle), 0, NULL));
	}

	void CHPD::Unreg(HWND& lpHandle)
	{
		CCapture::Unreg(lpHandle);

		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnTermination, (void*)new tuple<CHPD*, HWND>(this, lpHandle), 0, NULL));

		m_cTermination.wait();
	}

	void CHPD::Termination()
	{
		CSWLock lock(&m_lLock);

		m_bTermination = true;
	}

	inline bool CHPD::IsTermination()
	{
		bool bTermination = true;
		{
			CSRLock lock(&m_lLock);

			bTermination = m_bTermination;
		}
		return bTermination;
	}

	void CHPD::EnumBoards(vector<string>& vBoards)
	{
		vBoards.clear();

		int serverCount = SapManager::GetServerCount();

		if (!!!serverCount) return;

		char serverName[CORSERVER_MAX_STRLEN] = { 0 };

		for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
		{
			if (!!!SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcq)) continue;

			memset(serverName, 0, sizeof(serverName));

			SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));

			vBoards.push_back(serverName);
		}
	}

	void CHPD::EnumCameras(const string& strBoard, vector<string>& vCameras)
	{
		vCameras.clear();

		if (strBoard.empty()) return;

		int deviceCount = SapManager::GetResourceCount(strBoard.c_str(), SapManager::ResourceAcq);

		if (!!!deviceCount) return;

		char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)] = { 0 };

		for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
		{
			memset(deviceName, 0, sizeof(deviceName));

			SapManager::GetResourceName(strBoard.c_str(), SapManager::ResourceAcq, deviceIndex, deviceName, sizeof(deviceName));

			vCameras.push_back(deviceName);
		}
	}

	void CHPD::EnumCameras(vector<string>& vBoards, map<string, vector<string>>& vCameras)
	{
		vCameras.clear();

		vector<string> vCamera;

		for (auto& iBoard : vBoards)
		{
			EnumCameras(iBoard, vCamera);

			if (vCamera.empty()) continue;

			vCameras.insert(pair<string, vector<string>>(iBoard, vCamera));
		}
	}

	void CHPD::OnArrival(
		map<string, vector<string>>& vCur, 
		map<string, vector<string>>& vHis, 
		map<string, vector<string>>& vArrival)
	{
		vArrival.clear(); vector<string> vCamera;

		for (auto& iBoard : vCur)
		{
			auto iHis = vHis.find(iBoard.first);

			if (iHis == vHis.end()) { vArrival.insert(pair<string, vector<string>>(iBoard.first, iBoard.second)); continue; }

			vCamera.clear();

			for (auto& iCamera : iBoard.second)
			{
				auto iHisCam = find(iHis->second.begin(), iHis->second.end(), iCamera);

				if (iHisCam != iHis->second.end()) continue;

				vCamera.push_back(iCamera);
			}
			if (vCamera.empty()) continue;

			vArrival.insert(pair<string, vector<string>>(iBoard.first, vCamera));
		}
	}

	void CHPD::OnRemove(
		map<string, vector<string>>& vCur, 
		map<string, vector<string>>& vHis,
		map<string, vector<string>>& vRemove)
	{
		vRemove.clear(); vector<string> vCamera;

		for (auto& iBoard : vHis)
		{
			auto iCur = vCur.find(iBoard.first);

			if (iCur == vCur.end()) { vRemove.insert(pair<string, vector<string>>(iBoard.first, iBoard.second)); continue; }

			vCamera.clear();

			for (auto& iCamera : iBoard.second)
			{
				auto iHisCam = find(iCur->second.begin(), iCur->second.end(), iCamera);

				if (iHisCam != iCur->second.end()) continue;

				vCamera.push_back(iCamera);
			}
			if (vCamera.empty()) continue;

			vRemove.insert(pair<string, vector<string>>(iBoard.first, vCamera));
		}
	}

	unsigned int __stdcall CHPD::OnHPD(void* lpParam)
	{
		tuple<CHPD*, HWND>* lpUserParam = (tuple<CHPD*, HWND>*)lpParam;

		CHPD* lpThis = get<0>(*lpUserParam);

		HWND lpHandle = get<1>(*lpUserParam);

		vector<string> vBoards;

		map<string, vector<string>> vCameras, vArrival, vRemove;

		while (!lpThis->IsTermination())
		{
			lpThis->EnumBoards(vBoards);

			lpThis->EnumCameras(vBoards, vCameras);

			lpThis->OnArrival(vCameras, lpThis->m_vCameras, vArrival);

			lpThis->OnRemove(vCameras, lpThis->m_vCameras, vRemove);

			vArrival.empty() ? []() {}() : lpThis->OnArrival(vArrival, lpHandle);

			vRemove.empty() ? []() {}() : lpThis->OnRemove(vRemove, lpHandle);

			lpThis->m_vCameras = vCameras;

			Sleep(100);
		}

		lpThis->m_cTermination.signal();

		delete lpUserParam;

		return 0;
	}

	unsigned int __stdcall CHPD::OnTermination(void* lpParam)
	{
		CHPD* lpThis = (CHPD*)lpParam;

		lpThis->Termination();

		return 0;
	}
}
