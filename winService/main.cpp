#include "serviceInstall.h"
#include "timerSvc.h"

void LogInit(int argc, TCHAR* argv[])
{
	// Initialize Google's logging library.    
	google::InitGoogleLogging(argv[0]);    
	google::SetLogDestination(google::INFO,"D://svclog/");    
	// 设置日志路径  INFO WARNING ERROR FATAL    
   
	//实时输出日志信息
	FLAGS_logbufsecs = 0;
}

int main(int argc, TCHAR* argv[])
{
	// Initialize Google's logging library.    
	
	CTimerService* TimeService = new CTimerService();
	if (argc > 1)
	{
		if (strcmp(argv[1], TEXT("install")) == 0) 
		{
			cout<<"Installing service\n"<<endl;
			if (!CServiceInstaller::Installer(TimeService)) 
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
			if (!CServiceInstaller::Uninstall(TimeService)) 
			{
				cout<<"Couldn't Uninstall service: "<<::GetLastError()<<endl;
				return -1;
			}
			cout<<"Service Uninstalled~"<<endl;
			return 0;
		}		
	}
	LogInit(argc,argv);
	TimeService->Run();
	system("pause");
	return 0;
}