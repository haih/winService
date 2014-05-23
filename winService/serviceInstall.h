#ifndef SERVICEINSTALL_H
#define SERVICEINSTALL_H

#include "servicebase.h"

class CServiceInstaller 
{
public:
	static bool Installer(CServiceBase* service);
	static bool Uninstall(CServiceBase* service);
private:

};


#endif