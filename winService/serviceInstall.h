#ifndef SERVICEINSTALL_H
#define SERVICEINSTALL_H

#include "servicebase.h"

class CServiceInstaller 
{
public:
	static bool Install(const CServiceBase& service);
	static bool Installer(CServiceBase* service);
	static bool Uninstall(CServiceBase* service);
private:

};


#endif