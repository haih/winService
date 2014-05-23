#pragma once

#include "copyDisable.h"
#include "servicebase.h"


class CWindowsServiceImpl : public CServiceBase
{
public:
	CWindowsServiceImpl();

	
	~CWindowsServiceImpl(void);

	void Start(DWORD argc, TCHAR* argv[]);

protected:
	virtual void OnStart(DWORD dwArgc, TCHAR* lpszArgv[]);
	virtual void OnStop();
	
	static DWORD WINAPI ThreadProc(PVOID context);
	
private:
	
    HANDLE m_hStoppedEvent;
};

