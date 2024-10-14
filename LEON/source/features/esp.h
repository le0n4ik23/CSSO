#pragma once
#include "../interfaces/interfaces.h"
#include <string>
#include "../globals.h"
#include "../util/color.h"


namespace ESP
{
	void DrawBoxes(CEntity* player);
	
    void DrawNames(CEntity* player);
	void DrawHealthbars(CEntity* player);
	
	void DrawFlags(vgui::HFont font, CEntity* player);
	void DrawC4(vgui::HFont font);
	void DrawWeapons(CEntity* player, vgui::HFont font);
	void HandleESP();
	void DrawHitmarker(CEntity* player);


		void DrawOffscreenIndicator(CEntity* player);


	bool WorldToScreen(const Vector3& worldPos, Vector3& screenPos);

}

