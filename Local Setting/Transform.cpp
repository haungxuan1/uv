#include "pch.h"
#include "Transform.h"
#include <ctype.h>
#include <Windows.h>

namespace Transform
{
	std::wstring String2Wstring(std::string strSrc)
	{
		int nSize = MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), strSrc.size(), NULL, 0);
		wchar_t* buffer = new wchar_t[nSize + 1];
		MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), strSrc.size(), buffer, nSize);
		buffer[nSize] = '\0'; 
		std::wstring strDest(buffer);
		delete[] buffer;
		return strDest;
	}

	std::string Wstring2String(std::wstring strSrc)
	{
		int nSize = WideCharToMultiByte(CP_ACP, 0, strSrc.c_str(), strSrc.size(), NULL, 0, NULL, NULL);
		char* buffer = new char[nSize + 1];
		WideCharToMultiByte(CP_ACP, 0, strSrc.c_str(), strSrc.size(), buffer, nSize, NULL, NULL);
		buffer[nSize] = '\0';
		std::string strDest(buffer);
		delete[] buffer;
		return strDest;
	}

	std::wstring CString2WString(CString rSrc)
	{
		return rSrc.GetBuffer();
	}

	std::string CString2String(CString rSrc)
	{
		return Wstring2String(CString2WString(rSrc));
	}

	CString WString2CString(std::wstring strSrc)
	{
		return strSrc.c_str();
	}
}