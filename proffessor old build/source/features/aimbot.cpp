#include "aimbot.h"
#include "../interfaces/interfaces.h"
#include "../util/usercmd.h"
#include "../globals.h"
#include "../util/enums.h"
#include "../gui.h"
#include "../interfaces/IClientEntityList.h"
#include "autowall.h"
#include "../classes/CBaseCombatWeapon.h"
#define mask_shot ( 0x1 | 0x4000 | 0x2000000 | 0x2 | 0x4000000 | 0x40000000 )

Vector3 VelocityExtrapolate(CEntity* player, Vector3 aimPos)
{
	return aimPos + (player->m_vecVelocity * globalVars->intervalPerTick);
}



//void Aimbot::Triggerbot(UserCmd* cmd) noexcept
//{
//	/*Vector3 pos = player->GetBonePos(8);*/
//	Vector3 eyePos = globals::localPlayer->GetEyePos();
//	Vector3 aimPunch = globals::localPlayer->m_aimPunchAngle;
//	CBaseCombatWeapon* weapon = globals::localPlayer->GetActiveWeapon();
//	const Vector3 dst = eyePos + (Vector3{ cmd->viewAngles + (aimPunch * 2) }.ToVector()) * 8000.f;
//
//	trace_t trace;
//	CTraceFilter filter(globals::localPlayer);
//	Ray_t ray;
//	ray.Init(eyePos, dst);
//	engineTrace->TraceRay(ray, 0x4600400B, &filter, &trace);
//
//	if (!trace.entity)
//		return;
//
//	if (!trace.entity->IsPlayer() || trace.entity->IsDormant() || !trace.entity->IsAlive() || trace.entity->m_bImmunity)
//		return;
//
//	if (trace.entity->m_iTeamNum == globals::localPlayer->m_iTeamNum || trace.entity->m_iTeamNum == 0)
//		return;
//
//	if (trace.fraction == 1.f)
//		return;
//
//	Vector3 vel = globals::localPlayer->m_vecVelocity;
//	if ((globals::localPlayer->GetActiveWeapon()->m_flNextPrimaryAttack - globalVars->currentTime) <= 0)
//
//		if ((vel.Length2D() <= weapon->GetIdealAccurateSpeed() || cmd->buttons & IN_DUCK) /*&& (globals::localPlayer->GetActiveWeapon()->m_flNextPrimaryAttack - globalVars->currentTime) <= 0*/ && trace.hitbox == 0)
//		{
//			Aimbot::AutoFire(cmd, weapon);
//		}
//
//
//
//	return;
//}


//TODO
void Aimbot::Aim(UserCmd* cmd) noexcept
{
    if (globals::localPlayer->m_bIsDefusing || !gui::settings::aimbot::enable)
        return;

    Vector3 bestAngle{};
    float bestFov{ gui::settings::aimbot::fov * 2 };
    Vector3 aimTarget{};
    float closestFov = FLT_MAX;



    for (int i = 1; i <= 32; ++i)
    {
        CEntity* player = entityList->GetClientEntity(i);

        if (!player)
            continue;

        if (!player->IsAlive() || player->IsDormant() || player->m_bImmunity || player->m_iTeamNum == globals::localPlayer->m_iTeamNum)
            continue;

        if (player->IsVisible(4) || player->IsVisible(8))
        {
            CMatrix3x4 bones[128];

            if (!player->SetupBones(bones, 128, 256, globalVars->currentTime))
                continue;

            Vector3 eyePos = globals::localPlayer->GetEyePos();
            enum Hitbox
            {
                HEAD = 8,
                BODY = 6,
                RIGHT_HAND = 39,
                LEFT_HAND = 13,
                RIGHT_LEG = 73,
                LEFT_LEG = 66,
                RIGHT_FOOT = 74,
                LEFT_FOOT = 67
            };
            Vector3 headpos = bones[8].Origin();
            Vector3 bodypos = bones[6].Origin();

            Vector3 aimPunch = globals::localPlayer->m_aimPunchAngle;

            Vector3 enemyAngle;
            Vector3 bodyAngle = (bodypos - eyePos).ToAngle() - cmd->viewAngles;
            Vector3 headAngle = (headpos - eyePos).ToAngle() - cmd->viewAngles;
            enemyAngle = bodyAngle.LengthSqr() < headAngle.LengthSqr() ? bodyAngle : headAngle;
            const float fov = std::hypot(enemyAngle.x, enemyAngle.y);

            if (fov < closestFov)
            {
                closestFov = fov;
                aimTarget = enemyAngle;
            }

            if (gui::settings::aimbot::rcs && gui::settings::aimbot::rcsAmmount > 0) {
                CBaseCombatWeapon* weapon = player->GetActiveWeapon();
                if (weapon) {
                    const char* weapName = weapon->GetWeaponName();
                     enemyAngle = enemyAngle - (aimPunch * (1.f + (gui::settings::aimbot::rcsAmmount / 100.f)));
                }
            }

            if (const float fov = std::hypot(enemyAngle.x, enemyAngle.y); fov < bestFov)
            {
                bestFov = fov;
                bestAngle = enemyAngle;
            }
        }
    }

    if (gui::settings::aimbot::smooth)
    {
        cmd->viewAngles = cmd->viewAngles + bestAngle.Scale(1.f - ((float)gui::settings::aimbot::smooth / 100.f));
        //Triggerbot(cmd);
    }
    else
    {
        cmd->viewAngles = cmd->viewAngles + bestAngle;
        //Triggerbot(cmd);
    }
}







//void Aimbot::AutoFire(UserCmd* cmd, CBaseCombatWeapon* weapon) noexcept
//{
//	bool pistol = weapon->IsPistol();
//	bool sniper = weapon->IsSniper();
//	if ((weapon->IsSniper() || pistol) && (weapon->m_flNextPrimaryAttack - globalVars->currentTime) <= 0)
//	{
//		cmd->buttons |= IN_ATTACK;
//		return;
//	}
//	if (!pistol && !weapon->IsSniper())
//	{
//		cmd->buttons |= IN_ATTACK;
//		return;
//	}
//
//}

int Aimbot::GetBestBone(CEntity* ent)
{
	return 0;
}

