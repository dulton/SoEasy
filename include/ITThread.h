#pragma once
#include <string>

#if defined(_MSC_VER) && (_MSC_VER >= 1800)  ///1800为2013  2013以上支持c++11标准
	#include <mutex>
	#include <thread>
	#define LOCK_MUTEX std::mutex
#else 
	#ifdef _WIN32
		#include <Windows.h>
		#include <process.h>
		#define LOCK_MUTEX CRITICAL_SECTION		///线程锁 耗费时间短 但不能跨进程
	#else
		#include <pthread.h>
		#define LOCK_MUTEX pthread_mutex_t			///线程锁 时间长
	#endif
#endif

///目前只有windows版本，linux版本还没写
typedef void (*UsrThreadFun)(void* pThis);
class ITThread
{
public:
	ITThread(void);	
	ITThread(const char * ThreadName);  
	ITThread(std::string ThreadName);  
	~ITThread(void);
	bool Start(bool bSuspend = false);  	
	bool Start(UsrThreadFun usrFun,void* pUsrData=NULL,bool bSuspend = false);  	

	void Join(int timeout = -1);  
	void Resume();  
	void Suspend();  
	bool Terminate(unsigned long ExitCode);  

	unsigned int GetThreadID();  
	std::string GetThreadName();  
	void SetThreadName(std::string ThreadName);  
	void SetThreadName(const char * ThreadName);  	
	void SetThreadFun(UsrThreadFun tFun,void* pThis);
	static int ThreadFunc(void* pParam);// Adapter
protected:
	int ThreadFuncKernal();// Kernal
private:  	
	HANDLE			m_handle;
	UsrThreadFun		m_threadfun;
	void*		    m_pThis;	
	unsigned int	m_nThreadID;  
	std::string		m_strThreadName;  
	volatile bool	m_bRun;				//线程状态
	bool			m_bRunThreadCond;	//运行条件
};



