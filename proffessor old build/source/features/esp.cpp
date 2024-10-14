#include "esp.h"
#include "../gui.h"
#include <codecvt>


void ESP::DrawBoxes(CEntity* player)
{
	Vector3 bonePos = player->GetBonePos(8);

	Vector3 top;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) + Vector3{ 0.f, 0.f, 8.f }, top))
	{
		return;
	}

	Vector3 bottom;
	if (debugOverlay->ScreenPosition(player->m_vecOrigin - Vector3{ 0.f, 0.f, 6.f }, bottom))
	{
		return;
	}

	surface->SetDrawColor(255, 255, 255);

	const float h = bottom.y - top.y;
	const float w = h * 0.3f;

	const auto left = (top.x - w);
	const auto right = (top.x + w);
	const auto topY = top.y;
	const auto bottomY = bottom.y;

	// Draw main box
	surface->DrawOutlinedRect((int)left, (int)topY, (int)right, (int)bottomY);

	//	// Draw outline
	//	surface->SetDrawColor(0, 0, 0);
	//	surface->DrawOutlinedRect((int)left - 1, (int)topY - 1, (int)right + 1, (int)bottomY + 1);
	//	surface->DrawOutlinedRect((int)left - 2, (int)topY - 2, (int)right + 2, (int)bottomY + 2);
	//	surface->DrawOutlinedRect((int)left - 3, (int)topY - 3, (int)right + 3, (int)bottomY + 3);
	//
	//	if (gui::settings::esp::enableBoxOutline) {
	//		// Draw black outline
	//		surface->SetDrawColor(0, 0, 0);
	//		surface->DrawOutlinedRect((int)left - 1, (int)topY - 1, (int)right + 1, (int)bottomY + 1);
	//		surface->DrawOutlinedRect((int)left - 2, (int)topY - 2, (int)right + 2, (int)bottomY + 2);
	//
	//		// Draw white outline
	//		surface->SetDrawColor(255, 255, 255);
	//		surface->DrawOutlinedRect((int)left + 1, (int)topY + 1, (int)right - 1, (int)bottomY - 1);
	//	}
	//}
}



void ESP::DrawNames(int index, CEntity* player, vgui::HFont font)
{

	player_info_t info;

	engine->GetPlayerInfo(index, &info);


	Vector3 top;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) + Vector3{0.f,0.f, 8.f}, top))
	{
		return;
	}

	Vector3 bottom;
	if (debugOverlay->ScreenPosition(player->m_vecOrigin - Vector3{ 0.f,0.f,6.f }, bottom))
	{
		return;
	}

	int tW, tH;

	std::wstring s = util::ansi2unicode(info.name);
	const wchar_t* text = s.c_str();
	surface->GetTextSize(font, text, tW, tH);

	const float h = bottom.y - top.y;

	const float w = h * 0.3f;

	const auto left = (top.x - w);
	const auto right = (top.x + w);
	const float center = left - (tW / 2);

	surface->DrawTextSurface(font, Vector3{ top.x - (tW / 2), bottom.y - (h + 8), 0.f}, info.name, Color(255,255,255,255));

}

void ESP::DrawHealthbars(CEntity* player, vgui::HFont font)
{
	Vector3 bonePos = player->GetBonePos(8);

	Vector3 top;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) + Vector3{ 0.f,0.f, 8.f }, top))
	{
		return;
	}

	Vector3 bottom;
	if (debugOverlay->ScreenPosition(player->m_vecOrigin , bottom))
	{
		return;
	}
	
	const float h = bottom.y - top.y;
	
	const float w = h * 0.3f;
	

	const auto left = (top.x - w);
	const auto right = (top.x + w);

	float health = (int)(player->m_iHealth);
	float healthFrac = (health / 100.f);
	float barHeight = h * healthFrac;
	

	int tW, tH;
	std::wstring s = util::ansi2unicode(std::to_string((int)health));
	const wchar_t* text = s.c_str();
	surface->GetTextSize(font, text, tW, tH);

	surface->SetDrawColor(0, 0, 0, 255);
	surface->DrawFilledRect((int)left - 5, (int)top.y, (int)left - 2, (int)bottom.y);
	surface->SetDrawColor(100, 255, 100);
	surface->DrawFilledRect((int)left - 4, (int)(top.y + h  - barHeight + 1), (int)left - 3, (int)bottom.y - 1);

	if (player->m_iHealth < 100)
		surface->DrawTextSurface(font, Vector3{ left - 12 - (tW / 2), top.y + (h - barHeight) -5}, std::to_string((int)health).c_str(), Color(255,255,255,255));

}

