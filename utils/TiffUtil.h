#pragma once
#include <afxwin.h>
#include <iostream>

using namespace std;

class TiffUtil
{
public:
	static bool readTiffFileToMat(Mat &tTiffMat, std::string tFileName);
	static bool writeMatToTiffFile(std::string tFileName, Mat &tImgMat);

private:
	TiffUtil();
	~TiffUtil();
};
