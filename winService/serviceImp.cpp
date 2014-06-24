
#include "serviceImp.h"
#include "workthread.h"

#pragma endregion


#define BUFFER_SIZE		4096 // 4K bytes


//HANDLE CWindowsServiceImplm_hStoppedEvent = NULL;

CWindowsServiceImpl* CWindowsServiceImpl::s_ServiceImpl = NULL;

CWindowsServiceImpl::CWindowsServiceImpl():CServiceBase( TEXT("IPCService"),
	TEXT("IPCService"),
	SERVICE_WIN32_OWN_PROCESS,
	SERVICE_DEMAND_START,
	SERVICE_ERROR_NORMAL)
{
	 cout<<"CWindowsServiceImpl Constructor!"<<endl;       
}


CWindowsServiceImpl::~CWindowsServiceImpl(void)
{
	if (m_hStoppedEvent)
    {
        CloseHandle(m_hStoppedEvent);
        m_hStoppedEvent = NULL;
    }
}

CWindowsServiceImpl* CWindowsServiceImpl::GetInstance()
{
    if (!s_ServiceImpl)
    {
        s_ServiceImpl = new CWindowsServiceImpl();
        cout<<"CWindowsServiceImpl Create CWindowsServiceImpl Instance~"<<endl;      
    }
    
    else 
    {
        cout<<"CWindowsServiceImpl Create CWindowsServiceImpl Instance Error!"<<endl;
        return NULL;
    }
    
    return s_ServiceImpl;
}

void CWindowsServiceImpl::Start(DWORD argc, TCHAR* argv[])
{
	LOG(INFO) << "CWindowsServiceImpl::Start begin!" <<endl;    

//	SetStatus(SERVICE_START_PENDING);
	OnStart(argc,argv);
	SetStatus(SERVICE_RUNNING);
	LOG(INFO) << "CWindowsServiceImpl::Start end!" <<endl;	 
	return;
}

void CWindowsServiceImpl::OnStart(DWORD dwArgc, TCHAR *lpszArgv[])
{
    // Log a service start message to the Application log.
     LOG(INFO) <<"CWindowsServiceImpl OnStart"<<endl;

	 m_hStoppedEvent = CreateEvent(  NULL,    // default security attributes
			                         TRUE,    // manual reset event
			                         FALSE,   // not signaled
			                         NULL);
	if(NULL == m_hStoppedEvent)
	{
		SetStatus(SERVICE_STOPPED,NO_ERROR,0);
		return;
	}
	SetStatus(SERVICE_RUNNING,NO_ERROR,0); 
	
	CWorkThread* pWorkThread = new CWorkThread("IPCService");
	pWorkThread->Start();
	
//	pWorkThread->SetStop();
	pWorkThread->Stop();
	
	delete pWorkThread;

	while(1)
    {
        // Check whether to stop the service.
        WaitForSingleObject(m_hStoppedEvent, INFINITE);

        SetStatus(SERVICE_STOPPED,NO_ERROR,0);
		return;
    }

    // Queue the main service function for execution in a worker thread.
//    CThreadPool::QueueUserWorkItem(&CWindowsServiceImpl::ServiceWorkerThread, this);
}

