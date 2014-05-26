#pragma once

#include "copyDisable.h"
#include "servicebase.h"
#include "define.h"

//windows service impliment the ipc between server <---> client using named pipe
class CWindowsServiceImpl : public CServiceBase
{
public:
	CWindowsServiceImpl();

	
	~CWindowsServiceImpl(void);

	

	int OnRecvMsg();

	void Start(DWORD argc, TCHAR* argv[]);

protected:
	virtual void OnStart(DWORD dwArgc, TCHAR* lpszArgv[]);
	
	virtual void OnStop();
	
//	static HANDLE OnThreadInit();

//	static unsigned OnThreadRun(HANDLE pipe);
	
//	static unsigned WINAPI ThreadProc(PVOID context);

private:

    HANDLE m_hStoppedEvent;
};

