#pragma once

#include "../util/vector.h"
#include "../hooks.h"
struct UserCmd;
class CBaseCombatWeapon;
class IClientEntityList;
class Vector3;
class CEntity;

namespace Aimbot
{
    // Объявления методов
    float GetMaxFov(CBaseCombatWeapon* weapon);
    float GetWeaponRcs(CBaseCombatWeapon* weapon);
    std::map<int, int> GetHitboxMapping(CBaseCombatWeapon* weapon);
    float GetSmoothFactor(CBaseCombatWeapon* weapon);
    void Triggerbot(UserCmd* cmd) noexcept;
    void Aim(UserCmd* cmd, float maxFov, float smoothFactor, const std::map<int, int>& hitboxMapping) noexcept;
    void AutoPeek(UserCmd* cmd) noexcept;
    void AA(UserCmd* cmd, bool& bSendPacket) noexcept;
    void AutoStop(UserCmd* cmd, CBaseCombatWeapon* weapon) noexcept;
    void ManualIndicator();
    bool CheckEnemyVisibility();
    void AntiAim(UserCmd* cmd) noexcept;
    void FakeLag(UserCmd* cmd, bool& bsendpacket);
    void AutoFire(UserCmd* cmd, CBaseCombatWeapon* weapon) noexcept;
    int GetBestBone(CEntity* ent);
    
    
}

Vector3 VelocityExtrapolate(CEntity* player, Vector3 aimPos);

void CalcAngle(const Vector3& src, const Vector3& dst, Vector3& angles);