bool CWindowsServiceImpl::OnStop()
{
	// kill service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		TCHAR pTemp[121];
		LOG(INFO)<<"OpenSCManager failed, error code = %d";
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, s_ServiceImpl->GetName(), SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();	
			LOG(INFO)<<"OpenService failed, error code"<<nError;
		}
		else
		{
			// call ControlService to kill the given service
			SERVICE_STATUS status;
			if(ControlService(schService,SERVICE_CONTROL_STOP,&status))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				LOG(INFO)<<"Service "<< s_ServiceImpl->GetName()<<" killed";
				return true;
			}
			else
			{
				long nError = GetLastError();
				LOG(INFO)<<"ControlService failed, error code = "<< nError;
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

#if 0
HANDLE CWindowsServiceImpl::OnThreadInit()
{
	LOG(INFO)<<"CWindowsServiceImpl NamedPipeInit Begin~"<<endl;

	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\haihu_pipe"); 
	//security attribute setting begin
	SECURITY_ATTRIBUTES sa;
	sa.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(
		SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(sa.lpSecurityDescriptor, 
		SECURITY_DESCRIPTOR_REVISION);
	// ACL is set as NULL in order to allow all access to the object.
	SetSecurityDescriptorDacl(sa.lpSecurityDescriptor, TRUE, NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	//security attribute setting end

	// Create the named pipe.
	HANDLE hPipe = CreateNamedPipe(
		lpszPipename,				// The unique pipe name. This string must 
		// have the form of \\.\pipe\pipename
		PIPE_ACCESS_DUPLEX, 		// The pipe is bi-directional; both  
		// server and client processes can read 
		// from and write to the pipe
		PIPE_TYPE_MESSAGE | 		// Message type pipe 
		PIPE_READMODE_MESSAGE | 	// Message-read mode 
		PIPE_WAIT,					// Blocking mode is enabled
		PIPE_UNLIMITED_INSTANCES,	// Max. instances

		// These two buffer sizes have nothing to do with the buffers that 
		// are used to read from or write to the messages. The input and 
		// output buffer sizes are advisory. The actual buffer size reserved 
		// for each end of the named pipe is either the system default, the 
		// system minimum or maximum, or the specified size rounded up to the 
		// next allocation boundary. The buffer size specified should be 
		// small enough that your process will not run out of nonpaged pool, 
		// but large enough to accommodate typical requests.
		BUFFER_SIZE,				// Output buffer size in bytes
		BUFFER_SIZE,				// Input buffer size in bytes

		NMPWAIT_USE_DEFAULT_WAIT,	// Time-out interval
		&sa 						// Security attributes
		);
	if(INVALID_HANDLE_VALUE == hPipe)
	{
		LOG(INFO)<<"Can not create the named pipe "<<lpszPipename<<"Error = "<<GetLastError()<<endl;
		return NULL;
	}

	LOG(INFO)<<"The named pipe "<<lpszPipename<<" is created.\n";

	//wait for the client connection
	LOG(INFO)<<"wait for the client connection... ";
	BOOL bConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

	if (!bConnected)
	{
		LOG(INFO)<<"Error occurred while connecting to the client,GetLastError = "<<GetLastError(); 
		CloseHandle(hPipe);
		return NULL;
	}
	return hPipe;
	
}

unsigned CWindowsServiceImpl::OnThreadRun(HANDLE hPipe)
{
	/////////////////////////////////////////////////////////////////////////
	// Read client requests from the pipe and write the response.
	//
	TCHAR ReadBuffer[BUFFER_SIZE] = {0};
	DWORD ReadBufferSize = 0;
	DWORD ReadDataSize = 0;

	string WriteBuffer = "response from server to client " ;
	DWORD WriteToBufferSize = 0;
	DWORD WrittenDataSize = 0;

	BOOL bResult;

	while (TRUE)
	{
		// Receive one message from the pipe.

		ReadBufferSize = sizeof(TCHAR) * BUFFER_SIZE;
		bResult = ReadFile( 		// Read from the pipe.
			hPipe,					// Handle of the pipe
			ReadBuffer, 			// Buffer to receive data
			ReadBufferSize, 		// Size of buffer in bytes
			&ReadDataSize,			// Number of bytes read
			NULL);					// Not overlapped I/O

		if (!bResult/*Failed*/ || ReadDataSize == 0/*Finished*/) 
		{
			LOG(INFO)<<"Receive Data error!"<<endl;
			break;
		}
		LOG(INFO)<<"Received "<<ReadDataSize<<" bytes"<<" Message = "<<ReadBuffer<<endl;

		// Prepare the response.

		if((WriteBuffer.length() + 1) > BUFFER_SIZE)
		{
			LOG(INFO)<<"response message is too large to send~";
			return -1;
		}
		WriteToBufferSize = sizeof(TCHAR) * (WriteBuffer.length()+1);
		LOG(INFO)<<"WriteToBufferSize = "<<WriteToBufferSize<<endl;
		// Write the response to the pipe.

		bResult = WriteFile(		// Write to the pipe.
			hPipe,					// Handle of the pipe
			WriteBuffer.c_str(),		// Buffer to write to 
			WriteToBufferSize,			// Number of bytes to write 
			&WrittenDataSize,		// Number of bytes written 
			NULL);					// Not overlapped I/O 

		if(!bResult)
		{
			LOG(INFO)<<"WriteFile Data error!"<<endl;
			break;
		}

		LOG(INFO)<<"Send "<<WrittenDataSize<<" bytes"<<" Message = "<<WriteBuffer<<endl;
		Sleep(200);
	}

	FlushFileBuffers(hPipe); 
	DisconnectNamedPipe(hPipe); 
	CloseHandle(hPipe);
	return 0;
}


unsigned WINAPI CWindowsServiceImpl::ThreadProc(PVOID context)
{
	ASSERT_RET(context,NULL);
	HANDLE hPipe = OnThreadInit();
	if(NULL == hPipe)
	{
		LOG(INFO) <<"CWindowsServiceImpl in ThreadProc~ pipe init failed!"<<endl;//CreateThread error!
		return -1;
	}
	OnThreadRun(hPipe);
	return 0;
}

void CWindowsServiceImpl::OnStop()
{
    // Log a service stop message to the Application log.
	LOG(INFO) <<"CWindowsServiceImpl in OnStop "<<endl;//CreateThread error!

  	SetEvent(m_hStoppedEvent);
	SetStatus(SERVICE_STOPPED,NO_ERROR,0);
}
#endif
