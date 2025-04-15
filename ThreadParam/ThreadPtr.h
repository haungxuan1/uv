#pragma once
#include "SRWLock.h"

namespace ThreadParam
{
	template<typename T>
	class CElement
	{
	public:
		CElement(T *lpParam);
		~CElement();

	public:
		int Ref();
		int Unref();

	private:
		int m_nCount;
		T *m_lpParam;
		CSRWLockBase m_cLock;
	};
	/**********************************************************************/
	template<typename T>
	class CThreadPtr
	{
	public:
		CThreadPtr();
		CThreadPtr(T *lpElement);
		CThreadPtr(const CThreadPtr &other);
		~CThreadPtr();

	public:
		CThreadPtr &operator=(const CThreadPtr &other);

	private:
		CElement<T> *m_lpElement;
	};
}