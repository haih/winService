#ifndef SERVICEBASE_H
#define SERVICEBASE_H

#include "copyDisable.h"

#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iostream>
using namespace std;

class CServiceBase
{
public:
	CServiceBase();
	virtual ~CServiceBase();

	// Called by windows when starting the service.
	bool Run() 
	{
		return RunInternal(this);
	}
	void WriteToEventLog(const TCHAR* msg, WORD type); 
	void SetStatus(DWORD dwState, DWORD dwErrCode = NO_ERROR, DWORD dwWait = 0);

	void Start(DWORD argc, TCHAR* argv[]);
	static DWORD WINAPI Service(LPVOID lpvThread);
	void Stop();
	void Pause();
	void Continue();
	void Shutdown();

	//���������ʵ��
	virtual void OnStart(DWORD argc, TCHAR* argv[])
	{
		 	ofstream ofile;               //��������ļ�
    		ofile.open("d:\\myfile.txt");     //��Ϊ����ļ���
    		ofile<<"123456"<<endl;
	};
	virtual void OnStop(){};
	virtual void OnPause(){};
	virtual void OnContinue(){};
	virtual void OnShutdown(){};
	char* GetName();
	char* GetDisplayName();

	CServiceBase(char* name,
				 char* displayName,
				 DWORD  dwServiceType,
				 DWORD  dwStartType,
				 DWORD  dwErrorCtrlType);
protected:
private:
	// Registers handle and starts the service.
	static void WINAPI SvcMain(DWORD argc, TCHAR* argv[]);

	// Called whenever service control manager updates service status.
	static DWORD WINAPI ServiceCtrlHandler(DWORD ctrlCode, DWORD evtType,
		void* evtData, void* context);

	static BOOL RunInternal(CServiceBase* svc);
	
	//Attribute of the service
	char* m_name;
	char* m_displayName;
	DWORD m_serviceType;
	DWORD m_startType;
	DWORD m_ErrorCtrlType;

	SERVICE_STATUS m_svcStatus;
	SERVICE_STATUS_HANDLE m_svcStatusHandle;

	static CServiceBase* m_service;

	DISALLOW_COPY_AND_ASSIGN(CServiceBase);
};



#endif