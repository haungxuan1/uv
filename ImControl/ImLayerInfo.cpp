#include "pch.h"
#include "ImLayerInfo.h"

namespace ImLayer
{
	tagLayer::tagLayer() :
		nSign(0), 
		lpUserParam(nullptr),
		bEnable(true), bFocus(false), bBackground(false),
		x{ 0.0, 0.0, 0.0 }, y{ 0.0, 0.0, 0.0 },
		nTop(0), nLeft(0), nWidth(0), nHeight(0),
		dRotated(0.0), dRotating{ 0.0, 0.0, 0.0 },
		dScaled{ 0.0, 0.0 }, dScaling{ 1.0, 1.0, 1.0, 1.0, 0.0 },nScaledSize{ 0, 0 },
		rName(L""), dOpacity(1.0), bTIFF(false), bAssociated(true),
		bOpt{ true, false, false, false },
		nTimer{ 0, 0, 0, 0, 0 ,0 }
	{

	}

	void tagLayer::Opt(int nOpt)
	{
		bOpt.fill(false);

		bOpt[nOpt] = true;
	}

	int tagLayer::CurOpt()
	{
		auto iOpt = find(bOpt.begin(), bOpt.end(), true);

		if (iOpt == bOpt.end()) return 0;

		return iOpt - bOpt.begin();
	}

	Point2d tagLayer::Center()
	{
		if (x[2] != 0.0 && y[2] != 0.0) return { x[0], y[0] };

		return { x[0] + x[1], y[0] + y[1] };
	}

	double tagLayer::Angle()
	{
		if (dRotating[2] == 0.0) return dRotating[0];

		return dRotating[0] + dRotating[1];
	}

	array<double, 2> tagLayer::ScalingFactor()
	{
		if (dScaling[4] == 0.0) return { dScaling[0], dScaling[1] };

		return { dScaling[0] * dScaling[2], dScaling[1] * dScaling[3] };
	}

	void tagLayer::Create()
	{
		nSign = Sign();

		nTimer[eTT_Image] = cImage.Timer();
	}
	/********************************************************************************/
	void tagLayer::GetBGRAParam(tagLayer& other)
	{
		other.nSign = nSign;

		other.cScaled = cScaled;

		other.bTIFF = bTIFF;
	}

	void tagLayer::SyncBGRA(tagLayer& other)
	{
		if (other.cBRGA.Timer() <= cBRGA.Timer()) return;

		cBRGA = other.cBRGA;

		nTimer[eTT_BRGA] = cBRGA.Timer();
	}
	/********************************************************************************/
	void tagLayer::GetPreviewParam(tagLayer& other)
	{
		other.nSign = nSign;

		other.cBRGA = cBRGA;
	}

	void tagLayer::SyncPreview(tagLayer& other)
	{
		if (other.cPreview.Timer() <= cPreview.Timer()) return;

		cPreview = other.cPreview;

		nTimer[eTT_Preview] = cPreview.Timer();
	}
	/********************************************************************************/
	void tagLayer::GetRotateParam(tagLayer& other)
	{
		other.nSign = nSign;

		other.cImage = cImage;

		other.dRotating = dRotating;
	}

	void tagLayer::SyncRotate(tagLayer& other)
	{
		if (other.cRotated.Timer() <= cRotated.Timer()) return;

		cRotated = other.cRotated;

		dRotated = other.Angle();

		nTimer[eTT_Rotate] = cRotated.Timer();
	}
	/********************************************************************************/
	void tagLayer::GetScaleParam(tagLayer& other)
	{
		other.nSign = nSign;

		other.cRotated = cRotated;

		other.dScaling = dScaling;
	}

	void tagLayer::GetContrastParam(tagLayer& other)
	{
		other.nSign = nSign;

		other.cRotated = cRotated;

		other.cScaled = cScaled;

		other.cBRGA = cBRGA;

		other.dContrastRotio = dContrastRotio;
	}

