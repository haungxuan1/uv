#include "pch.h"
#include "WinUtil.h"
#include "../Local Setting/Transform.h"
#include <regex>

using namespace Transform;

string WinUtil::getEditText(CEdit& tEdit)
{
	TCHAR acBuf[512];

	memset((void*)acBuf, 0, sizeof(acBuf));

	tEdit.GetWindowText(acBuf, sizeof(acBuf));

	string strBuf = CString2String(acBuf);
	
	return trim(strBuf);
}

int WinUtil::getEditTextToInt(CEdit& tEdit)
{
	TCHAR acBuf[512];

	memset((void*)acBuf, 0, sizeof(acBuf));

	tEdit.GetWindowText(acBuf, sizeof(acBuf));

	string strBuf = CString2String(acBuf);

	return strBuf.empty() ? 0 : stoi(strBuf);
}

float WinUtil::getEditTextToFloat(CEdit& tEdit)
{
	TCHAR acBuf[512];

	memset((void*)acBuf, 0, sizeof(acBuf));

	tEdit.GetWindowText(acBuf, sizeof(acBuf));

	string strBuf = CString2String(acBuf);

	return strBuf.empty() ? 0.0f : stof(strBuf);
}

string WinUtil::trim(string& str)
{
	if (str.empty()){
		return str;
	}

	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ") + 1);
	return str;
}

bool WinUtil::checkIllegalCharacters(string& str)
{
	std::smatch result;
	std::regex illegalChars(R"([<>:"/\\|?*])");

	std::regex_search(str, result, illegalChars);

	return result.empty();
}
