#pragma once
#include "../gui.h"
#include "../classes/CEntity.h"
#include "../util/usercmd.h"
#include "../interfaces/gameevent.h"
namespace Misc
{
	void Bhop(UserCmd* cmd);
	void JumpBug(UserCmd* cmd);
	void EdgeBug(UserCmd* cmd);
	void RadarHack();
	void EdgeJump(UserCmd* cmd);
	void oofarr(CEntity* player);
	void Velocity(vgui::HFont font);
	void stamina(vgui::HFont font);
	void Indicators(vgui::HFont indiFont, bool jumpBugKey, bool edgeBugKey, bool edgejumpKey);
	void nevidimajaxuynya(vgui::HFont bigFont);
	void BombInfo(CEntity* player);
	void BombInfo();
	void ClearSpectatorList();
	void SpectatorList();
	void Watermark();
	void Musicdisplay();
	void speclik1(vgui::HFont bigFont);
	void handle_browser();
	void Clan();
}


enum obs_mode
{

	OBS_MODE_NONE,
	OBS_MODE_DEATHCAM,
	OBS_MODE_FREEZECAM,
	OBS_MODE_FIXED,
	OBS_MODE_IN_EYE,
	OBS_MODE_CHASE,
	OBS_MODE_ROAMING,
};

