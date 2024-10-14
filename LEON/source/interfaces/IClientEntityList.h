#pragma once
#include "interfaces.h"
#include "../util/util.h"


class CEntity;

class IClientEntityList
{
	//private:
	//	virtual void un0() = 0;
	//	virtual void un1() = 0;
	//	virtual void un2() = 0;
public:
	CEntity* GetClientEntity(int index);
	
	CEntity* GetClientEntityFromHandle(uintptr_t handle);

	CEntity* GetEntityFromClass(int classID);

	int GetHighestEntityIndex();
};


