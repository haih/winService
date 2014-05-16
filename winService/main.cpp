#include "glog/logging.h"  
#pragma comment(lib,"libglog.lib")  
#pragma comment(lib,"libglog_static.lib")

#include "serviceInstall.h"
#include "timerSvc.h"

void LogInit()
{
	// Initialize Google's logging library.    
	google::InitGoogleLogging(NULL);    
	google::SetLogDestination(google::INFO,"E://");    
	// ������־·��  INFO WARNING ERROR FATAL    
	// ...    
	char str[20] = "hello log!";    
	LOG(INFO) << "Found " << google::COUNTER <<endl;    
	LOG(INFO) << str ;//<< " cookies";    
	LOG(WARNING) << "warning test";  // �����һ��Warning��־    
	LOG(ERROR) << "error test";//�����һ��Error��־    
}

int main(int argc, TCHAR* argv[])
{
	// Initialize Google's logging library.    
	google::InitGoogleLogging(argv[0]);    
	google::SetLogDestination(google::INFO,"D://log/");    
	// ������־·��  INFO WARNING ERROR FATAL    
	// ...    
	char str[20] = "hello log!";    
	LOG(INFO) << "Found " << google::COUNTER <<endl;    
	LOG(INFO) << str ;//<< " cookies";    
//	LOG(WARNING) << "warning test";  // �����һ��Warning��־    
//	LOG(ERROR) << "error test";//�����һ��Error��־    
	CServiceBase* Service = new CServiceBase();

	if (argc > 1)
	{
		if (strcmp(argv[1], TEXT("install")) == 0) 
		{
			cout<<"Installing service\n"<<endl;
			if (!CServiceInstaller::Installer(Service)) 
			{
				cout<<"Couldn't install service: "<<::GetLastError()<<endl;
				return -1;
			}
			cout<<"Service Installed~"<<endl;
			return 0;
		}
		if (strcmp(argv[1], TEXT("uninstall")) == 0) 
		{
			cout<<"Uninstall service\n"<<endl;
			if (!CServiceInstaller::Uninstall(Service)) 
			{
				cout<<"Couldn't Uninstall service: "<<::GetLastError()<<endl;
				return -1;
			}
			cout<<"Service Uninstalled~"<<endl;
			return 0;
		}		
	}
	
	Service->Run();
	system("pause");
	return 0;
}