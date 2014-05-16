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
	LOG(INFO) <<"CTimerService::OnStart Begin~ "<<endl;
	for(int i = 0;i < 10;i++)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		LOG(INFO) <<"CTimerService::OnStart "<<i<<" 现在时间是 "<<sysTime.wYear<<sysTime.wMonth<<sysTime.wDay<<endl;
		Sleep(1000);
	}
	//MessageBox(str,NULL,MB_OK);
}

void CTimerService::OnStop()
{

}