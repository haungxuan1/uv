#include "pch.h"
#include "Mat2Prt.h"
#include "PQSetting.h"
#include "Log.h"
#include "PQMsg.h"

CMat2Prt::CMat2Prt():
	m_bThread(false), m_bTermination(false), 
	m_lpHandle1bit(nullptr), m_lpHandle2bit(nullptr), m_fMat2Prt1bit(nullptr), m_fMat2Prt2bit(nullptr)
{
	m_vThParam = make_tuple(
		__FUNCTION__,
		this,
		&m_lLock,
		&m_cPrtMatrix,
		&m_cTermination,
		&m_bTermination,
		&m_vParams,
		bind(&CMat2Prt::OnMat2Prt, this, placeholders::_1));
}

CMat2Prt::~CMat2Prt()
{

}

bool CMat2Prt::Reg()
{
	if (!CPrtTCP::Reg()) return false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	Restart(m_bTermination, m_lLock);

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle);

	return (m_bThread = (!!lpHandle));
}

void CMat2Prt::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CPrtTCP::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CMat2Prt::AllocateMmemory()
{
	CPrtTCP::AllocateMmemory();

	wstring strPrtDll1bit = PQSetting::ExeDir() + L"DllErrDiffu1bit.dll";

	wstring strPrtDll2bit = PQSetting::ExeDir() + L"DllErrDiffu2bit.dll";

	m_lpHandle1bit = LoadLibrary(strPrtDll1bit.c_str());

	m_lpHandle2bit = LoadLibrary(strPrtDll2bit.c_str());

	m_fMat2Prt1bit = (fMat2Prt1bit)GetProcAddress(m_lpHandle1bit, "error_diffusion_calibrate16");// 年前打印 1bit

	m_fMat2Prt2bit = (fMat2Prt2bit)GetProcAddress(m_lpHandle2bit, "error_diffusion16"); // // 新dll 2bit
}

void CMat2Prt::DeallocateMemory()
{
	for (auto& iParam : m_vParams) 
	{ 
		bool* lpFinish = (bool*)get<ePI_UserParam>(iParam);
		
		delete lpFinish; 
	}

	m_vParams.clear();

	m_lpHandle1bit ? FreeLibrary(m_lpHandle1bit) : []()->BOOL {return 0; }();	m_lpHandle1bit = nullptr;

	m_lpHandle2bit ? FreeLibrary(m_lpHandle2bit) : []()->BOOL {return 0; }();	m_lpHandle2bit = nullptr;

	CPrtTCP::DeallocateMemory();
}

void CMat2Prt::Mat2Prt(vParam iParam)
{	
	Pushback(iParam, m_vParams, m_lLock);

	m_cPrtMatrix.signal();
}

void CMat2Prt::OnMat2Prt(vParam& iParam)
{
	unsigned int uSize = 0;

	unsigned short* lp16U = nullptr;

	auto& iFleece = get<ePI_Fleece>(iParam);

	bool* lpFinish = (bool*)get<ePI_UserParam>(iParam);

	auto& iMatrix = get<ePI_ImPtr>(iParam);

	M8U2M16U(*iMatrix.Get(), lp16U, uSize);

	m_b1bitDll ? m_fMat2Prt1bit(lp16U, iMatrix.Get()->channels(), iMatrix.Get()->rows, iMatrix.Get()->cols, 0, nullptr, 0, 0, 0) : []() {}();// 年前打印 1bit

	m_b1bitDll ? []() {}() : m_fMat2Prt2bit(lp16U, iMatrix.Get()->channels(), iMatrix.Get()->rows, iMatrix.Get()->cols);// 新dll 2bit

	cv::Mat v8U;

	M16U2M8U(lp16U, v8U, *iMatrix.Get());

	cv::Rect rtPrt(0, 0, 0, v8U.rows);

	rtPrt.width = (m_b1bitDll ? ((v8U.cols + 15) / 16) : ((v8U.cols + 7) / 8));

	cv::Mat vPrt = v8U(rtPrt).clone();

	Matrix2Buffer(vPrt, (unsigned char*)lp16U, uSize);

	iMatrix = ImPtr::CImPtr(new Mat(vPrt.rows, vPrt.cols, vPrt.type()));

	memcpy(iMatrix.Get()->data, lp16U, sizeof(unsigned char) * uSize);

	get<ePI_UserParam>(iParam) = new array<unsigned int, 3>{*lpFinish, 0, uSize};

	delete lpFinish; delete[] lp16U;

	get<ePI_Timer>(iParam).push_back({ __FUNCTION__, clock() });

	iFleece ? ::PostMessage(m_lpInitiator->GetSafeHwnd(), PQM_FLEECE_RESTRICT, 0, (LPARAM)new vParam(iParam))
		: [&iParam, this]()->BOOL { CPrtTCP::TCP(iParam); return 0; }();

//	CPrtTCP::TCP(iParam);
}

