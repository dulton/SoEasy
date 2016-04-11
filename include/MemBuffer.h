#pragma once
/*
* MemBuffer 类 数据buf缓存类  
* AutoLock  自动锁
* ThreadLock 线程锁
* 时间：2015/7/27 WZQ
*/

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
#include "assert.h"
#include<iostream>
using namespace std;

class ThreadLock;
class AutoLock;
template<typename T>
class MemBuffer
{
public:	
	typedef size_t   size_type;	
	MemBuffer(size_t nSize) : m_pMemBuffer(new T[nSize+1]),m_size(nSize),m_count(0) 
	{		

	}

	~MemBuffer() 
	{ 
		delete []m_pMemBuffer; 
		m_pMemBuffer = NULL;
	}
	

	MemBuffer& operator=(const MemBuffer<T>& other) 
	{
		if (this != &other) 
		{
			AutoLock	guard(m_mutex);
			MemBuffer<T>	temp(other);        // invoke copy constructor
			_Swap(temp);                      // this->_Swap();
		}
		return (*this);
	}

	bool is_full() const 
	{
		AutoLock guard(m_mutex);
		return (m_count == m_size);
	}

	bool is_empty() const 
	{
		AutoLock guard(m_mutex);
		return (m_count == 0);
	}
	size_type getRemainSize() const
	{
		return (m_size - m_count);
	}
	size_type size()// const 
	{
		AutoLock guard(m_mutex);
		return m_count;
	}

	size_type capacity() const 
	{ 
		return m_size; 
	}
	size_type getData(T* data,size_type datasize)
	{
		AutoLock guard(m_mutex);
		if(data == NULL)
			return -1;
		if(datasize < m_count)
		{
			///给的内存过小
			return 0;
		}
		::memmove(data,m_pMemBuffer,m_count);
		return m_count;
	}
	////推入的数据指针，数据长度  返回实际推入的大小
	size_type push_back(const T *data, size_type length) 
	{
		AutoLock guard(m_mutex);
		if(data == NULL)
			return 0;
		//assert(data != NULL);
		if (length == 0 || length > (m_size - m_count))
		{
			cout<<"\nlength > (m_size - m_count)\n";
			return 0;
		}
		
		size_type rearLen = m_size - m_count;    // 尾部剩余空间
		if (length <= rearLen) 
		{
			::memmove(&m_pMemBuffer[m_count], data, length);			
		}else
		{
			length = rearLen;		///实际push数量
			::memmove(&m_pMemBuffer[m_count], data, rearLen);			
		}
		m_count += length;
		return (length);
	}
	size_type push_front(const T *data, size_type length) 
	{

		AutoLock guard(m_mutex);
		if(data == NULL)
			return 0;
		//assert(data != NULL);
		if (length == 0 || length > (m_size - m_count))
		{
	//		cout<<"\nlength > (m_size - m_count)\n";
			return 0;
		}

		size_type rearLen = m_size - m_count;    // 尾部剩余空间
		if (length <= rearLen) 
		{
			///把原有数据往后移 再把数据塞到前面
			::memmove(&m_pMemBuffer[0],&m_pMemBuffer[rearLen],m_count);
			::memmove(&m_pMemBuffer[0], data, length);			
		}else
		{
			length = rearLen;		///实际push数量
			::memmove(&m_pMemBuffer[0],&m_pMemBuffer[rearLen],m_count);
			::memmove(&m_pMemBuffer[0], data, rearLen);			
		}
		m_count += length;
		return (length);
	}
	size_type pop_back(T *buf, size_type length)
	{
		AutoLock guard(m_mutex);
		//assert(buf != NULL);
		if(buf == NULL)
			return 0;
		if (length == 0)// || length > m_count) 
		{
			//		OutputDebugString("\n无数据可取!\n");
			return 0;
		}
		//如果取的长度 > 全部数据长度，则全取
		if (length > m_count)
		{
			length = m_count;
			::memmove(buf, &m_pMemBuffer[0], length);		
		}else
		{
			size_type rearLen = m_count - length;    // 前面剩余数据		
			::memmove(buf, &m_pMemBuffer[rearLen], length);			
		}		
		m_count -= length;				
		return (length);
	}
	size_type pop_front(T *buf, size_type length)
	{
		AutoLock guard(m_mutex);
//		assert(buf != NULL);
		if(buf == NULL)
			return 0;
		if (length == 0)// || length > m_count) 
		{
			//		OutputDebugString("\n无数据可取!\n");
			return 0;
		}
		//如果取的长度 > 全部数据长度，则全取
		if (length > m_count)
		{
			length = m_count;
			::memmove(buf, &m_pMemBuffer[0], length);		
		}else
		{
			::memmove(buf, &m_pMemBuffer[0], length);	
			size_type rearLen = m_count - length;    // 剩余数据		
			::memmove(m_pMemBuffer, &m_pMemBuffer[length], rearLen);	//剩余数据移到前面
		}	
		m_count -= length;				
		return (length);
	}
	size_type pop_front(size_type length)
	{
		AutoLock guard(m_mutex);
		//		assert(buf != NULL);		
		if (length == 0)// || length > m_count) 
		{			
			return 0;
		}
		//如果取的长度 > 全部数据长度，则全取
		if (length > m_count)
		{
			length = m_count;			
		}else
		{			
			size_type rearLen = m_count - length;    // 剩余数据		
			::memmove(m_pMemBuffer, &m_pMemBuffer[length], rearLen);	//剩余数据移到前面
		}	
		m_count -= length;				
		return (length);
	}
	size_type remove_front(size_type length)
	{
		AutoLock guard(m_mutex);
		//		assert(buf != NULL);		
		if (length == 0)// || length > m_count) 
		{			
			return 0;
		}
		//如果取的长度 > 全部数据长度，则全取
		if (length > m_count)
		{
			length = m_count;			
		}else
		{			
			size_type rearLen = m_count - length;    // 剩余数据		
			::memmove(m_pMemBuffer, &m_pMemBuffer[length], rearLen);	//剩余数据移到前面
		}	
		m_count -= length;				
		return (length);
	}
	void clear()
	{
		AutoLock guard(m_mutex);
		m_count = 0;
		memset(m_pMemBuffer,0,m_size);
	}
private:
	void _Swap(MemBuffer<T>& other)
	{
		std::swap(m_pMemBuffer, other.m_pMemBuffer);
		std::swap(m_size, other.m_size);	
		std::swap(m_count, other.m_count);
	}
	
private:
	T*					m_pMemBuffer;		// buffer	
	size_type			m_size;				//buffer总大小	
	size_type           m_count;            //有效字节数	
	mutable  ThreadLock			m_mutex;			//线程锁
};



class ThreadLock
{
public:
	ThreadLock(void)
	{
		Init();
	}
	~ThreadLock()
	{
		Close();
	}
	void Lock()
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
	void UnLock()
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

	void Init()
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
	void Close()
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

private:
	LOCK_MUTEX m_lock;	
};

//定义一个自动加锁的类
class AutoLock{
public:	
	AutoLock(ThreadLock& pThreadLock)
	{
		m_pThreadLock=&pThreadLock;
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->Lock();
		}
	}	
	AutoLock(ThreadLock *pThreadLock)
	{
		m_pThreadLock=pThreadLock;
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->Lock();
		}
	}
	~AutoLock()
	{
		if (NULL!=m_pThreadLock)
		{
			m_pThreadLock->UnLock();
		}
	}
private:
	ThreadLock* m_pThreadLock;	
};
