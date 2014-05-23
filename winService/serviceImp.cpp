#pragma region Includes
#include "serviceImp.h"

#pragma endregion


#define BUFFER_SIZE		4096 // 4K bytes


//HANDLE CWindowsServiceImplm_hStoppedEvent = NULL;

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
     LOG(INFO) <<"WindowsService in OnStart"<<endl;

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
	HANDLE hThread = CreateThread(NULL,0,ThreadProc,NULL,0,NULL);
	if(hThread == NULL)
	{
	   LOG(INFO) <<"WindowsService in OnStart CreateThread error!"<<endl;//CreateThread error!
	   return;
	}
	WaitForSingleObject(hThread,INFINITE);
	while(1)
    {
        // Check whether to stop the service.
        WaitForSingleObject(m_hStoppedEvent, INFINITE);

        SetStatus(SERVICE_STOPPED,NO_ERROR,0);
		return;
    }
     CloseHandle(hThread);
    // Queue the main service function for execution in a worker thread.
//    CThreadPool::QueueUserWorkItem(&CWindowsServiceImpl::ServiceWorkerThread, this);
}
DWORD WINAPI CWindowsServiceImpl::ThreadProc(PVOID context)
{
	LOG(INFO)<<"CWindowsServiceImpl ThreadProc Begin~"<<endl;

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
		return -1;
	}

	LOG(INFO)<<"The named pipe "<<lpszPipename<<" is created.\n";

	//wait for the client connection
	LOG(INFO)<<"wait for the client connection... ";
	BOOL bConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

	if (!bConnected)
	{
		LOG(INFO)<<"Error occurred while connecting to the client,GetLastError = "<<GetLastError(); 
		CloseHandle(hPipe);
		return -1;
	}

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
#if 0		
		if (!bResult/*Failed*/ || WriteToBufferSize != WrittenDataSize/*Failed*/) 
		{
			cout<<"WriteFile Data error!"<<endl;
			break;
		}
#endif	
		LOG(INFO)<<"Send "<<WrittenDataSize<<" bytes"<<" Message = "<<WriteBuffer<<endl;

	}

	FlushFileBuffers(hPipe); 
	DisconnectNamedPipe(hPipe); 
	CloseHandle(hPipe);

	return 0;
}

void CWindowsServiceImpl::OnStop()
{
    // Log a service stop message to the Application log.
	LOG(INFO) <<"CWindowsServiceImpl in OnStop "<<endl;//CreateThread error!
  
	SetStatus(SERVICE_STOPPED,NO_ERROR,0);
}

