#pragma once
#include "../util/util.h"
class IMaterial;

class IVModelRender
{
public:
	IMaterial* ForceMaterialOverride(IMaterial* pMaterial, int nOverrideType = 0)
	{
		return util::get_vfunc<1, IMaterial*>(this, pMaterial, nOverrideType);
	}


};