	void tagLayer::SyncScale(tagLayer& other)
	{
		if (other.cScaled.Timer() <= cScaled.Timer()) return;

		cScaled = other.cScaled;

		dScaled = ScalingFactor();

		nTimer[eTT_Scale] = cScaled.Timer();

		nWidth = cScaled.Get()->cols;

		nHeight = cScaled.Get()->rows;

		nLeft = Center().x - 0.5 * nWidth;

		nTop = Center().y - 0.5 * nHeight;

		if (dScaling[4] != 0.0) return; //  Û±ÍÕœ∂Ø

		nScaledSize = { nWidth, nHeight };
	}
	void tagLayer::SyncContrast(tagLayer& other)
	{
		if (other.cContrast.Timer() <= cContrast.Timer()) return;

		cContrast = other.cContrast;

		dContrastRotio = other.dContrastRotio;

		nTimer[eTT_Contrast] = cContrast.Timer();
	}
	/********************************************************************************/
	void tagLayer::GetSyncInfo(tagLayer& other)
	{
		other.nSign = nSign;

		other.bEnable = bEnable;

		other.bFocus = bFocus;

		other.bBackground = bBackground;

		other.nTop = nTop;

		other.nLeft = nLeft;

		other.nWidth = nWidth;

		other.nHeight = nHeight;

		other.dRotated = dRotated;

		other.dScaled = dScaled;

		other.rName = rName;

		other.dOpacity = dOpacity;

		other.bAssociated = bAssociated;

		other.bOpt = bOpt;

		other.cPreview = cPreview;

		other.cBRGA = cBRGA;

		other.dContrastRotio = dContrastRotio;
	}

	void tagLayer::GetRenderParam(tagLayer& other)
	{
		other.nSign = nSign;

		other.bEnable = bEnable;

		other.cBRGA = cBRGA;

		other.nTop = nTop;

		other.nLeft = nLeft;

		other.nWidth = nWidth;

		other.nHeight = nHeight;

		other.dOpacity = dOpacity;

		other.dContrastRotio = dContrastRotio;
	}

	void tagLayer::LeftChanged(int& _nLeft)
	{
		nLeft = _nLeft;

		x[0] = nLeft + 0.5 * nWidth;

		x[1] = x[2] = 0;
	}

	void tagLayer::TopChanged(int& _nTop)
	{
		nTop = _nTop;

		y[0] = nTop + 0.5 * nHeight;

		y[1] = y[2] = 0;
	}

	void tagLayer::WidthChanged(int& _nWidth)
	{
		double dSx = ((double)_nWidth / nWidth); // scaling = hro[0]*[2]  ver[1]*[3] 

		dScaling[0] *= dSx; dScaling[2] = 1.0;

		dScaling[1] = (bAssociated ? dScaling[0] : dScaling[1]);

		dScaling[3] = (bAssociated ? dScaling[2] : dScaling[3]);

		dScaling[4] = 0.0;

		nTimer[ImLayer::tagLayer::eTT_Scale] = 0;
	}

	void tagLayer::HeightChanged(int& _nHeight)
	{
		double dSy = ((double)_nHeight / nHeight);

		dScaling[1] *= dSy; dScaling[3] = 1.0;

		dScaling[0] = (bAssociated ? dScaling[1] : dScaling[0]);

		dScaling[2] = (bAssociated ? dScaling[3] : dScaling[2]);

		dScaling[4] = 0.0;

		nTimer[ImLayer::tagLayer::eTT_Scale] = 0;
	}

	void tagLayer::ScalChanged(double dHor, double dVer, bool bHor)
	{
		dScaling[0] = (bHor ? dHor : (bAssociated && !bHor ? dVer : dScaling[0]));

		dScaling[1] = (!bHor ? dVer : (bAssociated && bHor ? dHor : dScaling[1]));

		dScaling[2] = dScaling[3] = 1.0;

		dScaling[4] = 0.0;

		nTimer[ImLayer::tagLayer::eTT_Scale] = 0;
	}

	void tagLayer::AngleChanged(double dAngle)
	{
		dRotating[0] = dAngle;

		dRotating[1] = 0.0;

		dRotating[2] = 0.0;

		nTimer[ImLayer::tagLayer::eTT_Rotate] = 0;
	}

