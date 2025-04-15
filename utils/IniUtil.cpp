#include "pch.h"
#include "IniUtil.h"

int IniUtil::readIntValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, int iDefVal)
{
	int iRet = iDefVal;

	if ((pcFileName != nullptr) && (*pcFileName != L'\0') && (pcSection != nullptr) && 
		(*pcSection != L'\0') && (pcKey != nullptr) && (*pcKey != L'\0')) {
		iRet = GetPrivateProfileInt(pcSection, pcKey, iDefVal, pcFileName);
	}

	return iRet;
}

bool IniUtil::writeIntValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, int iValue)
{
	bool bRet = false;

	if ((pcFileName != nullptr) && (*pcFileName != L'\0') && (pcSection != nullptr) && 
		(*pcSection != L'\0') && (pcKey != nullptr) && (*pcKey != L'\0')) {
		TCHAR acVal[32] = L"\0";

		wsprintf(acVal, L"%d", iValue);

		if (WritePrivateProfileString(pcSection, pcKey, acVal, pcFileName)) {
			bRet = true;
		}
	}

	return bRet;
}

double IniUtil::readDoubleValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, double dDefVal)
{
	CString tValue = readStringValue(pcFileName, pcSection, pcKey, L"0");

	return _ttof(tValue);
}

bool IniUtil::writeDoubleValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, double dValue)
{
	bool bRet = false;
	TCHAR acVal[32] = L"\0";

	wsprintf(acVal, L"%f", dValue);

	if (WritePrivateProfileString(pcSection, pcKey, acVal, pcFileName)) {
		bRet = true;
	}

	return bRet;
}

CString IniUtil::readStringValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, LPCTSTR pcDefVal)
{
	CString tRet(pcDefVal);

	if ((pcFileName != nullptr) && (*pcFileName != L'\0') && (pcSection != nullptr) &&
		(*pcSection != '\0') && (pcKey != nullptr) && (*pcKey != '\0')) {
		TCHAR acBuf[512];

		memset((void*)acBuf, 0, sizeof(acBuf));

		GetPrivateProfileString(pcSection, pcKey, pcDefVal, acBuf, sizeof(acBuf), pcFileName);

		tRet = CString(acBuf);
	}

	return tRet;
}

bool IniUtil::writeStringValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, LPCTSTR pcValue)
{
	bool bRet = false;

	if ((pcFileName != nullptr) && (*pcFileName != L'\0') && (pcSection != nullptr) &&
		(*pcSection != L'\0') && (pcKey != nullptr) && (*pcKey != L'\0') && (pcValue != nullptr)) {
		if (WritePrivateProfileString(pcSection, pcKey, pcValue, pcFileName)) {
			bRet = true;
		}
	}

	return bRet;
}
