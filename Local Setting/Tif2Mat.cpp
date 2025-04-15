#include "pch.h"
#include "tiffio.h"
#include "opencv.hpp"

namespace PQSetting
{
	namespace Tif2Matrix
	{
		static TIFF* g_lpTif = nullptr;

		static uint32 g_uLength = 0;

		static uint32 g_uWidth = 0;

		static uint32 g_uRowSperstrip = 0;

		static uint32 g_uStrips = 0;

		static uint32* g_lpBuffer = nullptr;

		int GetTifParam()
		{
			g_uStrips = TIFFNumberOfStrips(g_lpTif);

			if (!!!TIFFGetField(g_lpTif, TIFFTAG_IMAGEWIDTH, &g_uWidth)) return 0;

			if (!!!TIFFGetField(g_lpTif, TIFFTAG_IMAGELENGTH, &g_uLength)) return 0;

			return (!!TIFFGetField(g_lpTif, TIFFTAG_ROWSPERSTRIP, &g_uRowSperstrip));
		}

		void BotLeft2TopLeft(uint32* lpTmp, tmsize_t nStrip)
		{
			unsigned int uMaxStripIndex = g_uStrips - 1;

			unsigned int uRowInStrip[2] = { 
				g_uRowSperstrip, 
				g_uLength % g_uRowSperstrip ? (g_uLength - uMaxStripIndex * g_uRowSperstrip) : g_uRowSperstrip };

			auto& iRowInStrip = uRowInStrip[nStrip == uMaxStripIndex];

			unsigned int uRow = floor((double)iRowInStrip * 0.5);

			for (int r = 0; r < uRow; ++r) // ORIENTATION_BOTLEFT -> ORIENTATION_TOPLEFT
			{
				int oy1 = r * g_uWidth;

				int oy2 = (iRowInStrip - r - 1) * g_uWidth;

				memcpy(g_lpBuffer, lpTmp + oy1, sizeof(uint32) * g_uWidth);

				memcpy(lpTmp + oy1, lpTmp + oy2, sizeof(uint32) * g_uWidth);

				memcpy(lpTmp + oy2, g_lpBuffer, sizeof(uint32) * g_uWidth);
			}
		}

		bool Tif2Mat(std::wstring szTifFile, cv::Mat& vTif)
		{
			TIFFSetErrorHandler([](const char*, const char*, va_list) {});

			g_lpTif = TIFFOpenW(szTifFile.c_str(), "rb");

			if (!!!g_lpTif) return false;

			if (!!!GetTifParam())return false;

			vTif.create(g_uLength, g_uWidth, CV_8UC4);

			for (tmsize_t i = 0; i < g_uStrips; ++i)
			{
				uint32* lpData = (uint32 *)vTif.data;

				TIFFReadEncodedStrip(g_lpTif, i, lpData + i * g_uWidth * g_uRowSperstrip, g_uWidth * g_uRowSperstrip * 4);
			}

			TIFFClose(g_lpTif);

			return true;
		}
	}
}