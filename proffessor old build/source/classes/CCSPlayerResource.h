#pragma once
#include "../interfaces/interfaces.h"


class CCSPlayerResource
{
public:
	union
	{
		DEFINE_MEMBER_N(bool, m_bBombSpotted, 0xf70);
		DEFINE_MEMBER_N(bool, m_bPlayerSpotted[64], 0xf71);
		DEFINE_MEMBER_N(bool, m_bHostageSpotted[64], 0xfb3);
		DEFINE_MEMBER_N(int, m_iPlayerC4, 0xe40);
		DEFINE_MEMBER_N(Vector3, m_vecC4, 0xe44);
		DEFINE_MEMBER_N(Vector3, m_vecOrigin, 0x340);

	};
};