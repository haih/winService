#pragma once

#include "copyDisable.h"
#include "servicebase.h"
#include "define.h"

class CServiceBase;


//windows service impliment the ipc between server <---> client using named pipe
class CWindowsServiceImpl : public CServiceBase
{
public:
	CWindowsServiceImpl();

	
	~CWindowsServiceImpl(void);

	static CWindowsServiceImpl* GetInstance();

	int OnRecvMsg();

	void Start(DWORD argc, TCHAR* argv[]);

protected:
	virtual void OnStart(DWORD dwArgc, TCHAR* lpszArgv[]);
	
	virtual bool OnStop();
	
//	static HANDLE OnThreadInit();

//	static unsigned OnThreadRun(HANDLE pipe);
	
//	static unsigned WINAPI ThreadProc(PVOID context);

private:
	static CWindowsServiceImpl* s_ServiceImpl;
    HANDLE m_hStoppedEvent;
};

