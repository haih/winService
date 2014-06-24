#include "named_pipe.h"

CNamedPipe::CNamedPipe(HANDLE pipe):m_hPipe_(pipe)
{
	LOG(INFO)<<"CNamedPipe Constructor pass by pipe Handler~";
}

CNamedPipe::CNamedPipe(const char* name )
{
	
	m_name_ = "\\\\.\\pipe\\haihu_pipe";
    LOG(INFO)<<"CNamedPipe Constructor~ "<<"name_ = "<<m_name_;
    Open();
}

CNamedPipe::~CNamedPipe()
{
	LOG(INFO)<<"CNamedPipe Destructor~"<<endl;
	Close();
}

void CNamedPipe::Close()
{
	LOG(INFO)<<"CNamedPipe Close~";
	DisconnectNamedPipe(m_hPipe_); 
	CloseHandle(m_hPipe_); 
	m_hPipe_=NULL;
}

int CNamedPipe::Open()
{
    LOG(INFO)<<"CNamedPipe Open~";
	//security attribute setting begin
	SECURITY_ATTRIBUTES sa;
	sa.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(sa.lpSecurityDescriptor,SECURITY_DESCRIPTOR_REVISION);
	// ACL is set as NULL in order to allow all access to the object.
	SetSecurityDescriptorDacl(sa.lpSecurityDescriptor, TRUE, NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	//security attribute setting end
	
	m_hPipe_ = CreateNamedPipe( 
		(LPSTR)m_name_.c_str(),             // pipe name 
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,       // read/write access 
		PIPE_TYPE_BYTE |  
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // max. instances  
		BUFFER_PIPE_SIZE,                  // output buffer size 
		BUFFER_PIPE_SIZE,                  // input buffer size 
		NMPWAIT_USE_DEFAULT_WAIT,	// Time-out interval
		&sa 						// Security attributes
		);

	
	if (INVALID_HANDLE_VALUE == m_hPipe_)
	{
		LOG(INFO)<<"Can not create the named pipe "<<m_name_<<"Error = "<<GetLastError();
		return RET_ERR;
	}

	LOG(INFO)<<"The named pipe "<<m_name_<<" is Open.\n";
	return RET_OK;
}

CNamedPipe* CNamedPipe::WaitForConnection(unsigned int timeout)
{
    OVERLAPPED lpOverlapped = {0};
    lpOverlapped.hEvent = CreateEvent(0,1,1,0);
    if(ConnectNamedPipe(m_hPipe_, &lpOverlapped)==0)
    {
        if (GetLastError()==ERROR_PIPE_CONNECTED)
            if (!SetEvent(lpOverlapped.hEvent)) 
            {
                LOG(INFO)<<"AsyncWaitForConnection failed";
            }
        int result = WaitForSingleObject(lpOverlapped.hEvent,timeout);
        if (WAIT_OBJECT_0 == result)
        {
            HANDLE client=m_hPipe_;
            Open();
            ResetEvent(lpOverlapped.hEvent);
            return new CNamedPipe(client);
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        LOG(INFO)<<"AsyncWaitForConnection failed";
    }
    ResetEvent(lpOverlapped.hEvent);
}


//Note:调用ConnetNamedPipe时，返回ERROR_PIPE_CONNECTED 535 是表明:There is a process on other end of the pipe.
CNamedPipe* CNamedPipe::WaitForConnection()
{
    if (NULL == m_hPipe_)
    {
        Open();
    }
    
	if (ConnectNamedPipe(m_hPipe_, NULL)||GetLastError()==ERROR_PIPE_CONNECTED)
	{
		HANDLE client = m_hPipe_;
//		Open();
		return new CNamedPipe(client);
	}
	else
	{
		//LOG(INFO)<<"CNamedPipe::WaitForConnection() Error!"<<GetLastError();
		return NULL;
	}

}

void CNamedPipe::InternalReadBytes(void* buf,size_t size)
{
    LOG(INFO)<<"CNamedPipe::InternalReadBytes()!";
	DWORD dwBytesRead = 0;
	BOOL fSuccess = FALSE;
    TCHAR ReadBuffer[BUFFER_SIZE] = {0};
	// Read from the pipe. 
	fSuccess = ReadFile( 
		m_hPipe_, 	   // handle to pipe 
		buf,	// buffer to receive data 
		size, // size of buffer 
		&dwBytesRead, // number of bytes read 
		NULL);		  // not overlapped I/O 
	  
	if (!fSuccess || dwBytesRead == 0)
	{	
		if (GetLastError() == ERROR_BROKEN_PIPE)
		{
			LOG(INFO)<<"pipe disconnected"; 
		}
		else
		{
		    LOG(INFO)<<"read failed!"<<" the fSuccess = "<<fSuccess<<" dwBytesRead = "<<dwBytesRead; 
            return;
		}
	}

    LOG(INFO)<<"read succeed!"<<" dwBytesRead = "<<dwBytesRead ; 
		
}

void CNamedPipe::InternalWriteBytes(const void* buf,size_t size)
{
    LOG(INFO)<<"CNamedPipe::InternalWriteBytes()";
	DWORD dwWritten;
	BOOL fSuccess = FALSE;
    
	fSuccess = WriteFile( 
		m_hPipe_, 	   // handle to pipe 
		buf,	 // buffer to write from 
		size, // number of bytes to write 
		&dwWritten,   // number of bytes written 
		NULL);		  // not overlapped I/O 

    string msg ;
	if (!fSuccess || size != dwWritten)
	{	
		LOG(INFO)<<"WriteFile failed"; 
	}
    else
    {
        msg = (char*)buf;
        LOG(INFO)<<"CNamedPipe::InternalWriteBytes() SendData = "<<msg;
    }
}


#if 0
void CNamedPipe::InternalReadBytes(void* buf,size_t size)
{
    TCHAR ReadBuffer[BUFFER_SIZE] = {0};
	DWORD ReadBufferSize = 0;
	DWORD ReadDataSize = 0;

    BOOL bResult;
    ReadBufferSize = sizeof(TCHAR) * BUFFER_SIZE;
		bResult = ReadFile( 		// Read from the pipe.
			m_hPipe_,					// Handle of the pipe
			ReadBuffer, 			// Buffer to receive data
			ReadBufferSize, 		// Size of buffer in bytes
			&ReadDataSize,			// Number of bytes read
			NULL);					// Not overlapped I/O

		if (!bResult/*Failed*/ || ReadDataSize == 0/*Finished*/) 
		{
			LOG(INFO)<<"Receive Data error!"<<endl;
					
		}
        
        LOG(INFO)<<"Received "<<ReadDataSize<<" bytes"<<" Message = "<<ReadBuffer<<endl;
}

void CNamedPipe::InternalWriteBytes(const void* buf,size_t size)
{
    string WriteBuffer = "response from server to client " ;
    DWORD WriteToBufferSize = 0;
    DWORD WrittenDataSize = 0;

    BOOL bResult;

    if((WriteBuffer.length() + 1) > BUFFER_SIZE)
    {
        LOG(INFO)<<"Response message is too large to send~";
       // return RET_ERR;
    }
    WriteToBufferSize = sizeof(TCHAR) * (WriteBuffer.length()+1);
    LOG(INFO)<<"WriteToBufferSize = "<<WriteToBufferSize<<endl;
    // Write the response to the pipe.

    bResult = WriteFile(        // Write to the pipe.
        m_hPipe_,                  // Handle of the pipe
        WriteBuffer.c_str(),        // Buffer to write to 
        WriteToBufferSize,          // Number of bytes to write 
        &WrittenDataSize,       // Number of bytes written 
        NULL);                  // Not overlapped I/O 

    if(!bResult)
    {
      LOG(INFO)<<"WriteFile Data error!"<<endl;

    }

    LOG(INFO)<<"Send "<<WrittenDataSize<<" bytes"<<" Message = "<<WriteBuffer<<endl;
}
#endif

