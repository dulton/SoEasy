#include "stdafx.h"
#include "ITThread.h"


ITThread::ITThread(void)
{
	m_bRunThreadCond = false;
	m_handle=NULL;
	m_bRun=false;
}

ITThread::ITThread(const char * ThreadName):m_strThreadName(ThreadName), m_bRun(false)  
{  
	m_bRunThreadCond = false; 
	m_handle=NULL;
	m_bRun=false;
}  

ITThread::ITThread(std::string ThreadName):m_strThreadName(ThreadName), m_bRun(false)  
{  
	m_bRunThreadCond = false;
	m_handle=NULL;
	m_bRun=false;
}  


ITThread::~ITThread(void)
{
}
// Thread function Adapter
int ITThread::ThreadFunc(void* pParam)
{
	ITThread *pObj = (ITThread *)pParam;
	return pObj->ThreadFuncKernal();	
}

//************************************
//函数名:  ThreadFuncKernal
//描述：线程函数实际运行地方
//参数：无
//返回值：bool
//时间：2016/1/12 WZQ
//************************************
int ITThread::ThreadFuncKernal()
{
	if(m_threadfun)
	{
		m_threadfun(m_pThis);
	}	
	return 0;
}

bool ITThread::Start(bool bSuspend)  
{  
	if(m_bRun)  
	{  
		return true;  
	}  
	if(!m_threadfun)
		return false;
	m_bRunThreadCond = true;	//while条件置真
	if(bSuspend)  
	{  
		m_handle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))ThreadFunc, this, CREATE_SUSPENDED, &m_nThreadID);  
	}else  
	{  
		m_handle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))ThreadFunc, this, 0, &m_nThreadID);  
	}  
	m_bRun = (NULL != m_handle);  
	m_bRunThreadCond = m_bRun;
	return m_bRun;  
}  
bool ITThread::Start(UsrThreadFun usrFun,void* pUsrData,bool bSuspend)
{
	if(!usrFun)
		return false;	
	m_threadfun = usrFun;
	m_pThis = pUsrData;	
	m_bRunThreadCond = true;	//while条件置真
	if(bSuspend)  
	{  
		m_handle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))ThreadFunc, this, CREATE_SUSPENDED, &m_nThreadID);  
	}else  
	{  
		m_handle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))ThreadFunc, this, 0, &m_nThreadID);  
	}  
	m_bRun = (NULL != m_handle);  
	m_bRunThreadCond = m_bRun;
	return m_bRun;  
}

void ITThread::Join(int timeout)  
{  
	if(NULL == m_handle || !m_bRun)  
	{  
		return;  
	}  
	if(timeout <= 0)  
	{  
		timeout = INFINITE;  
	}  
	::WaitForSingleObject(m_handle, timeout);  
}  

void ITThread::Resume()  
{  
	if(NULL == m_handle || !m_bRun)  
	{  
		return;  
	}  
	::ResumeThread(m_handle);  
}  

void ITThread::Suspend()  
{  
	if(NULL == m_handle || !m_bRun)  
	{  
		return;  
	}  
	::SuspendThread(m_handle);  
}  

bool ITThread::Terminate(unsigned long ExitCode)  
{  
	if(NULL == m_handle || !m_bRun)  
	{  
		return true;  
	}  
	if(::TerminateThread(m_handle, ExitCode))  
	{  
		::CloseHandle(m_handle);  
		return true;  
	}  
	return false;  
}  

unsigned int ITThread::GetThreadID()  
{  
	return m_nThreadID;  
}  

std::string ITThread::GetThreadName()  
{  
	return m_strThreadName;  
}  

void ITThread::SetThreadName(std::string ThreadName)  
{  
	m_strThreadName = ThreadName;  
}  

void ITThread::SetThreadName(const char * ThreadName)  
{  
	if(NULL == ThreadName)  
	{  
		m_strThreadName = "";  
	}  
	else  
	{  
		m_strThreadName = ThreadName;  
	}  
}  
void ITThread::SetThreadFun(UsrThreadFun tFun,void* pThis)
{
	m_threadfun = tFun;
	m_pThis = pThis;	
}