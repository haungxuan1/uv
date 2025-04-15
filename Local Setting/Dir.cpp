#include "pch.h"
#include <string>

namespace PQSetting
{
	namespace Dir
	{
		std::wstring g_strDir;
		std::wstring g_strExeDir;
		std::wstring g_strMoldDir;

		const std::wstring &ExeDir()
		{
			return g_strExeDir;
		}

		const std::wstring &MoldDir()
		{
			return g_strMoldDir;
		}

		void Reg()
		{
			wchar_t szbuf[MAX_PATH] = { 0 };
			GetModuleFileName(NULL, szbuf, MAX_PATH);
			g_strDir.assign(szbuf);
			std::wstring::size_type nFind = g_strDir.find_last_of(L"\\");
			ASSERT(nFind != std::wstring::npos);
			g_strDir = g_strDir.substr(0, nFind);
			g_strExeDir = g_strDir + L"\\";
			g_strMoldDir = g_strDir + L"\\Mold\\";
		}

		void Unreg()
		{

		}
	}
}


