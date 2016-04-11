#pragma once
//************************************
//跨平台线程锁类
//时间：2016/1/18 WZQ
//************************************
//************************************
#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800为2013  2013以上支持c++11标准
	#include <mutex>
	#define LOCK_MUTEX std::mutex
#else 
	#ifdef _WIN32
		#include <Windows.h>
		#define LOCK_MUTEX CRITICAL_SECTION		///线程锁 耗费时间短 但不能跨进程
	#else
		#include <pthread.h>
		#define LOCK_MUTEX pthread_mutex_t			///线程锁 时间长
	#endif
#endif


class ITMutex
{
public:
	ITMutex(void)
	{
		Init();
	}
	~ITMutex()
	{
		Close();
	}
	void Init();
	void Lock();
	void UnLock();	
	void Close();
private:
	LOCK_MUTEX m_lock;	
};

//定义一个自动加锁的类
class ITAutoLock{
public:	
	ITAutoLock(ITMutex& pThreadLock)
	{
		m_pThreadLock=&pThreadLock;
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->Lock();
		}
	}	
	ITAutoLock(ITMutex *pThreadLock)
	{
		m_pThreadLock=pThreadLock;
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->Lock();
		}
	}
	~ITAutoLock()
	{
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->UnLock();
		}
	}
private:
	ITMutex* m_pThreadLock;	
};
