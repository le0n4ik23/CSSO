#include "hooks.h"
#include "intrin.h"
#include "../ext/imgui/imgui_impl_win32.h"
#include "../ext/imgui/imgui_impl_dx9.h"
#include "../ext/imgui/imgui.h"
#include "WinUser.h"
#include "interfaces/gameevent.h"
#include "../ext/discord/discord_rpc.h"
#include "prediction/engine_prediction.hpp"

class IClientRenderable;



HRESULT __stdcall ResetHook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = ResetOriginal(device, device, params);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}

long __stdcall EndSceneHook(IDirect3DDevice9* device)
{

	static const auto returnAddress = _ReturnAddress();

	const auto result = EndSceneOriginal(device, device);

	if (_ReturnAddress() == returnAddress)
		return result;
	if (!gui::setup)
		gui::SetupMenu(device);
		gui::Render();

}

bool __fastcall CreateMoveHook(void* ecx, void*, float frameTime, UserCmd* cmd)
{
	const bool result = CreateMoveOriginal(g_ClientMode, frameTime, cmd);

	if (!cmd || !cmd->commandNumber)
		return result;

	bool bSendPacket = cmd->commandNumber % 2;

	float currtime = globalVars->currentTime;
	/*prediction::start(cmd);
	prediction::end();*/
	globals::localPlayer = entityList->GetClientEntity(engine->GetLocalPlayer());
	globals::localPlayer->m_flFlashMaxAlpha = 50.f;

	if (gui::settings::aimbot::silent)
	{
		Vector3 angles = cmd->viewAngles;
		angles.z = 0.f;
		engine->SetViewAngles(angles);
	}

	if (engine->IsInGame() && globals::localPlayer->IsAlive())
	{
		// Check if the key for JumpBug (XBUTTON1) or EdgeBug (XBUTTON2) is pressed
		bool jumpBugKey = GetAsyncKeyState(VK_XBUTTON1) & 0x8000;
		bool edgeBugKey = GetAsyncKeyState(VK_XBUTTON2) & 0x8000;
		bool edgejumpKey = GetAsyncKeyState(VK_MBUTTON) & 0x8000;
		// If either JumpBug or EdgeBug key is pressed, show the indicators
		static vgui::HFont bigFont; // Initialize bigFont if needed
		Misc::Indicators(bigFont, jumpBugKey, edgeBugKey, edgejumpKey);

		if (jumpBugKey)
		{
			Misc::JumpBug(cmd);
		}

		if (edgeBugKey)
		{
			Misc::EdgeBug(cmd);
		}
		if (edgejumpKey)
		{
			Misc::EdgeJump(cmd);
		}

		if ((GetAsyncKeyState(VK_SPACE)))
		{
			Misc::Bhop(cmd);
		}
		Misc::RadarHack();
		if ((GetAsyncKeyState(VK_LBUTTON)))
		{
			Aimbot::Aim(cmd);
		}

		// Auto-fire functionality when mouse button 5 is held down
		if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) {
			// Mouse button 5 is pressed
			// For example:
			if (!gui::settings::aimbot::silent) {
				// Simulate rapid firing by repeatedly setting the attack button
				cmd->buttons |= IN_ATTACK;
			}
		}

		if (!gui::settings::aimbot::silent)
		{
			Vector3 angles = cmd->viewAngles;
			angles.z = 0.f;
			engine->SetViewAngles(angles);
		}

	}

	if (globals::playerResource != (CCSPlayerResource*)entityList->GetEntityFromClass(ClassID::CCSPlayerResource))
		globals::playerResource = (CCSPlayerResource*)entityList->GetEntityFromClass(ClassID::CCSPlayerResource);

	return false;
}





bool matPanelFound = false;
int matSystemTopPanel;

