#include "workthread.h"

void CWorkThread::Run(void)
{
    LOG(INFO)<< "CWorkThread::Run~ "<<endl;
	CNamedPipe* client;
	pipe_ = new CNamedPipe("haihu_pipe");

    LOG(INFO)<< "CWorkThread::Run~ active_ = "<<active_<<endl;
	while (!active_)
	{
		client = pipe_->WaitForConnection();

		if (NULL != client)
		{
		    HandleClient(client);
		}
		else
		{
			//LOG(INFO)<< "CWorkThread::Run wait for connection failed!"<<endl;
		}
		Sleep(20);
	}
    client->Close();
	delete pipe_;
	pipe_ = NULL;
}

unsigned WINAPI CWorkThread::ThreadProc(void *aPara)
{
	CWorkThread *pThread = static_cast<CWorkThread *>(aPara);
	ASSERT_RET(pThread, NULL);

	pThread->Run();

	//pThread->SetStop();
	
	delete pThread;

	return NULL;
}
void CWorkThread::Start(void)
{
    LOG(INFO)<<"CWorkThread::Start~ ";
	active_ = true;
	Create();
}
void CWorkThread::Create(void)
{
    LOG(INFO)<<"CWorkThread::Create~ ";
	m_handle_ = (HANDLE)::_beginthreadex(
			NULL,
			0,
			ThreadProc,
			this,
			0,
			(unsigned int *)(&m_tid_));
	
		if (m_handle_ == 0) 
		{
			LOG(INFO)<<"CWorkThread::Create, _beginthreadex() failed! err="<<GetLastError();
			//return RET_ERR;
		} 
}

void CWorkThread::Stop(void)
{
    LOG(INFO)<<"CWorkThread::Stop~ ";
	active_ = false;
	this->Join();
}

void CWorkThread::Join(void)
{
	if (NULL == m_handle_)
	{
		LOG(INFO)<<"CWorkThread::Join, invailed thread~ ";
		//return RET_ERR;
	}
	
	//if (GetStopFlag())
	//{
	//	LOG(INFO)<<"CWorkThread::Join, thread has stopped normally";
	//	Sleep(10);
	//	return RET_OK;
	//}

	DWORD dwRet = ::WaitForSingleObject(m_handle_,INFINITE);
	if (dwRet == WAIT_OBJECT_0)
	{
		//return RET_OK;
	}
	else 
	{
		LOG(INFO)<<"CWorkThread::Join, WaitForSingleObject() failed! err=" << ::GetLastError();
		//return RET_ERR;
	}
}

CWorkThread::~CWorkThread(void)
{
	this->Stop();
	delete pipe_;
	pipe_ = NULL;
}

void CWorkThread::HandleClient(CNamedPipe* client)
{
    LOG(INFO)<<"CWorkThread::HandleClient~ ";
    ASSERT_RET_VOID(client);
    size_t size = 0;
    
    client->InternalReadBytes(&size,sizeof(size));
    char* message = new char[size];
    //char message[256] = {0};// = new char[size + 1];
    //memset(message,'c',size);
    
    //LOG(INFO)<<"CWorkThread::HandleClient size = " << size;
    
    if (size > 0)
    {
        client->InternalReadBytes(message,size);
        string msg(message,size);	
		LOG(INFO)<<"Message from pipe: "<<msg;
    }
    
    string server_res= "default response from server~";
    size_t respone_size = server_res.size();
    //client->InternalWriteBytes(&respone_size,sizeof(respone_size)+1);
    client->InternalWriteBytes(server_res.c_str(),64);
    
    //Sleep(2000);
    //client->Close();
    
    delete[] message;
}



