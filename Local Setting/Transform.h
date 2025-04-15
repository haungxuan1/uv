#pragma once
#include <string>
#include <vector>

namespace Transform
{
	std::wstring String2Wstring(std::string strSrc);

	std::string Wstring2String(std::wstring strSrc);

	std::wstring CString2WString(CString rSrc);

	std::string CString2String(CString rSrc);

	CString WString2CString(std::wstring strSrc);
}