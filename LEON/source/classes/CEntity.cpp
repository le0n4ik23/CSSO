#include "CEntity.h"

CBaseCombatWeapon* CEntity::GetActiveWeapon()
{
	return (CBaseCombatWeapon*)(entityList->GetClientEntityFromHandle(this->m_hActiveWeapon));
}

bool CEntity::IsPeekable(int bone)
{
	CMatrix3x4 bones[128];

	if (!this->SetupBones(bones, 128, 256, globalVars->currentTime))
		return false;

	Vector3 aimPunch = globals::localPlayer->m_aimPunchAngle;
	Vector3 eyePos = globals::localPlayer->GetEyePos();

	Vector3 predvel = globals::localPlayer->m_vecVelocity * 0.1;
	
	Vector3 fakelagseyepos = Vector3(eyePos.x + predvel.x, eyePos.y + predvel.y, eyePos.z);
	
	Ray_t flray;
	
	trace_t trace1;
	CTraceFilter filter(globals::localPlayer);
	
	flray.Init(fakelagseyepos, bones[bone].Origin());

	
	engineTrace->TraceRay(flray, 0x4600400B, &filter, &trace1);

	if (!trace1.entity)
		return false;

	

	if (trace1.fraction < 0.85f)
		return false;

	return true;
}
bool CEntity::IsVisible(int bone)
{
	CMatrix3x4 bones[128];

	if (!this->SetupBones(bones, 128, 256, globalVars->currentTime))
		return false;

	Vector3 aimPunch = globals::localPlayer->m_aimPunchAngle;
	Vector3 eyePos = globals::localPlayer->GetEyePos();
	Vector3 vel = globals::localPlayer->m_vecVelocity * 0.01;
	
	Vector3 predictedEyePos = eyePos;
	
	Ray_t ray;
	
	trace_t trace;

	CTraceFilter filter(globals::localPlayer);
	ray.Init(eyePos, bones[bone].Origin());
	

	engineTrace->TraceRay(ray, 0x4600400B, &filter, &trace);

	if (!trace.entity)
		return false;

	bool close = (this->m_vecOrigin - globals::localPlayer->m_vecOrigin).Length2D() < 140;

	if (close ? trace.fraction < 0.50f : trace.fraction < 0.85f)
		return false;

	return true;
}
