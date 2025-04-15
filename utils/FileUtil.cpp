#include "pch.h"
#include <Shlwapi.h>
#include "FileUtil.h"

#pragma comment(lib, "Shlwapi.lib")

CString FileUtil::getExePath()
{
	TCHAR acPath[MAX_PATH];
	CString tExePath = L"\0";

	memset((void*)acPath, 0, sizeof(acPath));

	if (GetModuleFileName(NULL, acPath, sizeof(acPath)) > 0) {
		wchar_t* pcTmp = wcsrchr(acPath, L'\\');

		if (pcTmp != NULL) {
			*pcTmp = L'\0';
		}

		tExePath = acPath;
	}

	return tExePath;
}

CString FileUtil::getAppPath(const TCHAR* pcFile)
{
	CString tFilePath = L"\0";

	if ((pcFile != nullptr) && (*pcFile != L'\0')) {
		tFilePath = getExePath() + L"\\" + pcFile;
	}

	return tFilePath;
}

CString FileUtil::getFileName(const CString& tFilePath, bool bSuffix)
{
	CString tFileName = L"\0";
	LPCTSTR ptFileName = PathFindFileName(tFilePath);
	
	if (ptFileName != nullptr) {
		tFileName = CString(ptFileName);

		if (!bSuffix) {
			int iDotPos = tFileName.ReverseFind('.');

			if (iDotPos > 0) {
				tFileName = tFileName.Left(iDotPos);
			}
		}
	}

	return tFileName;
}

CString FileUtil::getFileNameWithoutSuffix(const CString& tFilePath)
{
	int iDotPos = tFilePath.ReverseFind('.');
	CString tFileName = L"\0";

	if (iDotPos > 0) {
		tFileName = tFilePath.Left(iDotPos);
	} else if (iDotPos < 0){
		tFileName = tFilePath;
	}

	return tFileName;
}

CString FileUtil::getPath(CString& tFileName)
{
	int iDotPos = tFileName.ReverseFind('\\');
	int iDotPos1 = tFileName.ReverseFind('/');
	CString tFilePath = L"\0";

	if (iDotPos < iDotPos1) {
		iDotPos = iDotPos1;
	}

	if (iDotPos > 0) {
		tFilePath = tFileName.Left(iDotPos);
	}
	else if (iDotPos < 0) {
		tFilePath = tFileName;
	}

	return tFilePath;
}
