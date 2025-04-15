#include "pch.h"
#include "PrinterBase.h"

CPrinterBase::CPrinterBase():
	m_uPrinterType(ePT_FastPrint), m_b1bitDll(false)
{
	m_strImBaseDir = "G:\\7.11\\debug\\";
}

CPrinterBase::~CPrinterBase()
{

}

int CPrinterBase::Pushback(vParam& iParam, vParams& iParams, CSRWLockBase& cLock)
{
	int nSize = 0;
	{
		CSWLock lock(&cLock);

		iParams.push_back(iParam);

		nSize = iParams.size();
	}
	return nSize;
}

void CPrinterBase::Pushfront(vParam& iParam, vParams& iParams, CSRWLockBase& cLock)
{
	CSWLock lock(&cLock);

	iParams.insert(iParams.begin(), iParam);
}

bool CPrinterBase::Popfront(vParam& iParam, vParams& iParams, CSRWLockBase& cLock)
{
	bool bEmpty = true;
	{
		CSWLock lock(&cLock);

		if (!(bEmpty = iParams.empty()))
		{
			auto iFront = iParams.begin();

			iParam = (*iFront);

			iFront = iParams.erase(iFront);
		}
	}
	return (!bEmpty);
}

void CPrinterBase::Restart(bool& bTermination, CSRWLockBase& cLock)
{
	CSWLock lock(&cLock);

	bTermination = false;
}

void CPrinterBase::Termination(bool& bTermination, CSRWLockBase& cLock)
{
	CSWLock lock(&cLock);

	bTermination = true;
}

bool CPrinterBase::IsTermination(bool& bTermination, CSRWLockBase& cLock)
{
	bool bTmp = false;
	{
		CSRLock lock(&cLock);

		bTmp = bTermination;
	}
	return bTmp;
}

unsigned int __stdcall CPrinterBase::OnWorkThread(void* lpUesrParam)
{
	vThParam* lpParam = (vThParam*)lpUesrParam;
	auto& iCaller = get<0>(*lpParam);
	auto& iThis = get<1>(*lpParam);
	auto& iLock = get<2>(*lpParam);
	auto& iSigWork = get<3>(*lpParam);
	auto& iSigTerm = get<4>(*lpParam);
	auto& iTermination = get<5>(*lpParam);
	auto& iParams = get<6>(*lpParam);
	auto& fPro = get<7>(*lpParam);

	while (!iThis->IsTermination(*iTermination, *iLock))
	{
		vParam iParam;

		if (iThis->Popfront(iParam, *iParams, *iLock))
		{
			fPro(iParam);

			continue;
		}
		iSigWork->wait();
	}
	iSigTerm->signal();

	return 0;
}

unsigned int __stdcall CPrinterBase::OnExitThread(void* lpUesrParam)
{
	vThParam* lpParam = (vThParam*)lpUesrParam;
	auto& iCaller = get<0>(*lpParam);
	auto& iThis = get<1>(*lpParam);
	auto& iLock = get<2>(*lpParam);
	auto& iSigWork = get<3>(*lpParam);
	auto& iSigTerm = get<4>(*lpParam);
	auto& iTermination = get<5>(*lpParam);
	iThis->Termination(*iTermination, *iLock);
	iSigWork->signal();
	return 0;
}