#ifndef SERVICEINSTALL_H
#define SERVICEINSTALL_H

#include "servicebase.h"

class CServiceManager 
{
public:
	static bool Install(CServiceBase* service);
	static bool Uninstall(CServiceBase* service);
	static bool Run(CServiceBase* service);
	static bool Stop(CServiceBase* service);

private:

};


#endif