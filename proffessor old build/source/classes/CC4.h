#pragma once
#include "../interfaces/interfaces.h"

class CC4
{
public:
	union
	{
		DEFINE_MEMBER_N(Vector3, m_vecOrigin, 0x340);
		DEFINE_MEMBER_N(bool, m_bStartedArming, 0x26f0);
		DEFINE_MEMBER_N(bool, m_bBombPlacedAnimation, 0x26f8);
		DEFINE_MEMBER_N(float, m_fArmedTime, 0x26f4);
		DEFINE_MEMBER_N(bool, m_bIsPlantingViaUse, 0x26f9);
	};
};
