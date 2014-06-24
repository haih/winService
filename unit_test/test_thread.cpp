#include <gtest/gtest.h>

#include "serviceImp.h"
#include "workthread.h"

#define private public

class UT_WorkThread: public testing::Environment
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

TEST(UT_WorkThread, thread)
{
	
	CWorkThread* thread = new CWorkThread();	
	
	EXPECT_NE(NULL, thread->Create());
	//EXPECT_NE(NULL, thread->Join());
	//EXPECT_EQ(6, Foo(30, 18));
}