void CMat2Prt::M8U2M16U(cv::Mat& v8U, unsigned short*& lp16U, unsigned int& uSize)
{
	const int& h = v8U.rows, & w = v8U.cols, & c = v8U.channels();

	unsigned long long uArea = (unsigned long long)h * w;

	unsigned long long uBytes = (unsigned long long)uArea * c;

	lp16U = new unsigned short[uBytes]; uSize = uBytes; // unsigned short -> unsigned char

	std::vector<cv::Mat> vC8U(c);

	cv::split(v8U, vC8U);

	for (unsigned long long i = 0; i < c; ++i)
	{
		auto& iC8U = vC8U[i];

		unsigned long long uOs = i * uArea;

		for (unsigned long long idx = 0; idx < uArea; ++idx)
		{
			lp16U[uOs + idx] = ((unsigned short)iC8U.data[idx] << 8 | iC8U.data[idx]);
		}
	}
}

void CMat2Prt::M16U2M8U(unsigned short*& lp16U, cv::Mat& v8U, cv::Mat& vImage)
{
	const int& h = vImage.rows, & w = vImage.cols, & ch = vImage.channels();

	unsigned long long uBytes = (unsigned long long)h * w;

	v8U = Mat(Mat::zeros(h, w * 2, CV_8UC(ch)));

	for (long long y = 0; y < h; ++y)
	{
		for (long long x = 0; x < w; ++x)
		{
			for (long long c = 0; c < ch; ++c)
			{
				long long idx1 = (c * h + y) * w + x; // (c * h + y) * w + x

				long long idx2 = (y * w * 2 + x * 2 + 0) * ch + c; // (y * w + x) * ch + c

				long long idx3 = (y * w * 2 + x * 2 + 1) * ch + c;

				v8U.data[idx2] = (lp16U[idx1] & 0xff);

				v8U.data[idx3] = ((lp16U[idx1] >> 8) & 0xff);
			}
		}
	} // merge 源码层bug
}

void CMat2Prt::Swap(cv::Mat& a, cv::Mat& b)
{
	cv::Mat cTmp = a.clone();

	a = b.clone();

	b = cTmp.clone();
}

void CMat2Prt::Matrix2Buffer(cv::Mat& vMatrix, unsigned char* lpBuffer, unsigned int& uSize)
{
	auto w = vMatrix.cols, h = vMatrix.rows, ch = vMatrix.channels();

	uSize = w * h * ch;

	std::vector<cv::Mat> vChannel(ch);

	cv::split(vMatrix, vChannel); // cmyk

	Swap(vChannel[0], vChannel[2]); // cmyk -> ymck

	for (unsigned long long y = 0; y < vMatrix.rows; ++y)
	{
		for (unsigned long long x = 0; x < vMatrix.cols; ++x)
		{
			for (unsigned long long c = 0; c < ch; ++c)
			{
				unsigned long long uImIndex = y * vMatrix.cols + x;

				unsigned long long uPrtIndex =
					y * vMatrix.cols * ch +
					c * vMatrix.cols +
					x;
				lpBuffer[uPrtIndex] = vChannel[c].data[uImIndex];
			}
		}
	}
}