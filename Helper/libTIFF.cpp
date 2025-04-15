#include "pch.h"
#include "Helper.h"
#include "libtiff\tiffio.h"

using namespace cv;
using namespace std;

void ReadTif(string strTif, Mat& cTif)
{
	TIFF* lptIFF = TIFFOpen(strTif.c_str(), "r");

	uint uWidth = 0, uRowSperstrip = 0, uStrips = 0, uSamplesperPixel = 0;

	uStrips = TIFFNumberOfStrips(lptIFF);

	TIFFGetField(lptIFF, TIFFTAG_IMAGEWIDTH, &uWidth);

	TIFFGetField(lptIFF, TIFFTAG_ROWSPERSTRIP, &uRowSperstrip);

	TIFFGetField(lptIFF, TIFFTAG_SAMPLESPERPIXEL, &uSamplesperPixel);

	size_t stripSize = TIFFStripSize(lptIFF);

	uint uHeight = uRowSperstrip * uStrips;

	cTif = Mat(uHeight, uWidth, CV_8UC(uSamplesperPixel));

	for (tmsize_t i = 0; i < uStrips; ++i)
	{
		uint8* lpData = (uint8*)cTif.data;

		tmsize_t os = i * stripSize;

		uint8* lpTmp = lpData + os;

		TIFFReadEncodedStrip(lptIFF, i, lpTmp, stripSize);
	}
	TIFFClose(lptIFF);
}

void WriteTif(string strHeader, string strTif, Mat& cTif)
{
	TIFF* lpIn = TIFFOpen(strHeader.c_str(), "r");

	TIFF* lpOut = TIFFOpen(strTif.c_str(), "w");


	uint32 uStrips = 0, rowsperstrip = 0;

	uint16 samplesperpixel, photometric, planarconfig, compress;

	float xResolution, yResolution;	uint16 bitspersample = 0;

	TIFFGetField(lpIn, TIFFTAG_BITSPERSAMPLE, &bitspersample);
	TIFFGetField(lpIn, TIFFTAG_PHOTOMETRIC, &photometric);
	TIFFGetField(lpIn, TIFFTAG_PLANARCONFIG, &planarconfig);
	TIFFGetField(lpIn, TIFFTAG_COMPRESSION, &compress);
	TIFFGetField(lpIn, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
	TIFFGetField(lpIn, TIFFTAG_XRESOLUTION, &xResolution);
	TIFFGetField(lpIn, TIFFTAG_YRESOLUTION, &yResolution); // 获取头文件结构


	uStrips = ceil((double)cTif.rows / rowsperstrip);
	size_t stripSize = (uint64)rowsperstrip * cTif.cols * cTif.channels();
	if (cTif.rows < uStrips * rowsperstrip)
		copyMakeBorder(cTif, cTif, 0, (uStrips * rowsperstrip - cTif.rows), 0, 0, BORDER_CONSTANT); // 填充整数行

	TIFFSetField(lpOut, TIFFTAG_IMAGEWIDTH, cTif.cols);
	TIFFSetField(lpOut, TIFFTAG_IMAGELENGTH, cTif.rows);
	TIFFSetField(lpOut, TIFFTAG_SAMPLESPERPIXEL, cTif.channels());
	TIFFSetField(lpOut, TIFFTAG_BITSPERSAMPLE, bitspersample);
	TIFFSetField(lpOut, TIFFTAG_PHOTOMETRIC, photometric);
	TIFFSetField(lpOut, TIFFTAG_PLANARCONFIG, planarconfig);
	TIFFSetField(lpOut, TIFFTAG_COMPRESSION, compress);
	TIFFSetField(lpOut, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
	TIFFSetField(lpOut, TIFFTAG_XRESOLUTION, xResolution);
	TIFFSetField(lpOut, TIFFTAG_YRESOLUTION, yResolution); // 填充文件头

	for (tmsize_t i = 0; i < uStrips; ++i)
	{
		uint8* lpData = (uint8*)cTif.data;

		tmsize_t os = i * stripSize;

		uint8* lpTmp = lpData + os;

		TIFFWriteEncodedStrip(lpOut, i, lpTmp, stripSize);
	}
	TIFFClose(lpIn); TIFFClose(lpOut);
}