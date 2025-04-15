#include "pch.h"
#include "Helper.h"

using namespace cv;
using namespace std;

//// prt ͷ - 726 * 1200
//void PrtHeader(unsigned int uImagew, unsigned int uImageh, unsigned int uChannel, std::tuple<unsigned int, unsigned char*>& vPrtHeader)
//{
//	const static unsigned int uXResolution = 726;
//	const static unsigned int uYResolution = 1200;
//	const static unsigned char uCMYK[16] = { 0x59, 0x4d, 0x43, 0x4b };
//
//	auto& iSize = get<0>(vPrtHeader); auto& iPtr = get<1>(vPrtHeader);
//
//	iSize = 84; iPtr = new uchar[iSize];
//
//	uchar* lpUchar = iPtr;
//
//	uint* lpUint = (uint*)iPtr;
//
//	lpUint[0] = 0x58485942;// int	0x58485942
//	lpUint[1] = 0x4;		// int	0x4(�汾)
//	lpUint[2] = uImagew;	// int	ͼ���(����)
//	lpUint[3] = uImageh;   // int  ͼ���(����)
//	lpUint[4] = uChannel;	// int	ͼ����ɫ��	- ymck = 4 | ymckwv = 6
//	lpUint[5] = 1;			// int	byte/pix
//	lpUint[6] = uXResolution;// int  x �ֱ���
//	lpUint[7] = uYResolution;// int  y �ֱ���
//	lpUint[8] = 0;			  // int  ѹ��ģʽ 0
//	lpUint[9] = (uImagew * lpUint[5] + 31) / 32 * 4; // int  ÿ���ֽ���	= (imw * byte/pix + 31) / 32 * 4
//	lpUint[10] = 0;		  // int	˫���ʶ(0 or 1)
//	lpUint[11] = 8;//  0xffffffff;	// int	pass
//	lpUint[12] = 0;	// int  0
//	lpUchar[52] = 0;	// unsigned char ��չͨ��
//	lpUchar[53] = 2;	// unsigned char VSDmode
//	lpUchar[54] = 0;	// unsigned char ��ӡģʽ�� 0 �������� | 1 �ٶ�����
//	memcpy(lpUchar + 55, uCMYK, sizeof(uCMYK));// unsigned char[16] ͨ����ɫ��
//	lpUchar[71] = 0;// unsigned char ѹ����ʽ 0��ѹ��
//	lpUint[18] = 0;// int  Ԥ��ͼ�ֽ�
//	lpUchar[75] = 0;// unsigned char 0 ��Ԥ�� | 1 bmp
//	memset(lpUchar + 76, 0, 7 * sizeof(char));// unsigned char[7] ���� 
//}

// prt ͷ - 635 * 1200
void PrtHeader(unsigned int uImagew, unsigned int uImageh, unsigned int uChannel, unsigned int uBit, std::tuple<unsigned int, unsigned char*>& vPrtHeader)
{
	const static unsigned int uXResolution = 635;
	const static unsigned int uYResolution = 1200;
	static unsigned char uCMYK[16] = { 0x59, 0x4d, 0x43, 0x4b, 0x57, 0x56  };// Y // M / C // K // W // V

	auto& iSize = get<0>(vPrtHeader); auto& iPtr = get<1>(vPrtHeader);

	uCMYK[4] = (uChannel == 4 ? 0x00 : 0x57);
	uCMYK[5] = (uChannel == 4 ? 0x00 : 0x56); // ��չΪ6ͨ��

	iSize = 84; iPtr = new uchar[iSize];

	uchar* lpUchar = iPtr;

	uint* lpUint = (uint*)iPtr;

	lpUint[0] = 0x58485942;// int	0x58485942
	lpUint[1] = 0x4;		// int	0x4(�汾)
	lpUint[2] = uImagew;	// int	ͼ���(����)
	lpUint[3] = uImageh;   // int  ͼ���(����)
	lpUint[4] = uChannel;	// int	ͼ����ɫ��	- ymck = 4 | ymckwv = 6
	lpUint[5] = uBit;			// int	byte/pix
	lpUint[6] = uXResolution;// int  x �ֱ���
	lpUint[7] = uYResolution;// int  y �ֱ���
	lpUint[8] = 0;			  // int  ѹ��ģʽ 0
	lpUint[9] = (uImagew * lpUint[5] + 31) / 32 * 4; // int  ÿ���ֽ���	= (imw * byte/pix + 31) / 32 * 4
	lpUint[10] = 0;		  // int	˫���ʶ(0 or 1)
	lpUint[11] = 8;//  0xffffffff;	// int	pass
	lpUint[12] = 0;	// int  0
	lpUchar[52] = 0;	// unsigned char ��չͨ��
	lpUchar[53] = 2;	// unsigned char VSDmode
	lpUchar[54] = 0;	// unsigned char ��ӡģʽ�� 0 �������� | 1 �ٶ�����
	memcpy(lpUchar + 55, uCMYK, sizeof(uCMYK));// unsigned char[16] ͨ����ɫ��
	lpUchar[71] = 0;// unsigned char ѹ����ʽ 0��ѹ��
	lpUint[18] = 0;// int  Ԥ��ͼ�ֽ�
	lpUchar[75] = 0;// unsigned char 0 ��Ԥ�� | 1 bmp
	memset(lpUchar + 76, 0, 7 * sizeof(char));// unsigned char[7] ���� 
}

