#include "servicebase.h"

CServiceBase* CServiceBase::m_service = NULL;
HANDLE CServiceBase::m_thread = NULL;
BOOL CServiceBase::m_fStopping = false;

CServiceBase::CServiceBase( char* name = "ServiceBase",
							char* displayName = "ServiceBase",
							DWORD  dwServiceType = SERVICE_WIN32_OWN_PROCESS,
							DWORD  dwStartType = SERVICE_DEMAND_START,
							DWORD  dwErrorCtrlType = SERVICE_ERROR_NORMAL)
{
	cout<< "CServiceBase Constructor~ The Service Name = " << name <<endl;    
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
	cout<< "CServiceBase Constructor~ The Service Name = " <<"ServiceBase"<<endl;   
	m_name = "ServiceBase";
	m_displayName = "ServiceBase";
	m_serviceType = SERVICE_WIN32_OWN_PROCESS;
	m_startType = SERVICE_DEMAND_START;
	m_ErrorCtrlType = SERVICE_ERROR_NORMAL;

	m_svcStatus.dwServiceType	= SERVICE_WIN32;
	m_svcStatus.dwCurrentState  = SERVICE_START_PENDING;
	m_svcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

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
	LOG(INFO) << "CServiceBase::SetStatus~ Status = " <<dwState <<" dwErrCode = "<<dwErrCode<<" dwWait = "<<dwWait;  
	m_svcStatus.dwCurrentState = dwState;
	m_svcStatus.dwWin32ExitCode = dwErrCode;
	m_svcStatus.dwWaitHint = dwWait;

	::SetServiceStatus(m_svcStatusHandle, &m_svcStatus);
}

#if 0
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
#endif
const char event1_name[] = "EventName1";


void CServiceBase::OnStart(DWORD dwArgc, TCHAR* lpszArgv[])
{
	//具体实现在子类中
	LOG(INFO) << "CServiceBase::OnStart~" <<endl;	 
}


bool CServiceBase::OnStop()
{
	LOG(INFO) << "CServiceBase::OnStop~" <<endl;
	return true;
}
void CServiceBase::Start(DWORD argc, TCHAR* argv[])
{
	LOG(INFO) << "CServiceBase::Start begin!" <<endl;    

//	SetStatus(SERVICE_START_PENDING);
	OnStart(argc,argv);
	SetStatus(SERVICE_RUNNING);
	LOG(INFO) << "CServiceBase::Start end!" <<endl;	 
	return;
}

void CServiceBase::Stop()
{
	SetStatus(SERVICE_STOP_PENDING);
	OnStop();
	SetStatus(SERVICE_STOPPED);
}

#if 0
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
#endif

BOOL  CServiceBase::RunInternal(CServiceBase* svc)
{
	m_service = svc;
	char* svcName = m_service->GetName();
	
	LOG(INFO) << "CServiceBase::RunInternal~" <<endl;  

	SERVICE_TABLE_ENTRY tableEntry[] = 
	{
		{svcName, (LPSERVICE_MAIN_FUNCTION)SvcMain},
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

	if(m_service == NULL)
	{                               
		LOG(INFO) << "CServiceBase::SvcMain: service is not available!" <<endl;    
	}
	m_service->m_svcStatusHandle = RegisterServiceCtrlHandlerEx(m_service->GetName(),ServiceCtrlHandler,NULL);
	

	if(NULL == m_service->m_svcStatusHandle)
	{
		LOG(INFO) << "CServiceBase::SvcMain: service Status Handler is not available!" <<endl;  
	}

	LOG(INFO) << "CServiceBase::Service begin! name = "<<m_service->GetName()<<endl;  

	m_service->Start(argc, argv);
	return ;
}

DWORD WINAPI CServiceBase::ServiceCtrlHandler(DWORD ctrlCode, DWORD evtType,
	void* evtData, void* context)
{
	LOG(INFO)<<"ServiceCtrlHandler Begin~ service name = "<<m_service->GetName()<<endl;
	switch (ctrlCode)
	{
	case SERVICE_CONTROL_STOP: m_service->Stop(); break;
//	case SERVICE_CONTROL_PAUSE: m_service->Pause(); break;
//	case SERVICE_CONTROL_CONTINUE: m_service->Continue(); break;
//	case SERVICE_CONTROL_SHUTDOWN: m_service->Shutdown(); break;
	case SERVICE_CONTROL_INTERROGATE: break;
	default: 
		break;
	}
	return 0;
}

