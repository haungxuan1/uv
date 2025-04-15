#include "pch.h"
#include "ImPtr.h"

using namespace cv;
using namespace std;

namespace ImPtr
{
	CImData::CImData(Mat* lpImData)
		:m_nRef(1), m_lpImData(lpImData), m_nTimer(0)
	{
		LARGE_INTEGER tTimer = { 0 };

		QueryPerformanceCounter(&tTimer);

		m_nTimer = tTimer.QuadPart;

	//	cout << "new: " << hex << m_lpImData << endl;
	}

	CImData::~CImData()
	{ 
		delete m_lpImData; 

	//	cout << "del: " << hex << m_lpImData << endl;
	}

	void CImData::Ref()
	{
		CSWLock lock(&m_lLock); 
		
		++m_nRef; 
	}

	int CImData::Unref() 
	{ 
		CSWLock lock(&m_lLock); 
		
		return --m_nRef; 
	}

	Mat* CImData::Get() 
	{ 
		return m_lpImData; 
	}

	long long CImData::Timer()
	{
		return m_nTimer;
	}
	/*******************************************************************************/
	CImPtr::CImPtr() 
		:m_lpImPtr(nullptr) 
	{
	
	}

	CImPtr::CImPtr(Mat* lpImData) 
	{
		m_lpImPtr = new CImData(lpImData); 

//		cout << "new im data: " << hex << m_lpImPtr << endl;
	}

	CImPtr::CImPtr(const CImPtr& other) 
	{ 
		other.Ref(); 
		
		m_lpImPtr = other.m_lpImPtr; 
	}

	CImPtr::~CImPtr() 
	{ 
		if (Unref() <= 0) 
			m_lpImPtr ? [this]() {delete m_lpImPtr; }() : []() {}(); 
	}

	CImPtr& CImPtr::operator=(const CImPtr& other) 
	{ 
		if (Unref() <= 0) 
		{ 
			m_lpImPtr ? [this]() {delete m_lpImPtr; }() : []() {}(); 
		} 
		other.Ref(); 

		m_lpImPtr = other.m_lpImPtr; 

		return *this;
	}

	bool CImPtr::operator==(CImPtr& other) 
	{ 
		if (m_lpImPtr == other.m_lpImPtr) return true; // 同 nullptr 时

		if (!!!m_lpImPtr || !!!other.m_lpImPtr) return false; // 仅单个 nullptr 时

		return m_lpImPtr->Get() == other.m_lpImPtr->Get();
	}

	cv::Mat* CImPtr::Get() 
	{ 
		return (m_lpImPtr ? m_lpImPtr->Get() : nullptr); 
	}

	long long CImPtr::Timer()
	{
		return (Empty() ? 0 : m_lpImPtr->Timer());
	}

	bool CImPtr::Empty()
	{
		return (!!!m_lpImPtr || !!!m_lpImPtr->Get());
	}

	void CImPtr::Ref() 
	{ 
		if (m_lpImPtr) m_lpImPtr->Ref();
	}

	int CImPtr::Unref()
	{ 
		return !!m_lpImPtr && m_lpImPtr->Unref(); 
	}

	cv::Mat* CImPtr::Get()const 
	{ 
		return const_cast<CImPtr*>(this)->Get(); 
	}

	long long CImPtr::Timer()const
	{
		return const_cast<CImPtr*>(this)->Timer();
	}

	bool CImPtr::Empty()const
	{
		return const_cast<CImPtr*>(this)->Empty();
	}

	void CImPtr::Ref()const 
	{ 
		const_cast<CImPtr*>(this)->Ref(); 
	}

	int CImPtr::Unref()const 
	{ 
		return const_cast<CImPtr*>(this)->Unref(); 
	}
}