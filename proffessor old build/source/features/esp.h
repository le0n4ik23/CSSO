#pragma once
#include "../interfaces/interfaces.h"
#include <string>
#include "../globals.h"
#include "../util/color.h"
#include "../../ext/imgui/imgui.h"


namespace ESP
{
	void DrawBoxes(CEntity* player, bool drawOutline);
	void DrawBoxes(CEntity* player);
	void DrawNames(int index, CEntity* player, vgui::HFont font);

	void DrawHealthbars(CEntity* player, vgui::HFont font);
	void DrawFlashed(vgui::HFont font);
	void DrawFlags(vgui::HFont font, CEntity* player);
	void DrawC4(vgui::HFont font);
	void DrawWeapons(CEntity* player, vgui::HFont font);
	//void DrawdropedWeapons(vgui::HFont font);
	void HandleESP(vgui::HFont font);
	void Chams();
}
