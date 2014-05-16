#include "servicebase.h"

CServiceBase* CServiceBase::m_service = NULL;

CServiceBase::CServiceBase( char* name = "ServiceBase",
							char* displayName = "ServiceBase",
							DWORD  dwServiceType = SERVICE_WIN32_OWN_PROCESS,
							DWORD  dwStartType = SERVICE_DEMAND_START,
							DWORD  dwErrorCtrlType = SERVICE_ERROR_NORMAL)
{
	m_name = name;
	m_displayName = displayName;
	m_serviceType = dwServiceType;
	m_startType = dwStartType;
	m_ErrorCtrlType = dwErrorCtrlType;

	m_svcStatus.dwServiceType	= SERVICE_WIN32;
	m_svcStatus.dwCurrentState  = SERVICE_START_PENDING;
	m_svcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

	m_svcStatus.dwServiceSpecificExitCode = 0;
	m_svcStatus.dwWin32ExitCode = 0;
	m_svcStatus.dwCheckPoint = 0;
	m_svcStatus.dwWaitHint = 0;
}

CServiceBase::CServiceBase()
{
	m_name = "ServiceBase";
	m_displayName = "ServiceBase";
	m_serviceType = SERVICE_WIN32_OWN_PROCESS;
	m_startType = SERVICE_DEMAND_START;
	m_ErrorCtrlType = SERVICE_ERROR_NORMAL;

	m_svcStatus.dwServiceType	= SERVICE_WIN32;
	m_svcStatus.dwCurrentState  = SERVICE_START_PENDING;
	m_svcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

	m_svcStatus.dwServiceSpecificExitCode = 0;
	m_svcStatus.dwWin32ExitCode = 0;
	m_svcStatus.dwCheckPoint = 0;
	m_svcStatus.dwWaitHint = 0;
}

CServiceBase::~CServiceBase()
{

}

char* CServiceBase::GetName()
{
	return m_name;
}

char* CServiceBase::GetDisplayName()
{
	return m_displayName;
}

void CServiceBase::SetStatus(DWORD dwState, DWORD dwErrCode, DWORD dwWait) 
{
  m_svcStatus.dwCurrentState = dwState;
  m_svcStatus.dwWin32ExitCode = dwErrCode;
  m_svcStatus.dwWaitHint = dwWait;

  ::SetServiceStatus(m_svcStatusHandle, &m_svcStatus);
}

void CServiceBase::WriteToEventLog(const TCHAR* msg, WORD type) 
{
		HANDLE hSource = RegisterEventSource(nullptr, m_name);
		if (hSource) 
		{
			const TCHAR* msgData[2] = {m_name, msg};
			ReportEvent(hSource, type, 0, 0, nullptr, 2, 0, msgData, nullptr);
			DeregisterEventSource(hSource);
		}
}

void CServiceBase::Start(DWORD argc, TCHAR* argv[])
{
	HANDLE hThread;

	//创建服务线程	服务完成的功能在这里调用
	hThread = CreateThread(NULL,0,Service,NULL,0,NULL);
	if(hThread == NULL)
	{
		 //CreateThread error!
		 return;
	}
	CloseHandle(hThread);
	SetStatus(SERVICE_RUNNING);
	return;
}
DWORD WINAPI CServiceBase::Service(LPVOID lpvThread)
{
       //实现函数功能的地方
	for(int i = 0;i < 100;i++)
	{
		ofstream ofile;               //定义输出文件
		ofile.open("d:\\myfile.txt");     //作为输出文件打开
		ofile<<i<<endl;
		Sleep(2000);
	}
	return 1;
}

void CServiceBase::Stop()
{
	SetStatus(SERVICE_STOP_PENDING);
	OnStop();
	SetStatus(SERVICE_STOPPED);
}

void CServiceBase::Pause()
{
	SetStatus(SERVICE_STOP_PENDING);
	OnStop();
	SetStatus(SERVICE_PAUSED);
}

void CServiceBase::Continue()
{
	SetStatus(SERVICE_CONTINUE_PENDING);
	OnContinue();
	SetStatus(SERVICE_RUNNING);
}

void CServiceBase::Shutdown()
{
	OnShutdown();
	SetStatus(SERVICE_STOPPED);
}

BOOL  CServiceBase::RunInternal(CServiceBase* svc)
{
	m_service = svc;
	char* svcName = m_service->GetName();
	//char* m_name = new char[svcName.length() + 1];
	//strcpy(m_name, svcName.c_str());

	SERVICE_TABLE_ENTRY tableEntry[] = 
	{
		{svcName, SvcMain},
		{nullptr, nullptr}
	};
	int  success;
	success = StartServiceCtrlDispatcher(tableEntry);
	if(success == 0)
	{
		cout<<"StartServiceCtrlDispatcher Failed~ Error Code = "<<::GetLastError()<<endl;	
	}	
	return success;
}

void WINAPI CServiceBase::SvcMain(DWORD argc, TCHAR* argv[])
{
	#if 1
//	SERVICE_STATUS  ServiceStatus;
//	ServiceStatus = m_service.m_svcStatus;
	
	if(m_service == NULL)
	{                               
		//cout<<"Error! there is no service to execute! "<<endl;
	}
	m_service->m_svcStatusHandle = RegisterServiceCtrlHandlerEx(m_service->GetName(),ServiceCtrlHandler,NULL);
	

	if(NULL == m_service->m_svcStatusHandle)
	{
		cout<<"Error! CServiceBase::SvcMain RegisterService! "<<endl;
	}
	#endif
	m_service->Start(argc, argv);
}

DWORD WINAPI CServiceBase::ServiceCtrlHandler(DWORD ctrlCode, DWORD evtType,
	void* evtData, void* context)
{
	{
		switch (ctrlCode)
		{
		case SERVICE_CONTROL_STOP: m_service->Stop(); break;
		case SERVICE_CONTROL_PAUSE: m_service->Pause(); break;
		case SERVICE_CONTROL_CONTINUE: m_service->Continue(); break;
		case SERVICE_CONTROL_SHUTDOWN: m_service->Shutdown(); break;
		case SERVICE_CONTROL_INTERROGATE: break;
		default: 
			break;
		}
	}
	return 0;
}

