#pragma once
#include <afxstr.h>

class FileUtil
{
public:
	static CString getExePath();
	static CString getAppPath(const TCHAR* pcFile);
	static CString getFileName(const CString &tFilePath, bool bSuffix=true);
	static CString getFileNameWithoutSuffix(const CString& tFilePath);
	static CString getPath(CString &tFileName);

private:
	FileUtil();
	~FileUtil();
};
