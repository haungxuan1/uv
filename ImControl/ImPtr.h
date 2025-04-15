#pragma once
#include "opencv.hpp"
#include "SRWLock.h"

namespace ImPtr
{
	class CImPtr;
	class CImData
	{
		friend class CImPtr;
	private:
		CImData() = delete;

	private:
		CImData(cv::Mat* lpImData);
		~CImData();

	private:
		void Ref();
		int Unref();

	private: // 通过该接口修改mat存在安全隐患
		cv::Mat* Get();
		long long Timer();

	private:
		int m_nRef;
		cv::Mat* m_lpImData;
		long long m_nTimer; // 该计时器记录着指针生成时间
		CSRWLockBase m_lLock;
	};

	class CImPtr
	{
	public:
		CImPtr();
		~CImPtr();

	public:
		CImPtr(cv::Mat* lpImData);
		CImPtr(const CImPtr& other);

	public:
		CImPtr& operator=(const CImPtr& other);
		bool operator==(CImPtr& other);

	public:
		cv::Mat* Get();
		cv::Mat* Get()const;
		long long Timer();
		long long Timer()const;
		bool Empty();
		bool Empty()const;

	private:
		void Ref();
		int Unref();
		void Ref()const;
		int Unref()const;

	private:
		CImData* m_lpImPtr;
	};
}