// �Ҷ�ͼ��תprt cmykͼ
// c ָ��cmyk����Ч��ɫ std::vector < int>{3} k = ��ɫ
void Gray2Cmyk(Mat& cImage, Mat& cChannel, vector<int> ch)
{
	int64 idx1 = 0, idx2 = 0;

	cChannel = Mat(Mat::zeros(cImage.rows, cImage.cols, CV_8UC4));

	for (int64 y = 0; y < cImage.rows; ++y)
	{
		for (int64 x = 0; x < cImage.cols; ++x)
		{
			idx1 = y * cImage.cols + x;

			for (auto& c : ch)
			{
				idx2 = (y * cImage.cols + x) * 4 + c;

				cChannel.data[idx2] = cImage.data[idx1];
			}
		}
	}
}

void Gray2Cmykwv(cv::Mat& cImage, cv::Mat& cChannel, std::vector<int> ch)
{
	int64 idx1 = 0, idx2 = 0;

	cChannel = Mat(Mat::zeros(cImage.rows, cImage.cols, CV_8UC(6)));

	for (int64 y = 0; y < cImage.rows; ++y)
	{
		for (int64 x = 0; x < cImage.cols; ++x)
		{
			idx1 = y * cImage.cols + x;

			for (auto& c : ch)
			{
				idx2 = (y * cImage.cols + x) * cChannel.channels() + c;

				cChannel.data[idx2] = cImage.data[idx1];
			}
		}
	}
}

// prt ͷ - 726 * 1200
//Mat Chart()
//{
//	uint64 w = 726, h = 1200; // 25.4mm
//
//	uint64 w1 = 6, h1 = 10;   // 0.2032mm
//
//	uint64 w2 = w - w1 * 2, h2 = h - h1 * 2;
//
//	uint64 sw = 121, sh = 200; // 4.2333mm
//
//	Mat cCHart(Mat::zeros(h2, w2, CV_8UC1));
//
//	cv::copyMakeBorder(cCHart, cCHart, h1, h1, w1, w1, BORDER_CONSTANT, cv::Scalar(0xff));
//
//	cv::copyMakeBorder(cCHart, cCHart, sh, sh, sw, sw, BORDER_CONSTANT, cv::Scalar(0x00)); // 29.6333mm
//	
//	return cCHart;
//}

// prt ͷ - 635 * 1200 // 30.52mm
Mat* Chart() 
{
	uint64 w = 635, h = 1200; 

	uint64 w1 = 8, h1 = 15;

	uint64 w2 = w - w1 * 2, h2 = h - h1 * 2;

	uint64 sw = 128, sh = 242;

	Mat *lpCHart = new Mat(Mat::zeros(h2, w2, CV_8UC1));

	cv::copyMakeBorder(*lpCHart, *lpCHart, h1, h1, w1, w1, BORDER_CONSTANT, cv::Scalar(0xff));

	cv::copyMakeBorder(*lpCHart, *lpCHart, sh, sh, sw, sw, BORDER_CONSTANT, cv::Scalar(0x00)); // 29.6333mm

	return lpCHart;
}

Mat* Chart(uint64 uChartInCol)
{
	Rect rtPos;

	Mat* lpChart = Chart();

	uint64 uCols = (uChartInCol * lpChart->cols + 31) / 32 * 32; 	// prt�п�Ϊ4��������matתprt�п�Ϊ1/8,��������ȡ32����

	Mat* lpLine = new Mat(Mat::zeros(lpChart->rows, uCols, CV_8UC1));

	for (uint64 i = 0; i < uChartInCol; ++i)
		lpChart->copyTo((*lpLine)(Rect(i * lpChart->cols, 0, lpChart->cols, lpChart->rows)));

	delete lpChart;

	return lpLine;
}

void Chart(uint64 uChartInCol, uint64 uChartInRow, cv::Mat*& lpChart)
{
	Mat* lpLine = Chart(uChartInCol);

	lpChart = new Mat(Mat::zeros(uChartInRow * lpLine->rows, lpLine->cols, CV_8UC1));

	for (int i = 0; i < uChartInRow; ++i)
		lpLine->copyTo((*lpChart)(Rect(0, i * lpLine->rows, lpLine->cols, lpLine->rows)));

	delete lpLine;
}
