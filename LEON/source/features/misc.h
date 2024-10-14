#pragma once
#include "../gui.h"
#include "../classes/CEntity.h"
#include "../util/usercmd.h"
#include "../interfaces/QAngle.hpp"
#include <unordered_map>
#include <cwchar> 
#include <iostream>
namespace Misc
{
	void FastStop(UserCmd* cmd);
	void Bhop(UserCmd* cmd);
	void JumpBug(UserCmd* cmd);
	void AutoDuck(UserCmd* cmd);
	void Align(UserCmd* cmd);
	bool PredictEdgeBug(UserCmd* cmd, int& edgebugging_tick);
	void LockMovement(UserCmd* cmd, int edgebugging_tick);
	void SetClanTag(const char*);
	void AutoAlign();
	void Indicators();
	void RageIndicators(bool& bsendpacket);
	void EdgeJump(UserCmd* cmd);
	void MiniJump(UserCmd* cmd);
	void Autostrafe(UserCmd* cmd);
	void MovementCorrection(const Vector3& oang, UserCmd* cmd);
	void Watermarkk();
	void ClanTag();
	void iwebzscpec();
	void iwebzframe();
	void PrePred(UserCmd* cmd);
	void EdgeBug(UserCmd* cmd);
	void BombInfo();
	void InfoPanel();
	void FindFogControllers();
	void PlayerDLights();
	void ImguiVelocity();
	void ct();
	void SpectatorList();
	void RadarHack();
	void Watermark();
	
	void Velocity();

	
	

}
inline bool should_jumpbug;