#include "stdafx.h"
#include "ITMutex.h"

void ITMutex::Lock()
{
	#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800为2013  2013以上支持c++11标准
		m_lock.lock();
	#else
		#ifdef _WIN32
			EnterCriticalSection(&m_lock);
		#else
			pthread_mutex_lock(&m_lock);
		#endif
	#endif
}
void ITMutex::UnLock()
{
	#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800为2013  2013以上支持c++11标准
		m_lock.unlock();
	#else
		#ifdef _WIN32
			LeaveCriticalSection(&m_lock);
		#else
			pthread_mutex_unlock(&m_lock);
		#endif
	#endif
}
//protected:

void ITMutex::Init()
{
	#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800为2013  2013以上支持c++11标准
	#else
		#ifdef _WIN32
			InitializeCriticalSection(&m_lock);
		#else
			pthread_mutex_init(&m_lock,NULL);
		#endif
	#endif
}
void ITMutex::Close()
{
	#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800为2013  2013以上支持c++11标准
	#else
		#ifdef _WIN32
			DeleteCriticalSection(&m_lock);
		#else
			pthread_mutex_destroy(&m_lock);
		#endif
	#endif
}