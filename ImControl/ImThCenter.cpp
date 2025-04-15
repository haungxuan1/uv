#include "pch.h"
#include "ImThCenter.h"
#include "PQMsg.h"

namespace ImThCenter
{
	CImThCenter::CImThCenter():
		m_uTermination{ 0, 0, 0xf }
	{
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, PrThread, (void*)this, 0, NULL));
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, BgThread, (void*)this, 0, NULL));
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, ScThread, (void*)this, 0, NULL));
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, RoThread, (void*)this, 0, NULL));
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, CnThread, (void*)this, 0, NULL));
	}

	CImThCenter::~CImThCenter()
	{
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, TeThread, (void*)this, 0, NULL));

		m_cTermination.wait();

		m_vLayer.clear();
	}

	bool CImThCenter::IsTermination(int nTh)
	{
		CSRLock lock(&m_lLock);

		unsigned char uTermination = (unsigned char)0x1 << nTh;

		return (m_uTermination[0] & uTermination);
	}

	void CImThCenter::Termination(int nTh)
	{
		bool bTermination = false;
		{
			CSWLock lock(&m_lLock);

			m_uTermination[1] |= ((unsigned char)0x1 << nTh);

			bTermination = (m_uTermination[1] == m_uTermination[2]);
		}
		bTermination ? m_cTermination.signal() : []() {}();
	}

	void CImThCenter::Termination()
	{
		CSWLock lock(&m_lLock);

		m_uTermination[0] = m_uTermination[2];
	}

	unsigned int __stdcall CImThCenter::TeThread(void* lpUserparam)
	{
		CImThCenter* lpThis = (CImThCenter*)lpUserparam;

		lpThis->Termination();

		lpThis->m_cContrast.signal();

		lpThis->m_cScaling.signal();

		lpThis->m_cRotating.signal();

		lpThis->m_cPreview.signal();

		lpThis->m_cBGRA.signal();

		return 0;
	}

	unsigned int __stdcall CImThCenter::CnThread(void* lpUserparam)
	{
		CImThCenter* lpThis = (CImThCenter*)lpUserparam;

		while (!lpThis->IsTermination(4))
		{
			ImLayer::tagLayer tCn;

			if (!lpThis->Contrast(tCn)) { lpThis->m_cContrast.wait(); continue; }

			if (!lpThis->OnContrast(tCn)) continue;

			lpThis->SyncContrast(tCn);

			lpThis->Render(tCn.nSign);

		}
		lpThis->Termination(4);

		return 0;
	}
	/***********************************************************************************/
	unsigned int __stdcall CImThCenter::RoThread(void* lpUserparam)
	{
		CImThCenter* lpThis = (CImThCenter*)lpUserparam;

		while (!lpThis->IsTermination(2))
		{
			ImLayer::tagLayer tRotating;

			if (!lpThis->Rotating(tRotating)) { lpThis->m_cRotating.wait(); continue; }

			if (!lpThis->OnRotating(tRotating)) continue;

			lpThis->SyncRotating(tRotating);

			lpThis->m_cScaling.signal();
		}
		lpThis->Termination(2);

		return 0;
	}

	bool CImThCenter::Rotating(ImLayer::tagLayer& tParam)
	{
		CSRLock lock(&m_lLock);

		for (auto& iParam : m_vLayer)
		{
			if (iParam.nTimer[ImLayer::tagLayer::eTT_Rotate] < iParam.nTimer[ImLayer::tagLayer::eTT_Image])
			{
				iParam.GetRotateParam(tParam);

				return true;
			}
		}
		return false;
	}

	bool CImThCenter::OnRotating(ImLayer::tagLayer& tParam)
	{
		auto& iImage = tParam.cImage;

		if (iImage.Empty()) return false;

		auto& iRotating = tParam.cRotated;

		iRotating = ImPtr::CImPtr(new Mat);

		auto iAngle = tParam.Angle();

		Size2f szImage(iImage.Get()->cols, iImage.Get()->rows);

		Point2f ptCenter(0.5 * szImage.width, 0.5 * szImage.height);

		Rect2f rtBound = RotatedRect(ptCenter, szImage, iAngle).boundingRect();

		Mat M = getRotationMatrix2D(ptCenter, iAngle, 1.0);

		M.at<double>(0, 2) += (0.5 * rtBound.width - ptCenter.x);

		M.at<double>(1, 2) += (0.5 * rtBound.height - ptCenter.y);

		warpAffine(*iImage.Get(), *iRotating.Get(), M, rtBound.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0, 0));

		return true;
	}

	void CImThCenter::SyncRotating(ImLayer::tagLayer& tParam)
	{
		CSWLock lock(&m_lLock);

		auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), tParam.nSign, [](const ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

		if (iSign == m_vLayer.end()) return;

		iSign->SyncRotate(tParam);
	}
	/***********************************************************************************/
	unsigned int __stdcall CImThCenter::ScThread(void* lpUserparam)
	{
		CImThCenter* lpThis = (CImThCenter*)lpUserparam;

		while (!lpThis->IsTermination(3))
		{
			ImLayer::tagLayer tScaling;

			if (!lpThis->Scaling(tScaling)) { lpThis->m_cScaling.wait(); continue; }

			if (!lpThis->OnScaling(tScaling)) continue;

			lpThis->SyncScaling(tScaling);

			lpThis->m_cBGRA.signal();
		}
		lpThis->Termination(3);

		return 0;
	}

	bool CImThCenter::Scaling(ImLayer::tagLayer& tParam)
	{
		CSRLock lock(&m_lLock);

		for (auto& iParam : m_vLayer)
		{
			if (iParam.nTimer[ImLayer::tagLayer::eTT_Scale] < iParam.nTimer[ImLayer::tagLayer::eTT_Rotate])
			{
				iParam.GetScaleParam(tParam);

				return true;
			}
		}
		return false;
	}

	bool CImThCenter::OnScaling(ImLayer::tagLayer& tParam)
	{
		auto& iRotated = tParam.cRotated;

		if (iRotated.Empty()) return false;

		auto& iScaling = tParam.cScaled;

		iScaling = ImPtr::CImPtr(new Mat);

		auto isx = tParam.ScalingFactor()[0];

		auto isy = tParam.ScalingFactor()[1];

		Size2f szRotate(iRotated.Get()->cols, iRotated.Get()->rows);

		Size2f szScale(szRotate.width * isx, szRotate.height * isy);

		Mat M(Mat::zeros(2, 3, CV_64F));

		M.at<double>(0, 0) = isx;

		M.at<double>(1, 1) = isy;

		warpAffine(*iRotated.Get(), *iScaling.Get(), M, szScale, INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0, 0));

		return true;
	}

	void CImThCenter::SyncScaling(ImLayer::tagLayer& tParam)
	{
		CSWLock lock(&m_lLock);

		auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), tParam.nSign, [](const ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

		if (iSign == m_vLayer.end()) return;

		iSign->SyncScale(tParam);
	}
	bool CImThCenter::Contrast(ImLayer::tagLayer& tParam)
	{
		CSRLock lock(&m_lLock);

		for (auto& iParam : m_vLayer)
		{
			if (iParam.nTimer[ImLayer::tagLayer::eTT_Contrast] < iParam.nTimer[ImLayer::tagLayer::eTT_Scale])
			{
				iParam.GetContrastParam(tParam);

				return true;
			}
		}
		return false;
	}
	bool CImThCenter::OnContrast(ImLayer::tagLayer& tParam)
	{
		auto& iBGRA = tParam.cBRGA;

		if (iBGRA.Empty()) return false;

		auto& iContrast = tParam.cContrast;

		iContrast = ImPtr::CImPtr(new Mat);

		return true;
	}
	void CImThCenter::SyncContrast(ImLayer::tagLayer& tParam)
	{
		CSWLock lock(&m_lLock);

		auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), tParam.nSign, [](const ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

		if (iSign == m_vLayer.end()) return;

		iSign->SyncContrast(tParam);
	}
	/***********************************************************************************/
	unsigned int __stdcall CImThCenter::BgThread(void* lpUserparam)
	{
		CImThCenter* lpThis = (CImThCenter*)lpUserparam;

		while (!lpThis->IsTermination(0))
		{
			ImLayer::tagLayer tImage2BGRA;

			if (!lpThis->Image2BGRA(tImage2BGRA)) { lpThis->m_cBGRA.wait(); continue; }

			if (!lpThis->OnImage2BGRA(tImage2BGRA)) continue;

			lpThis->SyncBGRA(tImage2BGRA);

			lpThis->m_cPreview.signal();
		}
		lpThis->Termination(0);

		return 0;
	}

	bool CImThCenter::Image2BGRA(ImLayer::tagLayer& tParam)
	{
		CSRLock lock(&m_lLock);

		for (auto& iParam : m_vLayer)
		{
			if (iParam.nTimer[ImLayer::tagLayer::eTT_BRGA] < iParam.nTimer[ImLayer::tagLayer::eTT_Scale])
			{
				iParam.GetBGRAParam(tParam);

				return true;
			}
		}
		return false;
	}

	bool CImThCenter::OnImage2BGRA(ImLayer::tagLayer& tParam)
	{
		auto& iScaled = tParam.cScaled;

		if (iScaled.Empty()) return false;

		switch (iScaled.Get()->channels())
		{
		case 1:
			OnGray2BGRA(tParam);
			break;

		case 3:
			OnBGR2BGRA(tParam);
			break;

		case 4:
			tParam.bTIFF ? OnCMYK2BGRA(tParam) : OnBGRA2BGRA(tParam);
			break;

		default:
			OnCMKYW2BGRA(tParam);
			break;
		}
		return true;
	}

	void CImThCenter::OnGray2BGRA(ImLayer::tagLayer& tParam)
	{
		auto& iGray = tParam.cScaled;

		auto& iBGRA = tParam.cBRGA;

		iBGRA = ImPtr::CImPtr(new Mat);

		cvtColor(*iGray.Get(), *iBGRA.Get(), cv::COLOR_GRAY2BGRA);
	}

	void CImThCenter::OnBGR2BGRA(ImLayer::tagLayer& tParam)
	{
		auto& iBGR = tParam.cScaled;

		auto& iBGRA = tParam.cBRGA;

		iBGRA = ImPtr::CImPtr(new Mat);

		cvtColor(*iBGR.Get(), *iBGRA.Get(), cv::COLOR_BGR2BGRA);
	}

	void CImThCenter::OnCMYK2BGRA(ImLayer::tagLayer& tParam)
	{
		auto& iCMYK = tParam.cScaled;

		auto& iBGRA = tParam.cBRGA;

		iBGRA = ImPtr::CImPtr(new Mat);

		*iBGRA.Get() = iCMYK.Get()->clone();
	}

	void CImThCenter::OnBGRA2BGRA(ImLayer::tagLayer& tParam)
	{
		auto& iBGRA = tParam.cScaled;

		auto& iBGRA2 = tParam.cBRGA;

		iBGRA2 = ImPtr::CImPtr(new Mat);

		*iBGRA2.Get() = iBGRA.Get()->clone();
	}

	void CImThCenter::OnCMKYW2BGRA(ImLayer::tagLayer& tParam)
	{
		auto& iCMKYW = tParam.cScaled;

		auto& iBGRA = tParam.cBRGA;

		iBGRA = ImPtr::CImPtr(new Mat);

		vector<Mat> vChannel(iCMKYW.Get()->channels());

		split(*iCMKYW.Get(), vChannel);

		vChannel.resize(4);

		merge(vChannel, *iBGRA.Get()); // opencv中 merge 可能出现bug
	}

	void CImThCenter::SyncBGRA(ImLayer::tagLayer& tParam)
	{
		CSWLock lock(&m_lLock);

		auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), tParam.nSign,
			[](const ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

		if (iSign == m_vLayer.end()) return;

		iSign->SyncBGRA(tParam);
	}
	/***********************************************************************************/
	unsigned int __stdcall CImThCenter::PrThread(void* lpUserparam)
	{
		CImThCenter* lpThis = (CImThCenter*)lpUserparam;

		while (!lpThis->IsTermination(1))
		{
			ImLayer::tagLayer tPreview;

			if (!lpThis->Preview(tPreview)) { lpThis->m_cPreview.wait(); continue; }

			if (!lpThis->OnPreview(tPreview))continue;

			lpThis->SyncPreview(tPreview);

			lpThis->m_cContrast.signal();
		}
		lpThis->Termination(1);

		return 0;
	}

	bool CImThCenter::Preview(ImLayer::tagLayer& tParam)
	{
		CSRLock lock(&m_lLock);

		for (auto& iParam : m_vLayer)
		{
			if (iParam.nTimer[ImLayer::tagLayer::eTT_Preview] < iParam.nTimer[ImLayer::tagLayer::eTT_BRGA])
			{
				iParam.GetPreviewParam(tParam);

				return true;
			}
		}
		return false;
	}

	bool CImThCenter::OnPreview(ImLayer::tagLayer& tParam)
	{
		auto& iBGRA = tParam.cBRGA;

		if (iBGRA.Empty()) return false;

		auto& iPreview = tParam.cPreview;

		iPreview = ImPtr::CImPtr(new Mat);

		double w1 = 0.0, h1 = 0.0;	PrSize(w1, h1);

		double w2 = iBGRA.Get()->cols, h2 = iBGRA.Get()->rows;

		double dx = w1 / w2, dy = h1 / h2;

		double dScaling = min(dx, dy);

		Size szPre(dScaling * iBGRA.Get()->cols, dScaling * iBGRA.Get()->rows);

		szPre.width = max(1, szPre.width), szPre.height = max(1, szPre.height);

		resize(*iBGRA.Get(), *iPreview.Get(), szPre);

		double w3 = iPreview.Get()->cols, h3 = iPreview.Get()->rows;

		int cx = w1 - w3, cy = h1 - h3;

		int l = 0.5 * cx, r = cx - l, t = 0.5 * cy, b = cy - t;

		if (cx | cy)	copyMakeBorder(*iPreview.Get(), *iPreview.Get(), t, b, l, r, BORDER_CONSTANT, cv::Scalar(0, 0, 0, 0));

		return true;
	}

	void CImThCenter::SyncPreview(ImLayer::tagLayer& tParam)
	{
		CSWLock lock(&m_lLock);

		auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), tParam.nSign, [](const ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

		if (iSign == m_vLayer.end()) return;

		iSign->SyncPreview(tParam);
	}
	/***********************************************************************************/
	bool CImThCenter::Empty()
	{
		CSRLock lock(&m_lLock);

		return m_vLayer.empty();
	}
}