	void tagLayer::MoveEnable()
	{
		Opt(eOT_Move);
	}

	void tagLayer::ScaleEnable()
	{
		Opt(eOT_Scale);
	}

	void tagLayer::RotateEnable()
	{
		Opt(eOT_Rotate);
	}

	void tagLayer::SelecteEnable()
	{
		Opt(eOT_Selecte);
	}

	void tagLayer::Moving(int& _ox, int& _oy)
	{
		x[2] = y[2] = 1.0;

		x[1] = _ox; y[1] = _oy;

		nLeft = Center().x - 0.5 * nWidth;

		nTop = Center().y - 0.5 * nHeight;
	}

	void tagLayer::Move(int& _ox, int& _oy)
	{
		x[2] = y[2] = 0.0;

		x[1] = y[1] = 0.0;

		x[0] += _ox, y[0] += _oy;

		nLeft = Center().x - 0.5 * nWidth;

		nTop = Center().y - 0.5 * nHeight;
	}

	void tagLayer::Scaling(int& _ox, int& _oy)
	{
		Point2d ptSize1(nScaledSize[0], nScaledSize[1]);

		Point2d ptSize2(max(ptSize1.x + 2.0 * _ox, 1.0), max(ptSize1.y + 2.0 * _oy, 1.0));

		Point2d ptScale(ptSize2.x / ptSize1.x, ptSize2.y / ptSize1.y);

		dScaling[2] = ptScale.x, dScaling[3] = ptScale.y;

		dScaling[4] = 1.0;

		nTimer[ImLayer::tagLayer::eTT_Scale] = 0;
	}

	void tagLayer::Scale(int& _ox, int& _oy)
	{
		Point2d ptSize1(nScaledSize[0], nScaledSize[1]);

		Point2d ptSize2(max(ptSize1.x + 2.0 * _ox, 1.0), max(ptSize1.y + 2.0 * _oy, 1.0));

		Point2d ptScale(ptSize2.x / ptSize1.x, ptSize2.y / ptSize1.y);

		dScaling[0] *= ptScale.x, dScaling[1] *= ptScale.y;

		dScaling[2] = dScaling[3] = 1.0;

		dScaling[4] = 0.0;

		nTimer[ImLayer::tagLayer::eTT_Scale] = 0;
	}

	void tagLayer::Scale(double& dScale)
	{
		dScaling[0] *= dScale;

		dScaling[1] *= dScale;

		dScaling[2] = dScaling[3] = 1.0;

		dScaling[4] = 0.0;

		nTimer[ImLayer::tagLayer::eTT_Scale] = 0;
	}

	void tagLayer::ScaleEx(double& dScale)
	{
		x[0] *= dScale;

		y[0] *= dScale;

		x[1] = y[1] = 0.0;

		x[2] = y[2] = 0.0;


		dScaling[0] *= dScale;

		dScaling[1] *= dScale;

		dScaling[2] = dScaling[3] = 1.0;

		dScaling[4] = 0.0;

		nTimer[ImLayer::tagLayer::eTT_Scale] = 0;
	}

	void tagLayer::Rotating(double dAngle)
	{
		dRotating[1] = dAngle;

		dRotating[2] = 1.0;

		nTimer[ImLayer::tagLayer::eTT_Rotate] = 0;
	}

	void tagLayer::Rotate(double dAngle)
	{
		dRotating[0] += dAngle;

		dRotating[1] = 0.0;

		dRotating[2] = 0.0;

		nTimer[ImLayer::tagLayer::eTT_Rotate] = 0;
	}

	void tagLayer::Contrast(double dAngle)
	{
		dContrastRotio = dAngle;

		nTimer[ImLayer::tagLayer::eTT_Contrast] = 0;
	}

	long long tagLayer::Sign()
	{
		LARGE_INTEGER tTimer = { 0 };

		QueryPerformanceCounter(&tTimer);

		return tTimer.QuadPart;
	}
}