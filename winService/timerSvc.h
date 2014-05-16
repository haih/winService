#ifndef TIMERSVC_H
#define TIMERSVC_H

#include "copyDisable.h"
#include "servicebase.h"

class CTimerService : public CServiceBase
{
public:
	CTimerService():CServiceBase(  TEXT("TimerService"),
								 		TEXT("TimerService"),
								   		SERVICE_WIN32_OWN_PROCESS,
								 		SERVICE_DEMAND_START,
								 		SERVICE_ERROR_NORMAL) 
	{
		//LOG(INFO) << "CTimerService Constructor~ The Service Name = " << "TimerService" <<endl; 
	}
	~CTimerService();
private:
	void OnStart(DWORD argc, TCHAR* argv[]);
	void OnStop();								
	DISALLOW_COPY_AND_ASSIGN(CTimerService);
};

#endif
