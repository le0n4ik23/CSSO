#include "CEntity.h"
#ifndef TRACE_MASK_SHOT
#define TRACE_MASK_SHOT (0x4600400B)
#endif
CBaseCombatWeapon* CEntity::GetActiveWeapon()
{
	return (CBaseCombatWeapon*)(entityList->GetClientEntityFromHandle(this->m_hActiveWeapon));
}

bool CEntity::IsVisible(int bone)
{
    CMatrix3x4 bones[128];

    if (!this->SetupBones(bones, 128, 256, globalVars->currentTime))
        return false;

    Vector3 aimPunch = globals::localPlayer->m_aimPunchAngle;

    Vector3 vel = globals::localPlayer->m_vecVelocity;

    Ray_t ray;
    trace_t trace;
    CTraceFilter filter(globals::localPlayer);
    ray.Init(globals::localPlayer->GetEyePos(), bones[bone].Origin());

    engineTrace->TraceRay(ray, 0x4600400B, &filter, &trace);


    if (!trace.entity)
        return false;

    bool close = (this->m_vecOrigin - globals::localPlayer->m_vecOrigin).Length2D() < 140;

    if (close ? trace.fraction < 0.50f : trace.fraction < 0.97f)
        return false;

    return true;
}
