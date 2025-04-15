#include "pch.h"
#include "DALSACapture.h"

namespace DALSACamera
{
	CCapture::CCapture():
		m_bTermination(false), m_lpInitiator(nullptr)
	{

	}

	CCapture::~CCapture()
	{

	}

	void CCapture::Reg(HWND& lpHandle)
	{
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnCapture, (void*)this, 0, NULL));
	}

	void CCapture::Unreg(HWND& lpHandle)
	{
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnTermination, (void*)this, 0, NULL));

		m_cTermination.wait();
	}

	unsigned int __stdcall CCapture::OnTermination(void* lpParam)
	{
		CCapture* lpThis = (CCapture*)lpParam;

		lpThis->Termination();

		lpThis->m_cCapture.signal();

		return 0;
	}

	void CCapture::OnAcquisition(SapXferCallbackInfo* pInfo)
	{
		_cwprintf(L"%s begin...\r\n", __FUNCTIONW__);

		CCapture* lpThis = (CCapture*)pInfo->GetContext();

		auto iTransfer = pInfo->GetTransfer();

		auto iPair = iTransfer->GetPair(0);

		SapBuffer* lpBuffer = (SapBuffer*)iPair->GetDst();

		auto idx = lpBuffer->GetIndex();

		auto iImWidth = lpBuffer->GetWidth();

		auto iImHeight = lpBuffer->GetHeight();

		ImPtr::CImPtr cImage(new Mat(iImHeight, iImWidth, CV_8UC1));

		BYTE pData; void* lpDataAddr = &pData;

		BOOL bGet = lpBuffer->GetAddress(idx, &lpDataAddr);

		memcpy(cImage.Get()->data, lpDataAddr, iImHeight * iImWidth);

		BOOL bRelease = lpBuffer->ReleaseAddress(lpDataAddr);

		lpThis->Push(cImage);
	}

	unsigned int __stdcall CCapture::OnCapture(void* lpParam)
	{
		CCapture* lpThis = (CCapture*)lpParam;

		ImPtr::CImPtr cFrame;

		HWND lpInitiator = nullptr;

		while (!lpThis->IsTermination())
		{
			if (lpThis->Pop(cFrame))
			{ 
				lpThis->GetInitiator(lpInitiator); 

				lpThis->OnCapture(cFrame, lpInitiator); 

				continue; 
			}
			lpThis->m_cCapture.wait();
		}
		lpThis->m_cTermination.signal();

		return 0;
	}

	void CCapture::Push(ImPtr::CImPtr& cFrame)
	{
		{
			CSWLock lock(&m_lLock);

			m_vFrame.push_back(cFrame);
		}
		m_cCapture.signal();
	}

	bool CCapture::Pop(ImPtr::CImPtr& cFrame)
	{
		bool bEmpty = true;
		{
			CSWLock lock(&m_lLock);

			if (!(bEmpty = m_vFrame.empty()))
			{
				auto iFront = m_vFrame.begin();

				cFrame = *iFront;

				iFront = m_vFrame.erase(iFront);
			}
		}
		return !bEmpty;
	}

	void CCapture::Termination()
	{
		CSWLock lock(&m_lLock);

		m_bTermination = true;
	}

	bool CCapture::IsTermination()
	{
		CSRLock lock(&m_lLock);

		return m_bTermination;
	}

	void CCapture::SetInitiator(HWND& lpHandle)
	{
		CSWLock lock(&m_lLock);

		m_lpInitiator = lpHandle;
	}

	void CCapture::GetInitiator(HWND& lpHandle)
	{
		CSRLock lock(&m_lLock);

		lpHandle = m_lpInitiator;
	}
}
