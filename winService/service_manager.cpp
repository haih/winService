#include "service_manager.h"

//install the service to the SCM
bool CServiceManager::Install(CServiceBase* service)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szPath[MAX_PATH];

    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
        cout<<"Cannot get module file name! LastError = "<<GetLastError();
        return false;
    }

    // Get a handle to the SCM database. 
 
    schSCManager = OpenSCManager( 
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 
 
    if (NULL == schSCManager) 
    {
        cout<<"OpenSCManager failed! Last Error = "<<GetLastError();
        return false;
    }

    // Create the service
	cout<<"CServiceManager::Install~ Service Name ="<<service->GetName()<<endl;
	
    schService = CreateService( 
        schSCManager,              // SCM database 
        service->GetName(),                   // name of service 
        service->GetDisplayName(),  // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS , // service type 
        SERVICE_AUTO_START,         // start type 
        SERVICE_ERROR_NORMAL,       // error control type 
        szPath,                     // path to service's binary 
        NULL,                       // no load ordering group 
        NULL,                       // no tag identifier 
        NULL,                       // no dependencies 
        NULL,                       // LocalSystem account 
        NULL);                      // no password 
 
    if (schService == NULL) 
    {
        cout<<"CreateService failed! LastError = "<<GetLastError(); 
        CloseServiceHandle(schSCManager);
        return false;
    }
    else 
	{
		cout<<"Service installed successfully\n"; 
	}

    CloseServiceHandle(schService); 
    CloseServiceHandle(schSCManager);
	return true;
}

bool CServiceManager::Uninstall(CServiceBase* service)
{
	//SERVICE_STATUS_PROCESS ssp;
	SC_HANDLE svcControlManager = ::OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE );
	if(NULL == svcControlManager)
	{
		cout<<"ERROR! CServiceManager::Uninstall open scm failed~"<<endl;
		return false;
	}
	SC_HANDLE servHandle = ::OpenService( svcControlManager, 
										  service->GetName(),
                                          SERVICE_QUERY_STATUS|SERVICE_STOP|DELETE);

	if(!servHandle)
	{
		cout<<"Couldn't open service~ Error Code = "<<::GetLastError()<<endl;
		return false;
	}
	
	if (!::DeleteService(servHandle)) 
	{
		cout<<"Couldn't delete service~ Error Code = "<<::GetLastError()<<endl;	
		CloseServiceHandle(servHandle); 
    	CloseServiceHandle(svcControlManager);
    	return false;
  	}
	CloseServiceHandle(servHandle); 
    CloseServiceHandle(svcControlManager);
	return true;
}


bool CServiceManager::Run(CServiceBase* service)
{
	// run service with given name
		SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
		if (schSCManager==0) 
		{
			long nError = GetLastError();
			cout<<"OpenSCManager failed, error code = "<<nError<<endl;
			
		}
		else
		{
			// open the service
			SC_HANDLE schService = OpenService( schSCManager,
												service->GetName(),
												SERVICE_ALL_ACCESS);
			if(!schService)
			{
				cout<<"Couldn't open service~ Error Code = "<<::GetLastError()<<endl;
				return false;
			}
			else
			{
				// call StartService to run the service
				if(StartService(schService, 0, (const TCHAR**)NULL))
				{
					cout<<"Service "<<service->GetName()<<" started"<<endl;
					CloseServiceHandle(schService); 
					CloseServiceHandle(schSCManager); 
					return true;
				}
				else
				{
					long nError = GetLastError();
					cout<<"StartService failed, error code = "<<nError<<endl;
				}
				CloseServiceHandle(schService); 
			}
			CloseServiceHandle(schSCManager); 
		}
		return false;
	

}

bool CServiceManager::Stop(CServiceBase* service)
{
	return true;
}