void __stdcall PaintTraverseHook(uintptr_t panel, bool forceRepaint, bool allowForce)
{
	PaintTraverseOriginal(vguiPanel, panel, forceRepaint, allowForce);
	const char* panelName = vguiPanel->GetName(panel);
	int screenx, screeny;
	engine->GetScreenSize(screenx, screeny);
	static int topPanelID = 0;

	if (!topPanelID)
	{
		if (!strcmp(panelName, "MatSystemTopPanel"))
		{
			topPanelID = panel;
		}
	}

	// Check if topPanelID is found and handle watermark rendering
	if (topPanelID && panel == topPanelID) {
		static vgui::HFont espFont;
		static vgui::HFont bigFont;
		static vgui::HFont flagFont;
		static vgui::HFont indiFont;
		bool jumpBugKey = GetAsyncKeyState(VK_XBUTTON1) & 0x8000;
		bool edgeBugKey = GetAsyncKeyState(VK_XBUTTON2) & 0x8000;
		bool edgejumpKey = GetAsyncKeyState(VK_MBUTTON) & 0x8000;
		if (engine->IsInGame() && globals::localPlayer)
		{
			surface->SetupFont(indiFont, "Verdanabb", 32, 100, 0, 0, vgui::FONTFLAG_DROPSHADOW | vgui::FONTFLAG_ANTIALIAS);

			surface->SetupFont(bigFont, "Arial", 28, 100, 0, 0, vgui::FONTFLAG_DROPSHADOW | vgui::FONTFLAG_ANTIALIAS);
			surface->SetupFont(espFont, "Arial", 12, 100, 0, 0, vgui::FONTFLAG_DROPSHADOW | vgui::FONTFLAG_ANTIALIAS | vgui::FONTFLAG_OUTLINE);
			surface->SetupFont(flagFont, "Arial", 12, 100, 0, 0, vgui::FONTFLAG_DROPSHADOW | vgui::FONTFLAG_ANTIALIAS);
			ESP::DrawFlashed(bigFont);
			ESP::HandleESP(espFont);
			Misc::Velocity(indiFont);
			Misc::stamina(indiFont);
			Misc::Indicators(indiFont, jumpBugKey, edgeBugKey, edgejumpKey);
			Misc::nevidimajaxuynya(bigFont);
			c_discord::get().update();
		}
	}

	if (!topPanelID)
	{
		if (!strcmp(panelName, "HudZoom"))
		{
			topPanelID = panel;
		}
	}
	else if (panel == topPanelID)
	{
		return;
	}

}




