#pragma once
#include <afxwin.h>
#include <iostream>

using namespace std;

class WinUtil
{
public:
	static string getEditText(CEdit &tEdit);
	static int getEditTextToInt(CEdit& tEdit);
	static float getEditTextToFloat(CEdit& tEdit);
	static string trim(string& str);
	static bool checkIllegalCharacters(string& str);

private:
	WinUtil();
	~WinUtil();
};