void ESP::DrawFlashed(vgui::HFont font)
{

	if (!globals::localPlayer)
		return;
	
	if (!globals::localPlayer->m_flFlashDuration)
		return;

		int screenx, screeny;
		engine->GetScreenSize(screenx, screeny);

		int tW, tH;
		const wchar_t* text = L"Flasheaste";
		surface->GetTextSize(font, text, tW, tH);

		surface->DrawTextSurface(font, Vector3{(float)(screenx / 2) - (tW / 2) , (float)screeny / 5}, "Flasheaste", Color(255,255,255,255));

		float w = tW;
		float x = (screenx / 2) - (w), y = screeny / 4;
		float x2 = (screenx / 2) + (w);

		float duration = (globals::localPlayer->m_flFlashDuration);
		float durFrac = (duration / 5.095f);
		float barHeight = (w) * durFrac;

		surface->SetDrawColor(0, 0, 0);

		surface->DrawFilledRect(x, y -3, x2, y + 3);

		surface->SetDrawColor(200, 100, 100);

		surface->DrawFilledRect(x - 1, y - 2, x2 + 1 + - w + barHeight, y + 2);

	
}

void ESP::DrawFlags(vgui::HFont font, CEntity* player)
{
	Vector3 bonePos = player->GetBonePos(8);

	Vector3 top;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) + Vector3{ 0.f,0.f, 8.f }, top))
	{
		return;
	}

	Vector3 bottom;
	if (debugOverlay->ScreenPosition(player->m_vecOrigin, bottom))
	{
		return;
	}

	std::vector<std::pair<std::string, Color>> flags;
	
	if (player->m_ArmorValue > 0)
	{
		if (player->m_bHasHelmet)
			flags.push_back(std::pair<std::string, Color>(("HK"), Color(255,255,255,255)));
		else
			flags.push_back(std::pair<std::string, Color>(("K"), Color(255, 255, 255, 255)));
	}

	if (player->m_bHasDefuser)
		flags.push_back(std::pair<std::string, Color>(("KIT"), Color(46, 112, 255, 255)));

	if (player->m_bIsDefusing)
		flags.push_back(std::pair<std::string, Color>(("DEFUSING"), Color(255, 56, 56, 255)));

	if (player->m_bIsScoped)
		flags.push_back(std::pair<std::string, Color>(("SCOPED"), Color(255, 255, 255, 255)));

	if (player->m_flFlashDuration > 0.f)
		flags.push_back(std::pair<std::string, Color>(("FLASHED"), Color(255, 211, 66, 255)));

	if (player->m_bImmunity)
		flags.push_back(std::pair<std::string, Color>(("IMMUNE"), Color(255, 211, 66, 255)));

	if (globals::playerResource->m_iPlayerC4)
	{
		if (player->GetIndex() == globals::playerResource->m_iPlayerC4)
		{
			flags.push_back(std::pair<std::string, Color>(("C4"), Color(255, 56, 56, 255)));
			if (globals::C4 && globals::C4->m_bStartedArming)
			{
				flags.push_back(std::pair<std::string, Color>(("PLANTING"), Color(255, 56, 56, 255)));
			}
		}	
	}
		


	for (int i = 0; i < flags.size(); ++i)
	{
		auto text = flags[i];
		const float h = bottom.y - top.y;



		const float w = h * 0.3f;


		const auto left = (top.x - w);
		const auto right = (top.x + w);

		static int tW, tH;
		std::wstring s = util::ansi2unicode(text.first);
		const wchar_t* texto = s.c_str();
		surface->GetTextSize(font, texto, tW, tH);

		static int txtH = tH / 1.5;
		surface->DrawTextSurface(font, Vector3{ right , top.y + (txtH * i)}, text.first.c_str(), text.second);
	}



	
}



void ESP::DrawC4(vgui::HFont font)
{
	if (!globals::C4 || globals::C4 != (CC4*)entityList->GetEntityFromClass(ClassID::CC4))
		globals::C4 = (CC4*)entityList->GetEntityFromClass(ClassID::CC4);

	if (!globals::plantedC4 || globals::plantedC4 != (CPlantedC4*)entityList->GetEntityFromClass(ClassID::CPlantedC4))
		globals::plantedC4 = (CPlantedC4*)entityList->GetEntityFromClass(ClassID::CPlantedC4);

	Vector3 pos;
	int tW, tH;
	std::wstring s;
	const wchar_t* text;

	if (globals::playerResource->m_iPlayerC4)
		return;

	if (globals::C4)
	{
		
		if (debugOverlay->ScreenPosition(globals::C4->m_vecOrigin, pos))
		{
			return;
		}


		
		s = util::ansi2unicode("C4");
		text = s.c_str();
		surface->GetTextSize(font, text, tW, tH);

		surface->DrawTextSurface(font, Vector3{ pos.x - (tW / 2), pos.y, 0.f }, "C4", Color(255, 0, 0, 255));
	}

	if (!globals::plantedC4 || !globals::plantedC4->m_bBombTicking)
		return;

	if (debugOverlay->ScreenPosition(globals::playerResource->m_vecC4, pos))
	{
		return;
	}

	s = util::ansi2unicode(std::to_string((int)(globals::plantedC4->m_flC4Blow - globalVars->currentTime)));
	text = s.c_str();
	surface->GetTextSize(font, text, tW, tH);

	surface->DrawTextSurface(font, Vector3{ pos.x - (tW / 2), pos.y, 0.f }, std::to_string((int)(globals::plantedC4->m_flC4Blow - globalVars->currentTime)).c_str(), Color(255, 0, 0, 255));

	if (globals::plantedC4->m_hBombDefuser != 0xffffffff)
	{
		s = util::ansi2unicode(std::to_string((int)(globals::plantedC4->m_flDefuseCountDown - globalVars->currentTime)));
		text = s.c_str();
		surface->GetTextSize(font, text, tW, tH);

		surface->DrawTextSurface(font, Vector3{ pos.x - (tW / 2), pos.y + 10, 0.f }, std::to_string((int)(globals::plantedC4->m_flDefuseCountDown - globalVars->currentTime)).c_str(), Color(84, 190, 255, 255));
	}


}

