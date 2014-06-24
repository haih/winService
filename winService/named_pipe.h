#ifndef NAMED_PIPE_H
#define NAMED_PIPE_H

#include "define.h" 

const int BUFFER_PIPE_SIZE = 5120;

class CNamedPipe
{
public:
	CNamedPipe(HANDLE pipe);
	CNamedPipe(const char* name);
	~CNamedPipe();
	void Close();
	CNamedPipe* WaitForConnection();
	CNamedPipe* WaitForConnection(unsigned int timeout);
    void InternalReadBytes(void* buf,size_t size);
	void InternalWriteBytes(const void* buf,size_t size);

private:
	int  Open();
	
    

private:
	std::string m_name_;
	HANDLE m_hPipe_;
	size_t m_bufSize_;
	//bool m_server_with_client;
	

};



#endif