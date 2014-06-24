#include <gtest/gtest.h>

#include "serviceImp.h"
#include "serviceInstall.h"

#define private public

VOID __stdcall DoDeleteSvc()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	SERVICE_STATUS ssStatus; 

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager( 
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager) 
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the service.

	schService = OpenService( 
		schSCManager,       // SCM database 
		 TEXT("IPCService"),          // name of service 
		DELETE);            // need delete access 

	if (schService == NULL)
	{ 
		printf("OpenService failed (%d)\n", GetLastError()); 
		CloseServiceHandle(schSCManager);
		return;
	}

	// Delete the service.

	if (! DeleteService(schService) ) 
	{
		printf("DeleteService failed (%d)\n", GetLastError()); 
	}
	else printf("Service deleted successfully\n"); 

	CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager);
}

class UT_ServiceInstaller: public testing::Environment
{
public:
	virtual void SetUp()
	{
	}
	virtual void TearDown()
	{
		DoDeleteSvc();
	}
private:
};

TEST(UT_ServiceInstaller, install)
{
	CServiceBase* svcImpl = CWindowsServiceImpl::GetInstance();
	BOOL ret = CServiceInstaller::Installer(svcImpl);
	EXPECT_TRUE(ret);
	DoDeleteSvc();

	//EXPECT_NE(NULL, thread->Join());
	//EXPECT_EQ(6, Foo(30, 18));
}
