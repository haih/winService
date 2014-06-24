#include <iostream>


#include <gtest/gtest.h>

#include "serviceImp.h"
#include "workthread.h"

#define private public

void LogInit(int argc, TCHAR* argv[])
{
	// Initialize Google's logging library.    
	google::InitGoogleLogging(argv[0]);    
	google::SetLogDestination(google::INFO,"D://svclog/");    
	// 设置日志路径  INFO WARNING ERROR FATAL    

	//实时输出日志信息
	FLAGS_logbufsecs = 0;
}
class ServiceTestEnvironment : public testing::Environment
{
public:
	virtual void SetUp()
	{
	}
	virtual void TearDown()
	{
	}
private:
};

int main(int argc, char* argv[])
{
	
	testing::AddGlobalTestEnvironment(new ServiceTestEnvironment);
	testing::GTEST_FLAG(output) = "xml:./ServiceTest.xml";
    testing::InitGoogleTest(&argc, argv);
	LogInit(argc,argv);
	int ret = (RUN_ALL_TESTS ());
	system("pause");
	return ret;
}