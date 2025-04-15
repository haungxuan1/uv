#include "pch.h"
#include "Helper.h"

using namespace cv;
using namespace std;

void FileInDir(array<wstring, 2> vDir, vector<array<wstring, 2>>& vFile)
{
	WIN32_FIND_DATA findData;

	HANDLE hFile = FindFirstFile(vDir[0].c_str(), &findData);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				vFile.push_back(array<wstring, 2>{ findData.cFileName, vDir[1] + findData.cFileName });
			}
		} while (FindNextFile(hFile, &findData));

		FindClose(hFile);
	}
}

void FileInDir(wstring strDir, vector<array<wstring, 2>>& vFile)
{
	array<wstring, 2> vDir{ strDir + L"\\*.*", strDir + L"\\" };

	FileInDir(vDir, vFile);
}

void SortingFiles(vector<array<wstring, 2>>& vFile)
{
	sort(vFile.begin(), vFile.end(),
		[](array<wstring, 2>& s1, array<wstring, 2>& s2) { return (_wtoi(s1[0].c_str()) < _wtoi(s2[0].c_str())); });
}
