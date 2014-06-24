/*------------------------------------------------------*/
/* Work Thread class                                                    
/*                                                   		                   
/*                                                  			     	     
/*                                               		 		      
/* Author                                               			      
/*    HaiHu (haihu@iflytek.com)                                      
/*                                                                               
/* History                                                                      
/*    05/26/2014  Create                                              
/*                                                                                 
/*------------------------------------------------------*/


#ifndef WORKTHREAD_H
#define WORKTHREAD_H


#include "named_pipe.h"
#include <vector>


class CWorkThread
{
public:
	/*!
	* \brief
	* Starts the main loop (waits for new clients and puts them into queue)
	*/
	void Run();
	/*!
	* \brief
	* Start worker theads
	*/
	void Start();
	/*!
	* \brief
	* Pops the client from queue and passes it to handleClient()
	*/
	void workerProc();
	/*!
	* \brief
	* This function must be overriden. It is intended for handling data from clients;
	*/
	virtual void HandleClient(CNamedPipe* client); //Override this to make your own protocol
public:
	/*!
	* \brief
	* Starts the server and all worker theads. You must create a new thread for this func.
	*/
	void Create();
	/*!
	* \brief
	* Stops all worker threads and main thread - run()
	*/
	void Stop();
	/*!
	* \brief
	* Joins all worker theads
	*/
	void Join();
	
	static unsigned WINAPI ThreadProc(void *aPara);
	/*!
	* \brief
	* Write brief comment for NamedPipeServer here.
	* 
	* \param name
	* The name for server pipe
	* 
	* \param thread_pool_size
	* the number of threads for handling clients connection
	*/
	CWorkThread(const std::string& name):name_(name)
	{
        LOG(INFO)<< "CWorkThread::Constructor~ "<<endl;
    }
	virtual ~CWorkThread();
private:
	///the server pipe name 
	std::string name_;
	/// the pointer to server pipe
	CNamedPipe* pipe_;
	///activity. change to false in order to make all threads stop
	bool active_;

	THREAD_HANDLE m_handle_;

    THREAD_ID m_tid_;
	
//	DISALLOW_COPY_AND_ASSIGN(CWorkThread);

};

#endif
