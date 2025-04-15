#include "pch.h"
#include "ImCenter.h"
#include "PQMsg.h"
#include "CImCtrl.h"

namespace ImCenter
{
	CImCenter::CImCenter():
		m_lpHandle(nullptr),
		m_vPreviewSize{ 0, 0 },
		m_nFocus(-1), m_nBackground(-1)
	{

	}

	CImCenter::~CImCenter()
	{

	}

	void CImCenter::CreateLayer(ImLayer::tagLayer& tInfo)
	{
		tInfo.Create();

		Push(tInfo);

		SyncLayerType(tInfo);

		m_cRotating.signal();

		::SendMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_CREATE, 0, (LPARAM)tInfo.nSign);
	}

	void CImCenter::SyncLayerType(ImLayer::tagLayer& tInfo)
	{
		m_nFocus = (tInfo.bFocus ? tInfo.nSign : m_nFocus);

		m_nBackground = (tInfo.bBackground ? tInfo.nSign : m_nBackground);
	}

	void CImCenter::DestroyLayer(long long nSign)
	{
		Pop(nSign);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_DESTROY, 0, (LPARAM)nSign);
	}

	void CImCenter::ClearLayer()
	{
		Clear();
	}

	void CImCenter::GetSign(vector<long long>& vSign)
	{
		Sign(vSign);
	}

	void CImCenter::Sign(vector<long long>& vSign)
	{
		CSRLock lock(&m_lLock);

		vSign.resize(m_vLayer.size());

		transform(m_vLayer.begin(), m_vLayer.end(), vSign.begin(), [](const ImLayer::tagLayer& tParam) {return tParam.nSign; });
	}

	void CImCenter::Clear()
	{
		CSWLock lock(&m_lLock);

		m_vLayer.clear();
	}

	void CImCenter::Push(ImLayer::tagLayer& tParam)
	{
		CSWLock lock(&m_lLock);

		m_vLayer.push_back(tParam);
	}

	void CImCenter::Pop(long long nSign)
	{
		CSWLock lock(&m_lLock);

		auto iErase = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](const ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

		if (iErase == m_vLayer.end()) return;

		iErase = m_vLayer.erase(iErase);
	}

	bool CImCenter::Get(long long nSign, ImLayer::tagLayer& tParam)
	{
		CSWLock lock(&m_lLock);

		auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign,
			[](const ImLayer::tagLayer& tParam, const long long& nSign)
			{
				return tParam.nSign == nSign;
			});

		if (iSign == m_vLayer.end()) return false;

		tParam = *iSign;

		return true;
	}

	void CImCenter::Set(long long nSign, ImLayer::tagLayer& tParam)
	{
		CSWLock lock(&m_lLock);

		auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign,
			[](const ImLayer::tagLayer& tParam, const long long& nSign)
			{
				return tParam.nSign == nSign;
			});

		if (iSign == m_vLayer.end()) return;

		*iSign = tParam;
	}

	bool CImCenter::GetSyncInfo(long long nSign, ImLayer::tagLayer& tInfo)
	{
		CSRLock lock(&m_lLock);

		auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign){return tParam.nSign == nSign;});

		if (iSign == m_vLayer.end()) return false;

		iSign->GetSyncInfo(tInfo);

		return true;
	}

	bool CImCenter::GetRenderParam(long long nSign, ImLayer::tagLayer& tInfo)
	{
		CSRLock lock(&m_lLock);

		auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

		if (iSign == m_vLayer.end()) return false;

		iSign->GetRenderParam(tInfo);

		return true;
	}

	void CImCenter::Background(long long nBackground)
	{
		{
			CSWLock lock(&m_lLock);

			for (auto& iLayer : m_vLayer)
			{
				iLayer.bBackground = (iLayer.nSign == nBackground);
			}
		}

		m_nBackground = nBackground;

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nBackground);
	}

	void CImCenter::Focus(long long nFocus)
	{
		{
			CSWLock lock(&m_lLock);

			for (auto& iLayer : m_vLayer)
			{
				iLayer.bFocus = (iLayer.nSign == nFocus);
			}
		}
		m_nFocus = nFocus;

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nFocus);
	}

	void CImCenter::Enable(long long nSign, bool bEnable)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->bEnable = bEnable;
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::Name(long long nSign, CString rName)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->rName = rName;
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::Left(long long nSign, int nLeft)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->LeftChanged(nLeft);
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::Top(long long nSign, int nTop)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->TopChanged(nTop);
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::Width(long long nSign, int nWidth)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->WidthChanged(nWidth);
		}
		m_cScaling.signal();
	}

	void CImCenter::Height(long long nSign, int nHeight)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->HeightChanged(nHeight);
		}
		m_cScaling.signal();
	}

	void CImCenter::Opacity(long long nSign, double dOpacity)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->dOpacity = dOpacity;
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::HorScal(long long nSign, double dScal)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->ScalChanged(dScal, 0.0, true);
		}
		m_cScaling.signal();
	}

	void CImCenter::VerScal(long long nSign, double dScal)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->ScalChanged(0.0, dScal, false);
		}
		m_cScaling.signal();
	}

	void CImCenter::Angle(long long nSign, double dAngle)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->AngleChanged(dAngle);
		}
		m_cRotating.signal();
	}

	void CImCenter::Associate(long long nSign, bool bAssociate)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->bAssociated = bAssociate;
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::Moving(long long nSign)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->MoveEnable();
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::Scaling(long long nSign)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->ScaleEnable();
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::Rotating(long long nSign)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->RotateEnable();
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::Selecting(long long nSign)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->SelecteEnable();
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::Moving(long long nSign, int x, int y)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->Moving(x, y);
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::Move(long long nSign, int x, int y)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->Move(x, y);
		}
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign);
	}

	void CImCenter::Scaling(long long nSign, int x, int y)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->Scaling(x, y);
		}
		m_cScaling.signal();
	}

	void CImCenter::Scale(long long nSign, int x, int y)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->Scale(x, y);
		}
		m_cScaling.signal();
	}

	void CImCenter::Scale(long long nSign, double dScale)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->Scale(dScale);
		}
		m_cScaling.signal();
	}

	void CImCenter::Scale(double dScale)
	{
		{
			CSWLock lock(&m_lLock); // 跟随背景图层同步缩放，此时中心点需要同步缩放

			for(auto& iLayer : m_vLayer) iLayer.ScaleEx(dScale);
		}
		m_cScaling.signal();
	}

	void CImCenter::Rotating(long long nSign, double dAngle)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->Rotating(dAngle);
		}
		m_cRotating.signal();
	}

	void CImCenter::Rotate(long long nSign, double dAngle)
	{
		{
			CSWLock lock(&m_lLock);

			auto iSign = _Find_pr(m_vLayer.begin(), m_vLayer.end(), nSign, [](ImLayer::tagLayer& tParam, const long long& nSign) {return tParam.nSign == nSign; });

			if (iSign == m_vLayer.end()) return;

			iSign->Rotate(dAngle);
		}
		m_cRotating.signal();
	}
	void CImCenter::Contrast(double dCon)
	{
		{
			CSWLock lock(&m_lLock); 

			for (auto& iLayer : m_vLayer) iLayer.Contrast(dCon);
		}
		m_cContrast.signal();
	}
}