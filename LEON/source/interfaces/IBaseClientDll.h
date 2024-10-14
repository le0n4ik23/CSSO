#pragma once
#include "../util/util.h"
class ClientClass;


class IBaseClientDll
{
public:
	ClientClass* GetAllClasses(void)
	{
		return util::get_vfunc<8, ClientClass*>(this);
	}


};