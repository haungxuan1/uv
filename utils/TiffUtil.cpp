#include "pch.h"
#include "TiffUtil.h"

TiffUtil::TiffUtil()
{
}

TiffUtil::~TiffUtil()
{

}

bool TiffUtil::readTiffFileToMat(Mat& tTiffMat, std::string tFileName)
{
	bool bRet = false;

	TIFF* ptTiff = TIFFOpen(tFileName.c_str(), "rb");
	if (ptTiff != nullptr) {
		int iWid = 0;
		int iHei = 0;

		TIFFGetField(ptTiff, TIFFTAG_IMAGEWIDTH, &iWid);
		TIFFGetField(ptTiff, TIFFTAG_IMAGELENGTH, &iHei);

		if ((iWid > 0) && (iHei > 0)) {
			unsigned short usChn = 0;

			TIFFGetField(ptTiff, TIFFTAG_SAMPLESPERPIXEL, &usChn);

			if (usChn > 0) {
				unsigned char* pucLineBuf = new unsigned char[usChn * iWid];

				if (pucLineBuf != NULL) {
					int i;
					std::vector<cv::Mat> tProductMatVector;

					for (i = 0; i < usChn; i++) {
						tProductMatVector.push_back(Mat(iHei, iWid, CV_8UC1));
					}

					for (int y = 0; y < iHei; y++) {
						TIFFReadScanline(ptTiff, pucLineBuf, y);

						for (i = 0; i < usChn; i++) {
							for (int x = 0; x < iWid; x++) {
								tProductMatVector[i].at<uchar>(y, x) = *(pucLineBuf + usChn * x + i);
							}
						}
					}

					merge(tProductMatVector, tTiffMat);

					bRet = true;

					delete[] pucLineBuf;
				}
			}
		}

		TIFFClose(ptTiff);
	}

	return bRet;
}

bool TiffUtil::writeMatToTiffFile(std::string tFileName, Mat& tImgMat)
{
	bool bRet = false;

	if (!tImgMat.empty() && !tFileName.empty() && (CV_8U == tImgMat.depth()) && (tImgMat.channels() > 0)) {
		TIFF* ptTiff = TIFFOpen(tFileName.c_str(), "w");

		if (ptTiff != nullptr) {
			TIFFSetField(ptTiff, TIFFTAG_IMAGEWIDTH,      tImgMat.cols);
			TIFFSetField(ptTiff, TIFFTAG_IMAGELENGTH,     tImgMat.rows);
			TIFFSetField(ptTiff, TIFFTAG_SAMPLESPERPIXEL, tImgMat.channels());
			TIFFSetField(ptTiff, TIFFTAG_BITSPERSAMPLE,   8);
			TIFFSetField(ptTiff, TIFFTAG_ORIENTATION,     ORIENTATION_TOPLEFT);
			TIFFSetField(ptTiff, TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);
			TIFFSetField(ptTiff, TIFFTAG_SAMPLEFORMAT,    SAMPLEFORMAT_UINT);
			TIFFSetField(ptTiff, TIFFTAG_COMPRESSION,     COMPRESSION_LZW);
			//TIFFSetField(ptTiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

			TIFFSetField(ptTiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_SEPARATED);

			//_cwprintf(L"%s 111: %d, %d\r\n", __FUNCTIONW__, tImgMat.channels(), tImgMat.depth());

			unsigned char* pucLineBuf = new unsigned char[tImgMat.channels() * tImgMat.cols];

			if (pucLineBuf != NULL) {
				std::vector<cv::Mat> tMatChnsVec;
				cv::split(tImgMat, tMatChnsVec);

				for (int y = 0; y < tImgMat.rows; y++) {
					for (int x = 0; x < tImgMat.cols; x++) {
						for (int j = 0; j < tImgMat.channels(); j++) {
							*(pucLineBuf + x * tImgMat.channels() + j) = tMatChnsVec[j].at<uchar>(y, x);
						}
					}

					TIFFWriteScanline(ptTiff, pucLineBuf, y);
				}

				delete[] pucLineBuf;
				bRet = true;
			}

			TIFFClose(ptTiff);
		}
	}

	return bRet;
}
