#pragma once
#include "../interfaces/interfaces.h"

class CPlantedC4
{
public:
	union
	{
		DEFINE_MEMBER_N(bool, m_bBombTicking, 0x2580);
		DEFINE_MEMBER_N(bool, m_bBombDefused, 0x25a0);
		DEFINE_MEMBER_N(float, m_flC4Blow, 0x258c);
		DEFINE_MEMBER_N(float, m_flTimerLength, 0x2590);
		DEFINE_MEMBER_N(float, m_flDefuseCountDown, 0x259c);
		DEFINE_MEMBER_N(float, m_flDefuseLength, 0x2598);
		DEFINE_MEMBER_N(uintptr_t, m_hBombDefuser, 0x25a4);
	};
};