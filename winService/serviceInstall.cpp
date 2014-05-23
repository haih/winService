#include "serviceInstall.h"

//install the service to the SCM
bool CServiceInstaller::Installer(CServiceBase* service)
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
	cout<<"CServiceInstaller::Installer.Service Name ="<<service->GetName()<<endl;
	
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

//TO DO the status check
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