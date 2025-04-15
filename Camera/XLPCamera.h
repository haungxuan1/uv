#pragma once
#include <tuple>
#include "opencv.hpp"

namespace XLPCamera
{
	void Reg();

	void Unreg();

	bool Open(const char* lpFriendlyName);

	bool Close();
}
