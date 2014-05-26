#include "workthread.h"


CWorkThread::CWorkThread()
	: m_Tid(0)
	, m_Handle(INVALID_HANDLE_VALUE)
	, m_bStopFlag(FALSE)
{
}

CWorkThread::~CWorkThread()
{
	CloseHandle(m_Handle);
}

THREAD_ID CWorkThread::GetThreadId()
{
	return m_Tid;
}

THREAD_HANDLE CWorkThread::GetThreadHandle()
{
	return m_Handle;
}

void CWorkThread::Terminate()
{
	CloseHandle(this->GetThreadHandle());
	m_bStopFlag = TRUE;
}

RESULT CWorkThread::Create()
{
	m_Handle = (HANDLE)::_beginthreadex(
			NULL,
			0,
			ThreadProc,
			this,
			0,
			(unsigned int *)(&m_Tid));
		if (m_Handle == 0) 
		{
			LOG(INFO)<<"CWorkThread::Create, _beginthreadex() failed! err="<<GetLastError();
			return RET_ERR;
		} 

}
RESULT CWorkThread::Destory(RESULT aReason)
{
	LOG(INFO)<<"CWorkThread::Destory, Reason = "<<aReason;
	delete this;
	return RET_OK;		
}

RESULT CWorkThread::Join()
{
	if(NULL == m_Handle)
	{
		LOG(INFO)<<"CWorkThread::Join, invailed thread~ ";
		return RET_ERR;
	}
	if(GetStopFlag())
	{
		LOG(INFO)<<"CWorkThread::Join, thread has stopped normally";
		Sleep(10);
		return RET_OK;
	}

	DWORD dwRet = ::WaitForSingleObject(m_Handle,INFINITE);
	if (dwRet == WAIT_OBJECT_0)
	{
		return RET_OK;
	}
	else 
	{
		LOG(INFO)<<"CWorkThread::Join, WaitForSingleObject() failed! err=" << ::GetLastError();
		return RET_ERR;
	}
}

RESULT CWorkThread::OnThreadInit()
{
	LOG(INFO)<<"CWorkThread::OnThreadInit() Begin~"<<endl;

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
	m_PipeHandle = CreateNamedPipe(
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
	if(INVALID_HANDLE_VALUE == m_PipeHandle)
	{
		LOG(INFO)<<"Can not create the named pipe "<<lpszPipename<<"Error = "<<GetLastError()<<endl;
		return RET_ERR;
	}

	LOG(INFO)<<"The named pipe "<<lpszPipename<<" is created.\n";

	//wait for the client connection
	LOG(INFO)<<"wait for the client connection... ";
	BOOL bConnected = ConnectNamedPipe(m_PipeHandle, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

	if (!bConnected)
	{
		LOG(INFO)<<"Error occurred while connecting to the client,GetLastError = "<<GetLastError(); 
		CloseHandle(m_PipeHandle);
		return RET_ERR;
	}
	return RET_OK;

}
RESULT CWorkThread::OnThreadRun()
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
			m_PipeHandle,					// Handle of the pipe
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
			LOG(INFO)<<"Response message is too large to send~";
			return RET_ERR;
		}
		WriteToBufferSize = sizeof(TCHAR) * (WriteBuffer.length()+1);
		LOG(INFO)<<"WriteToBufferSize = "<<WriteToBufferSize<<endl;
		// Write the response to the pipe.

		bResult = WriteFile(		// Write to the pipe.
			m_PipeHandle,					// Handle of the pipe
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

	FlushFileBuffers(m_PipeHandle); 
	DisconnectNamedPipe(m_PipeHandle); 
	CloseHandle(m_PipeHandle);
	return RET_OK;

}

unsigned WINAPI CWorkThread::ThreadProc(void *aPara)
{
	CWorkThread *pThread = static_cast<CWorkThread *>(aPara);
	ASSERT_RET(pThread, NULL);

	pThread->OnThreadInit();

	pThread->OnThreadRun();

	pThread->SetStop();
	
	delete pThread;

	return NULL;
}

