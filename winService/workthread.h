/*------------------------------------------------------*/
/* Thread base class                                    */
/*                                                      */
/*                                                      */
/*                                                      */
/* Author                                               */
/*    HaiHu (haihu@iflytek.com)                */
/*                                                      */
/* History                                              */
/*    05/26/2014  Create                                */
/*                                                      */
/*------------------------------------------------------*/

#ifndef WORKTHREAD_H
#define WORKTHREAD_H


#include "define.h"


class  CWorkThread
{
public:
	THREAD_ID GetThreadId();
//	CCmThreadManager::TType GetThreadType();
	THREAD_HANDLE GetThreadHandle();
	void Terminate();

	// Create thread.
	// The function won't return until OnThreadInit() returns.
	virtual RESULT Create();

	// Wait until the thread function return.
	RESULT Join();

	// Delete this.
	RESULT Destory(RESULT aReason);

	virtual RESULT OnThreadInit();
	virtual RESULT OnThreadRun();

	virtual void SetStop()
	{
		m_bStopFlag = TRUE;
	}

	virtual BOOL GetStopFlag()
	{
		return m_bStopFlag;
	}

public:
	CWorkThread();
	virtual ~CWorkThread();

	CWorkThread(const CWorkThread &);
	CWorkThread &operator=(const CWorkThread &);

private:
	static unsigned WINAPI ThreadProc(void *aPara);


protected:
	THREAD_ID m_Tid;
	THREAD_HANDLE m_Handle;
	PIPE_HANDLE m_PipeHandle;

private:
	BOOL m_bStopFlag;
	
};

#endif // !WORKTHREAD_H
