#ifndef _DEFINE_H
#define _DEFINE_H


#include "glog/logging.h"  
#pragma comment(lib,"libglog.lib")  
#pragma comment(lib,"libglog_static.lib")


#include <stdlib.h>
#include <string>
#include <list>
#include <stdint.h>
#include <iostream>
#include <process.h>  
#include <windows.h>


using namespace std;



#define BUFFER_SIZE		4096 // 4K bytes


#define ASSERT(expr)                {if (!(expr)){LOG(INFO)<<"assert  "<< #expr <<" failed"; assert(expr);} }
#define ASSERT_ERR(expr, errinfo)   {if (!(expr)){LOG(INFO)<<"assert "<<  #expr <<" failed"; return errinfo;} }
#define ASSERT_RET(expr, args)      {if (!(expr)){LOG(INFO)<<"assert "<<  #expr <<" failed"; return args;   } }
#define ASSERT_RET_VOID(expr)       {if (!(expr)){LOG(INFO)<<"assert "<<  #expr <<" failed"; return;   } }

#define ASSERT_CONTINUE(expr)       {if (!(expr)){LOG(INFO)<<"assert "<<  #expr <<" failed"; continue;   } }



#define SAFE_DELETE(p)              {if(NULL != p) {delete p; p = NULL;}}
#define SAFE_DELETE_ARRAY(p)        {if(NULL != p) {delete [] p; p = NULL;}}


typedef DWORD THREAD_ID;
typedef HANDLE THREAD_HANDLE;
typedef HANDLE PIPE_HANDLE;



/* Standard "it worked" return value */
typedef int RESULT;

#define RET_OK                  0
#define RET_ERR                -1


#endif

