#include "timerSvc.h"
#include <time.h>

CTimerService::~CTimerService()
{
	/*if(NULL != this)
	{
		delete this;
	}*/
}
void CTimerService::OnStart(DWORD argc, TCHAR* argv[])
{
	Sleep(5000);
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	cout<<"现在时间是 "<<sysTime.wYear<<sysTime.wMonth<<sysTime.wDay<<endl;
	//MessageBox(str,NULL,MB_OK);
}

void CTimerService::OnStop()
{

}