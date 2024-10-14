#include "IClientEntityList.h"

CEntity* IClientEntityList::GetClientEntity(int index)
{
	return util::get_vfunc<3, CEntity*>(this, index);
}

CEntity* IClientEntityList::GetClientEntityFromHandle(uintptr_t handle)
{
	return util::get_vfunc<4, CEntity*>(this, handle);
}

CEntity* IClientEntityList::GetEntityFromClass(int classID)
{
	CEntity* entity = nullptr;
	CEntity* retVal = nullptr;
	for (int i = 1; i < entityList->GetHighestEntityIndex(); ++i)
	{
		entity = entityList->GetClientEntity(i);

		if (!entity)
			continue;

		if (entity->GetClientClass()->classID != classID)
			continue;

		retVal = entity;;
	}


	return retVal;

}

int IClientEntityList::GetHighestEntityIndex()
{
	return util::get_vfunc<6, int>(this);
}
