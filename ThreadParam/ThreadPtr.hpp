#include "pch.h"
#include "ThreadPtr.h"

namespace ThreadParam
{
	template<typename T>
	CElement<T>::CElement(T *lpParam)
	{
		m_nCount = 0;

		m_lpParam = lpParam;
	}

	template<typename T>
	CElement<T>::~CElement()
	{
		CSWLock lock(&m_cLock);

		if (m_lpParam)
		{
			delete m_lpParam;

			m_lpParam = nullptr;
		}
	}

	template<typename T>
	int CElement<T>::Ref()
	{
		int nCount = 0;
		{
			CSWLock lock(&m_cLock);

			nCount = (++m_nCount);
		}
		return nCount;
	}

	template<typename T>
	int CElement<T>::Unref()
	{
		int nCount = 0;
		{
			CSWLock lock(&m_cLock);

			nCount = (--m_nCount);
		}
		return nCount;
	}
	/*****************************************************************/
	template<typename T>
	CThreadPtr<T>::CThreadPtr()
	{
		m_lpElement = nullptr;
	}

	template<typename T>
	CThreadPtr<T>::CThreadPtr(T *lpElement)
	{
		m_lpElement = new CElement<T>(lpElement);
	}

	template<typename T>
	CThreadPtr<T>::CThreadPtr(const CThreadPtr &other)
	{
		other.m_lpElement->Ref();

		m_lpElement = other.m_lpElement;
	}

	template<typename T>
	CThreadPtr<T>::~CThreadPtr()
	{
		if (m_lpElement && m_lpElement->Unref() < 0) // 原则上拷贝已删除的变量就是一个错误的行为，因此这里足以构成多线程内存管理
		{
			delete m_lpElement;

			m_lpElement = nullptr;
		}
	}

	template<typename T>
	CThreadPtr<T> &CThreadPtr<T>::operator=(const CThreadPtr &other)
	{
		if (m_lpElement && m_lpElement->Unref() < 0)
		{
			delete m_lpElement;

			m_lpElement = nullptr;
		}

		other.m_lpElement ? other.m_lpElement->Ref() : NULL;

		m_lpElement = other.m_lpElement;

		return *this;
	}
}