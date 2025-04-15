#include "pch.h"
#include "Helper.h"

using namespace cv;
using namespace std;

template<typename  T>
T* Padding(T* lpIn, int nSize, int nWin)
{
	int nMaxIdx = nSize - 1;

	T* lpPadding = new T[2ll * nWin + nSize];

	memcpy(lpPadding + nWin, lpIn, nSize * sizeof(T));

	for (int i = 0; i < nWin; ++i)
	{
		lpPadding[nWin - i - 1] = lpPadding[nWin + i + 1];

		lpPadding[nMaxIdx + nWin + i + 1] = lpPadding[nMaxIdx + nWin - i - 1];
	}
	return lpPadding;
}

// 用于双峰查找,无需优化
void MeanFileter(int*& nHis, int nSize, int nWin)
{
	int* lp = Padding(nHis, nSize, nWin);

	for (int i = nWin; i < (nWin + nSize); ++i)
	{
		for (int j = -nWin; j < nWin; ++j)
		{
			nHis[i - nWin] += lp[j + i];
		}
		nHis[i - nWin] /= (2 * nWin + 1);
	}
	delete[] lp;
}