void __stdcall DrawModelHook(void* results, const CDrawModelInfo& info , CMatrix3x4* bones, float* flexWeights, float* flexDelayedWeights, const Vector3& modelOrigin, const std::int32_t flags)
{

	if (!engine->IsInGame())
		DrawModelOriginal(studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);


	if (globals::localPlayer && info.renderable)
	{
		CEntity* entity = info.renderable->GetIClientUnknown()->GetBaseEntity();

		if (entity && entity->IsPlayer() && entity->m_iTeamNum != globals::localPlayer->m_iTeamNum && !entity->m_bImmunity)
		{
			static IMaterial* material = matSystem->FindMaterial("debug/debugambientcube");

			ImVec4 visibleColor = ImVec4(gui::settings::chams::visiblecolor.x, gui::settings::chams::visiblecolor.y, gui::settings::chams::visiblecolor.z, gui::settings::chams::visiblecolor.w);
			ImVec4 hiddenColor = ImVec4(gui::settings::chams::hiddencolor.x, gui::settings::chams::hiddencolor.y, gui::settings::chams::hiddencolor.z, gui::settings::chams::hiddencolor.w);
			studioRender->SetAlphaModulation(1.f);

			float hiddenColorArray[4] = { gui::settings::chams::hiddencolor.x, gui::settings::chams::hiddencolor.y, gui::settings::chams::hiddencolor.z, gui::settings::chams::hiddencolor.w };
			material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
			material->SetMaterialVarFlag(MATERIAL_VAR_NOCULL, true);
			material->SetMaterialVarFlag(MATERIAL_VAR_NOFOG, true);
			material->SetMaterialVarFlag(MATERIAL_VAR_HALFLAMBERT, true);
			studioRender->SetColorModulation(hiddenColorArray);

			studioRender->ForcedMaterialOverride(material);
			DrawModelOriginal(studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);


			studioRender->SetAlphaModulation(1.f);
			float visibleColorArray[4] = { gui::settings::chams::visiblecolor.x, gui::settings::chams::visiblecolor.y, gui::settings::chams::visiblecolor.z, gui::settings::chams::visiblecolor.w };
			material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

			studioRender->SetColorModulation(visibleColorArray);
			studioRender->ForcedMaterialOverride(material);
			DrawModelOriginal(studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);


			return studioRender->ForcedMaterialOverride(nullptr);
		}
	}
	DrawModelOriginal(studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
}

void __fastcall SceneEndHook(void* thisptr, void* ecx)
{
	SceneEndOriginal(renderView);

	if (!globals::localPlayer || !gui::settings::chams::visible)
		return;
	ImVec4 visibleColor = ImVec4(gui::settings::chams::visiblecolor.x, gui::settings::chams::visiblecolor.y, gui::settings::chams::visiblecolor.z, gui::settings::chams::visiblecolor.w);
	ImVec4 hiddenColor = ImVec4(gui::settings::chams::hiddencolor.x, gui::settings::chams::hiddencolor.y, gui::settings::chams::hiddencolor.z, gui::settings::chams::hiddencolor.w);
	for (int i = 1; i <= 32; ++i)
	{
		CEntity* entity = entityList->GetClientEntity(i);

		if (entity && entity->IsAlive() && !entity->IsDormant() && entity->m_iTeamNum != globals::localPlayer->m_iTeamNum)
		{

			static IMaterial* material = matSystem->FindMaterial("debug/debugambientcube");
			float hidden[3] = { hiddenColor.x, hiddenColor.y, hiddenColor.z };
			float visible[3] = { visibleColor.x, visibleColor.y, visibleColor.z };
			if (!gui::settings::chams::visible) {

				material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				renderView->SetColorModulation(visible);
				renderView->SetBlend(1.f);
				modelRender->ForceMaterialOverride(material);

				entity->DrawModel(0x1);

			}

			if (!gui::settings::chams::invisible) {
				material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				renderView->SetColorModulation(hidden);


				entity->DrawModel(0x1);

				modelRender->ForceMaterialOverride(material);
			}
		}
	}

}
//
//bool __stdcall hkFireEvent(IGameEvent* pEvent) {
//	static auto oFireEvent = hooks::gameevents_hook.get_original<hooks::FireEvent>(index::FireEvent);
//
//	const char* szEventName = pEvent->GetName();
//
//
//	if (!strcmp(szEventName, "player_hurt"))
//	{
//		int attacker = pEvent->GetInt("attacker");
//		if (engine->GetPlayerForUserID(attacker) == engine->GetLocalPlayer())
//		{
//
//			if (gui::settings::misc::hit_sound)
//			{
//				surface->play_sound("buttons\\arena_switch_press_02.wav");
//			}
//			Misc::hitboxinvo(pEvent);
//		}
//	}
//
//
//	return oFireEvent(g_GameEvents, pEvent);
//}

void __stdcall LevelInitPostEntityHook()
{
	if (globals::playerResource != (CCSPlayerResource*)entityList->GetEntityFromClass(ClassID::CCSPlayerResource))
		globals::playerResource = (CCSPlayerResource*)entityList->GetEntityFromClass(ClassID::CCSPlayerResource);

	LevelInitPostEntityOriginal(client);
}

void __stdcall LockCursorHook()
{
	if (gui::MenuOpen)
	{
		surface->UnlockCursor();
	}
	else
	{
		LockCursorOriginal(surface);
	}
}

void hooks::SetupHooks()
{
	MH_Initialize();

	//CREATEMOVE
	MH_CreateHook(
		(*static_cast<void***>(g_ClientMode))[21],
		&CreateMoveHook,
		reinterpret_cast<void**>(&CreateMoveOriginal)
	);

	//PAINTTRAVERSE
	MH_CreateHook(
		(*reinterpret_cast<void***>(vguiPanel))[41],
		&PaintTraverseHook,
		reinterpret_cast<void**>(&PaintTraverseOriginal)
	);

	MH_CreateHook(
		(*reinterpret_cast<void***>(studioRender))[29],
		&DrawModelHook,
		reinterpret_cast<void**>(&DrawModelOriginal)
	);

	//RENDERVIEW
	MH_CreateHook(
		(*reinterpret_cast<void***>(renderView))[9],
		&SceneEndHook,
		reinterpret_cast<void**>(&SceneEndOriginal)
	);

	//LEVELINIT
	MH_CreateHook(
		(*reinterpret_cast<void***>(client))[6],
		&LevelInitPostEntityHook,
		reinterpret_cast<void**>(&LevelInitPostEntityOriginal)
	);

	//ENDSCENE
	MH_CreateHook(
		(*reinterpret_cast<void***>(gui::device))[42],
		&EndSceneHook,
		reinterpret_cast<void**>(&EndSceneOriginal)
	);

	//Reset
	MH_CreateHook(
		(*reinterpret_cast<void***>(gui::device))[16],
		&ResetHook,
		reinterpret_cast<void**>(&ResetOriginal)
	);

	MH_CreateHook(
		(*reinterpret_cast<void***>(surface))[62],
		&LockCursorHook,
		reinterpret_cast<void**>(&LockCursorOriginal)
	);

	MH_EnableHook(MH_ALL_HOOKS);

	gui::DestroyDirectX();
}

void hooks::RemoveHooks()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}