void ESP::DrawWeapons(CEntity* player, vgui::HFont font)
{
	Vector3 top;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) + Vector3{ 0.f,0.f, 8.f }, top))
	{
		return;
	}

	Vector3 bottom;
	if (debugOverlay->ScreenPosition(player->m_vecOrigin - Vector3{ 0.f,0.f,6.f }, bottom))
	{
		return;
	}

	int tW, tH;

	CBaseCombatWeapon* weapon = player->GetActiveWeapon();
	const char* weapName = weapon->GetWeaponName();
	std::wstring s = util::ansi2unicode(weapName);
	const wchar_t* text = s.c_str();
	surface->GetTextSize(font, text, tW, tH);

	const float h = bottom.y - top.y;

	const float w = h * 0.3f;

	const auto left = (top.x - w);
	const auto right = (top.x + w);
	const float center = left - (tW / 2);

	surface->DrawTextSurface(font, Vector3{ top.x - (tW / 2), bottom.y, 0.f }, weapName, weapon->IsKnife() ? Color(255, 56, 56, 255) : Color(255, 255, 255, 255));

}

//#include <cstdlib>   // for std::wcstombs
//
//void ESP::DrawdropedWeapons(vgui::HFont espfont)
//{
//	for (int classId = ClassID::CWeaponXM1014; classId <= ClassID::CWeaponAWP; ++classId)
//	{
//		CBaseCombatWeapon* weapon = (CBaseCombatWeapon*)entityList->GetEntityFromClass(classId);
//		if (weapon)
//		{
//			Vector3 pos;
//			CEntity* entity = (CEntity*)weapon;
//			if (entity && debugOverlay->ScreenPosition(entity->m_vecOrigin, pos))
//			{
//				const char* weaponName = weapon->GetWeaponName();
//				std::string narrowWeaponName(weaponName);
//
//				// Convert narrow string to wide string
//				size_t convertedChars = 0;
//				size_t bufferSize = narrowWeaponName.length() + 1;
//				wchar_t* wideWeaponName = new wchar_t[bufferSize];
//				mbstowcs_s(&convertedChars, wideWeaponName, bufferSize, narrowWeaponName.c_str(), _TRUNCATE);
//
//				int tW, tH;
//				surface->GetTextSize(espfont, wideWeaponName, tW, tH);
//
//				// Convert wide string back to narrow string for DrawTextSurface
//				char* narrowWeaponNameForDraw = new char[tW + 1];
//				wcstombs_s(&convertedChars, narrowWeaponNameForDraw, tW + 1, wideWeaponName, _TRUNCATE);
//
//				delete[] wideWeaponName;
//
//				surface->DrawTextSurface(espfont, Vector3{ pos.x - (tW / 2), pos.y, 0.f }, narrowWeaponNameForDraw, Color(255, 0, 0, 255));
//
//				delete[] narrowWeaponNameForDraw;
//			}
//		}
//	}
//}
//
//







void ESP::HandleESP(vgui::HFont font)
{
	if (!gui::settings::esp::enable)
		return;



	if (gui::settings::esp::drawc4)
		DrawC4(font);

	

	for (int i = 1; i < 32; ++i)
	{
		CEntity* player = entityList->GetClientEntity(i);

		if (!player)
			continue;
		if (player == globals::localPlayer)
			continue;

		if (!player->IsAlive() || player->IsDormant())
			continue;

		if (player->m_iTeamNum == globals::localPlayer->m_iTeamNum || !player->m_iTeamNum)
			continue;

		if (gui::settings::esp::flags)
			DrawFlags(font, player);

		if (gui::settings::esp::name)
			DrawNames(i, player, font);

		if (gui::settings::esp::health)
			DrawHealthbars(player, font);

		if (gui::settings::esp::weapon)
			DrawWeapons(player, font);
		if (gui::settings::esp::boxes)
			DrawBoxes(player);
	}	/*if (gui::settings::esp::droppedweapons)
			DrawdropedWeapons(font);*/

}