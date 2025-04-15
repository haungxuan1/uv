#include "pch.h"
#include "PQSetting.h"

namespace PQSetting
{
	namespace Dir
	{
		extern void Reg();

		extern void Unreg();

		extern const std::wstring &ExeDir();

		extern const std::wstring &MoldDir();
	}

	namespace Tif2Matrix
	{
		extern bool Tif2Mat(std::wstring szTifFile, cv::Mat& vTif);
	}

	const std::wstring &ExeDir()
	{
		return Dir::ExeDir();
	}

	const std::wstring &MoldDir()
	{
		return Dir::MoldDir();
	}

	bool Tif2Mat(std::wstring szTifFile, cv::Mat& vTif)
	{
		return Tif2Matrix::Tif2Mat(szTifFile, vTif);
	}

	void Reg()
	{
		Dir::Reg();
	}

	void Unreg()
	{
		Dir::Unreg();
	}
}
