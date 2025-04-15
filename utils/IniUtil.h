#pragma once
#include <afxstr.h>

class IniUtil
{
public:
	static int readIntValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, int iDefVal);
	static bool writeIntValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, int iValue);

	static double readDoubleValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, double dDefVal);
	static bool writeDoubleValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, double dValue);

	static CString readStringValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, LPCTSTR pcDefVal);
	static bool writeStringValue(LPCTSTR pcFileName, LPCTSTR pcSection, LPCTSTR pcKey, LPCTSTR pcValue);

private:
	IniUtil();
	~IniUtil();
};
