#pragma once
#include "../util/util.h"

class IEngineClientReplay
{
public:
	IGlobalVars* GetGlobalVars()
	{
		return util::get_vfunc<13, IGlobalVars*>(this);
	}


};