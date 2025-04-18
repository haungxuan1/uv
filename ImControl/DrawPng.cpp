#include "pch.h"
#include "DrawPng.h"
#include <atlimage.h>

typedef unsigned char uchar;

using namespace std;

namespace DrawPng
{
	bool Png2CImage(CImage& image, wstring filename)
	{
		if (image.Load(filename.c_str()) != E_FAIL)
		{
			if (image.GetBPP() == 32)
			{
				for (int y = 0; y < image.GetHeight(); ++y)
				{
					for (int x = 0; x < image.GetWidth(); ++x)
					{
						uchar* pByte = (uchar*)image.GetPixelAddress(x, y);
						pByte[0] = pByte[0] * pByte[3] / 255;
						pByte[1] = pByte[1] * pByte[3] / 255;
						pByte[2] = pByte[2] * pByte[3] / 255;
					}
				}
			}
			return true;
		}
		return false;
	}

	void DrawPng(HDC pDC, CRect rc, wstring filename)
	{

		CImage nImage;
		if (Png2CImage(nImage, filename))
		{
			nImage.Draw(pDC, rc);
			nImage.Destroy();
		}
	}
}
