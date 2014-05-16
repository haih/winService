#include "serviceInstall.h"


bool CServiceInstaller::Install(const CServiceBase& service)
{
	CHAR* modulePath = new char[MAX_PATH];
	DWORD      dwErrorCode;

	SC_HANDLE svcControlManager = NULL;
	if (::GetModuleFileName(nullptr, modulePath, MAX_PATH) == 0) 
	{
		cout<<"Couldn't get module file name: %d\n"<<GetLastError()<<endl;
		return false;
	}
	svcControlManager = ::OpenSCManager(nullptr,nullptr,SC_MANAGER_ALL_ACCESS );

	
	if(NULL == svcControlManager)
	{
		cout<<"ERROR! CServiceInstaller::Install open scm failed~"<<endl;
		CloseServiceHandle(svcControlManager);
		return false;
	}
	if (::GetModuleFileName(nullptr, modulePath, MAX_PATH) == 0) 
	{
		cout<<"Couldn't get module file name: %d\n"<<GetLastError()<<endl;
		CloseServiceHandle(svcControlManager);
		return false;
	}
	//创建服务
	SC_HANDLE servHandle = ::CreateServiceA( svcControlManager,
											"TIMERSVC",
											"TIMERSVC",
											SERVICE_ALL_ACCESS,
											SERVICE_WIN32_OWN_PROCESS,
											SERVICE_AUTO_START,
											SERVICE_ERROR_NORMAL,
											modulePath,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!servHandle) 
	{
		dwErrorCode = ::GetLastError();
		if(dwErrorCode != ERROR_SERVICE_EXISTS)
		{
			cout<<"Couldn't create service~ Error Code = "<<::GetLastError()<<endl;
			CloseServiceHandle(svcControlManager);
			return false;
		}
		else
		{
			servHandle = ::OpenService( svcControlManager,"TIMERSVC",SERVICE_QUERY_STATUS|SERVICE_START);
			if (!servHandle) 
			{
				cout<<"Couldn't Open Service~ Error Code = "<<::GetLastError()<<endl;
				CloseServiceHandle(svcControlManager);
				return false;
			}
		}
	}
	std::cout<<"Create Service Success!"<<"\n";
	if(StartService(servHandle, 0, NULL)==0 )
	{
		//启动失败
		dwErrorCode = GetLastError();
		if(dwErrorCode == ERROR_SERVICE_ALREADY_RUNNING)
		{
			//Service already run!;
			std::cout<<"Service already run!"<<"\n";
			CloseServiceHandle(svcControlManager);
			CloseServiceHandle(servHandle);
			return true;
		}
	}
	else
	{
		//Service pending
		std::cout<<"Service pending"<<"\n";
	}
	return true;
}
bool CServiceInstaller::Installer(CServiceBase* service)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szPath[MAX_PATH];

    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
        printf("Cannot install service (%d)\n", GetLastError());
        return false;
    }

    // Get a handle to the SCM database. 
 
    schSCManager = OpenSCManager( 
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 
 
    if (NULL == schSCManager) 
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return false;
    }

    // Create the service

	std::cout<<"CServiceInstaller::Installer.Service Name ="<<service->GetName()<<endl;
    schService = CreateService( 
        schSCManager,              // SCM database 
        service->GetName(),                   // name of service 
        service->GetDisplayName(),  // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        szPath,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 
 
    if (schService == NULL) 
    {
        printf("CreateService failed (%d)\n", GetLastError()); 
        CloseServiceHandle(schSCManager);
        return false;
    }
    else printf("Service installed successfully\n"); 

    CloseServiceHandle(schService); 
    CloseServiceHandle(schSCManager);
	return true;
}

bool CServiceInstaller::Uninstall(CServiceBase* service)
{
	SERVICE_STATUS_PROCESS ssp;
	SC_HANDLE svcControlManager = ::OpenSCManager(nullptr,nullptr,SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE );
	if(NULL == svcControlManager)
	{
		cout<<"ERROR! CServiceInstaller::Uninstall open scm failed~"<<endl;
		return false;
	}
	SC_HANDLE servHandle = ::OpenService( svcControlManager, 
										  "TIMERSVC",
                                          SERVICE_QUERY_STATUS|SERVICE_STOP|DELETE);

	if(!servHandle)
	{
		cout<<"Couldn't open service~ Error Code = "<<::GetLastError()<<endl;
		return false;
	}
	if (!ControlService(servHandle,SERVICE_CONTROL_STOP,(LPSERVICE_STATUS) &ssp ) )
    {
        cout<<"ControlService failed  Error Code = "<<::GetLastError()<<endl;
        CloseServiceHandle(servHandle); 
    	CloseServiceHandle(svcControlManager);
		return false;
    }
	if (!::DeleteService(servHandle)) 
	{
		cout<<"Couldn't delete service~ Error Code = "<<::GetLastError()<<endl;	
    	return false;
  	}
	return true;
}