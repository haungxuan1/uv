#pragma once
#include "DALSACapture.h"

namespace DALSACamera
{
	class CHPD : public CCapture
	{
	protected:
		CHPD();
		virtual ~CHPD();

	protected:
		virtual void Reg(HWND& lpHandle);
		virtual void Unreg(HWND& lpHandle);

	protected:
		virtual void OnArrival(map<string, vector<string>>& vArrival, HWND& lpHandle) = 0;
		virtual void OnRemove(map<string, vector<string>>& vRemove, HWND& lpHandle) = 0;

	private:
		void Termination();
		bool IsTermination();

	private:
		void EnumBoards(vector<string>& vBoards);
		void EnumCameras(const string& strBoard, vector<string>& vCameras);
		void EnumCameras(vector<string>& vBoards, map<string, vector<string>>& vCameras);

	private:
		void OnArrival(map<string, vector<string>>& vCur, map<string, vector<string>>& his, map<string, vector<string>>& vArrival);
		void OnRemove(map<string, vector<string>>& vCur, map<string, vector<string>>& his, map<string, vector<string>>& vRemove);

	private:
		static unsigned int __stdcall OnHPD(void* lpParam);
		static unsigned int __stdcall OnTermination(void* lpParam);

	private:
		map<string, vector<string>> m_vCameras;

	private:
		bool m_bTermination;
		CSRWLockBase m_lLock;
		CCondition m_cTermination;
	};
}