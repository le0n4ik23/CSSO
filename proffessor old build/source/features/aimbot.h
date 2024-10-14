#pragma once
#include "../util/vector.h"

struct UserCmd;
class CBaseCombatWeapon;
class IClientEntityList;
class Vector3;
class CEntity;

namespace Aimbot
{
	//void Triggerbot(UserCmd* cmd) noexcept;

	void Triggerbot(UserCmd* cmd) noexcept;

	void Aim(UserCmd* cmd) noexcept;

	bool IsSafeToAimAndFire(CBaseCombatWeapon* weapon) noexcept;

	void AutoStop(UserCmd* cmd) noexcept;

	void AntiAim(UserCmd* cmd) noexcept;

	//void AutoFire(UserCmd* cmd, CBaseCombatWeapon* weapon) noexcept;

	int GetBestBone(CEntity* ent);
}

Vector3 VelocityExtrapolate(CEntity* player, Vector3 aimPos);