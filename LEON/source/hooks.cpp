	#include "hooks.h"
#include "classes/singleton.h"
	#include "intrin.h"
	#include "gui.h"
	#include "../ext/imgui/imgui_impl_win32.h"
	#include "../ext/imgui/imgui_impl_dx9.h"
	#include "../ext/imgui/imgui.h"
	#include "WinUser.h"
	#include <Windows.h>
#include <stdarg.h> // Заголовочный файл для работы с переменным числом аргументов
#include <stdio.h> // Заголовочный файл для работы с функциями ввода-вывода
#include <iostream>
#include <windows.h>
#include <cstring>
#include <cstdint>

	class IClientRenderable;
	class UserCmd;

	
	

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

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f; // Замените 60 на вашу целевую частоту кадров

		
		gui::Render();

		return result;
	}

	void ClientCmdPrint(const char* format, ...)
	{
		// Буфер для форматированной строки
		char buffer[1024];

		// Добавляем "echo" к началу строки
		snprintf(buffer, sizeof(buffer), "echo \"%s\"", format);

		// Используем функцию vsnprintf для форматирования остальной части строки с переменным числом аргументов
		va_list args;
		va_start(args, format);
		vsnprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), format, args);
		va_end(args);

		// Отправляем полученную строку в консоль игры с помощью ClientCmd
		engine->ClientCmd(buffer);
	}

	

	void WriteMemory(std::uint8_t* address, bool value) {
		DWORD oldProtect;
		VirtualProtect(address, sizeof(value), PAGE_EXECUTE_READWRITE, &oldProtect);
		*reinterpret_cast<bool*>(address) = value;
		VirtualProtect(address, sizeof(value), oldProtect, &oldProtect);
	}


	void GatherPlayers() {
		hooks::playerList.clear();  // Очищаем список перед заполнением

		for (int i = 1; i <= engine->GetMaxClients(); ++i) {
			CEntity* player = entityList->GetClientEntity(i);
			if (player && player->IsPlayer()) {  // Проверяем, что это игрок
				hooks::playerList.push_back(player);  // Добавляем указатель на игрока в список
			}
		}
	}


	static bool setupdone = false;
	bool __fastcall CreateMoveHook(void* ecx, void*, float frameTime, UserCmd* cmd)
	{
		const bool result = CreateMoveOriginal(g_ClientMode, frameTime, cmd);
		
		if (!cmd || !cmd->commandNumber)
			return result;
		if (!globalVars) {
			// Инициализация объекта globalVars, если он не был инициализирован ранее
			globalVars = new IGlobalVars();
		}
		hooks::currentcmd = cmd;
		
		
		auto currentViewAngles{cmd->viewAngles};
	

		

		
		
	
		
		float currtime = globalVars->tickCount;
		globals::localPlayer = entityList->GetClientEntity(engine->GetLocalPlayer());
		globals::localPlayer->m_flFlashMaxAlpha = 50.f;
		
		
		
		if (engine->IsInGame())
		{
			if (gui::settings::misc::needset)
			{
				if (gui::settings::esp::disableshadows) { engine->ClientCmd("r_shadows 0"); }
				if (gui::settings::esp::disablephysics) { engine->ClientCmd("cl_ragdoll_physics_enable 0"); }
				if (gui::settings::esp::disablepostprocessing) { engine->ClientCmd("mat_postprocess_enable 0"); }
				if (gui::settings::esp::disableblood) { engine->ClientCmd("violence_hblood 0");  engine->ClientCmd("violence_ablood 0"); }
				if (gui::settings::esp::disable3dsky) { engine->ClientCmd("r_3dsky 0"); }
				if (gui::settings::misc::needswayset)
				{
					std::string command = "cl_wpn_sway_scale " + std::to_string(gui::settings::misc::sway);
					engine->ClientCmd(command.c_str());

					gui::settings::misc::needswayset = false;
				}

				if (gui::settings::misc::neednightmodeset)
				{
					std::string command = "mat_force_tonemap_scale " + std::to_string(gui::settings::misc::nightmode);
					engine->ClientCmd(command.c_str());

					gui::settings::misc::neednightmodeset = false;
				}




				gui::settings::misc::needset = false;
			}
			else
			{
				gui::settings::misc::needset = true;
			}
		 engine->GetviewAngles(hooks::realangles);
		 hooks::oldangles = cmd->viewAngles;
		
		 GatherPlayers();
		 hooks::setup = true;
			if (gui::settings::misc::hitsound)
			{
				
				static int oldHits = 0;
				if (hooks::hits > 0 && oldHits < hooks::hits) {
					oldHits = hooks::hits;
					engine->ClientCmd("playgamesound Alert.WarmupTimeoutBeep");
				}
			}
			
			if (gui::settings::aimbot::silent || gui::settings::aimbot::antiaim)
			{
				Vector3 angles = cmd->viewAngles;
				angles.z = 0.f;
				engine->SetviewAngles(angles);
			}
			if (gui::settings::misc::infospam)
			{
				
				Misc::AutoAlign();
				Misc::Align(cmd);
			}
			
			if (gui::settings::esp::fogenable)
			{
				Misc::FindFogControllers();
			}
			
				
			
			if (globals::localPlayer->IsAlive())
			{
				CBaseCombatWeapon* weapon = globals::localPlayer->GetActiveWeapon();
				float fov = Aimbot::GetMaxFov(weapon);
				float smooth = Aimbot::GetSmoothFactor(weapon);
				std::map<int, int> hitboxMapping = Aimbot::GetHitboxMapping(weapon);
				
				if (gui::settings::misc::faststop) {
					Misc::FastStop(cmd);
				}
			
				c_engine_prediction::Get().start(cmd, globals::localPlayer);
				Aimbot::Aim(cmd, fov, smooth, hitboxMapping);
				
				
				
				Aimbot::AutoPeek(cmd);
					if ((GetAsyncKeyState(VK_SPACE)) && ((!IsKP(gui::settings::binds::jumpbug))))
					{


						Misc::Bhop(cmd);



					}
					if ((gui::settings::binds::jumpbug)) {
						if (IsKP(gui::settings::binds::jumpbug)) {
							Misc::JumpBug(cmd);
						}
					}
					if ((gui::settings::binds::jumpbug)) {
						if (IsKP(gui::settings::binds::longjump)) {
							Misc::EdgeJump(cmd);
						}
					}
					if ((gui::settings::binds::minijump)) {
						if (IsKP(gui::settings::binds::minijump)) {
							Misc::MiniJump(cmd);
						}
					}

					
			
				Aimbot::FakeLag(cmd, hooks::bSendPacket);

				
				if (gui::settings::aimbot::antiaim)
				{

					Aimbot::AA(cmd, hooks::bSendPacket );
					Misc::MovementCorrection(hooks::realangles, cmd);
				}
				if (!gui::settings::aimbot::silent && !gui::settings::aimbot::antiaim)
				{
					Vector3 angles = cmd->viewAngles;
					angles.z = 0.f;
					engine->SetviewAngles(angles);
				}
			
			

			
				c_engine_prediction::Get().end(cmd, globals::localPlayer);
			}
		
			
			if (gui::settings::misc::edgebug && IsKP(gui::settings::binds::edgebug))
			{
				Misc::EdgeBug(cmd);
			}
			Vector3 backup_viewangles = cmd->viewAngles;
			cmd->viewAngles = backup_viewangles;
		}
		else 
		{
			hooks::bSendPacket = true;
			hooks::setup = false;
			
		}
		if (globals::playerResource != (CCSPlayerResource*)entityList->GetEntityFromClass(ClassID::CCSPlayerResource))
			globals::playerResource = (CCSPlayerResource*)entityList->GetEntityFromClass(ClassID::CCSPlayerResource);

		

		return false;
	}

	bool matPanelFound = false;
	int matSystemTopPanel;
	
	
	void __stdcall PaintTraverseHook(uintptr_t panel, bool forceRepaint, bool allowForce)
	{

		if (gui::settings::esp::disablezoom && !strcmp("HudZoom", vguiPanel->GetName(panel)))
			return;
		PaintTraverseOriginal(vguiPanel, panel, forceRepaint, allowForce);
		engine->GetScreenSize(hooks::screenwidth, hooks::screenheight);
		const char* panelName = vguiPanel->GetName(panel);
		
		static int screenx = hooks::screenheight;
		static int screeny = hooks::screenwidth;
		static int topPanelID = 0;
		
		if (!topPanelID)
		{
			if (!strcmp(panelName, "MatSystemTopPanel"))
			{
				topPanelID = panel;
				
			}
		}
		
		else if (panel == topPanelID)
		{
			
		
			if (gui::MenuOpen)
			{
				vguiPanel->SetMouseInputEnabled(panel, true);
			}
			
			

			
		
		
			static vgui::HFont espFont;
			static vgui::HFont bigFont;
			static vgui::HFont flagFont;
			static vgui::HFont veloFont;
		
			
			if (gui::settings::misc::watermark || gui::settings::misc::spectatorlist || gui::MenuOpen || gui::settings::misc::velocity || gui::settings::misc::stamina )
			{

				Misc::Watermark();


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
	

	
	void __stdcall DrawModelHook(void* results, const CDrawModelInfo& info, CMatrix3x4* bones, float* flexWeights, float* flexDelayedWeights, const Vector3& modelOrigin, const std::int32_t flags)
	{
		if (!engine->IsInGame()) {
			DrawModelOriginal(studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
			return;
		}

		if (globals::localPlayer && info.renderable) {
			CEntity* entity = info.renderable->GetIClientUnknown()->GetBaseEntity();
			bool isLocalPlayer = entity == globals::localPlayer;
			bool isEnemy = entity->IsPlayer() && entity->m_iTeamNum != globals::localPlayer->m_iTeamNum && !entity->m_bImmunity;

			const char* materialName = (gui::settings::misc::chamstype == 0) ? "debug/debugambientcube" : "debug/debugmrmwireframe";
			IMaterial* material = matSystem->FindMaterial(materialName);

			if (isLocalPlayer || isEnemy) {
				if (isLocalPlayer) {
					if (gui::settings::chams::self) {
						static auto startTime = std::chrono::steady_clock::now();
						static float blinkInterval = 1.0f;
						static float minAlpha = 0.1f;
						static float maxAlpha = 1.0f;
						auto now = std::chrono::steady_clock::now();
						std::chrono::duration<float> elapsed = now - startTime;
						float phase = std::fmod(elapsed.count() / blinkInterval * 2.0f * 3.14159f, 2.0f * 3.14159f);
						float alphaValue = minAlpha + (maxAlpha - minAlpha) * (0.5f * (1.0f + std::sin(phase)));
						float localPlayerColor[4] = { gui::settings::chams::selfcolor.x, gui::settings::chams::selfcolor.y, gui::settings::chams::selfcolor.z, alphaValue };

						// Настройка материала для чамсов
						material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
						
						
						studioRender->ForcedMaterialOverride(material);

						// Рисуем модель с чамсами
						DrawModelOriginal(studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);

						
						return;
					}
				}
				else if (isEnemy) {
					if (gui::settings::chams::invisible) {
						material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						material->SetMaterialVarFlag(MATERIAL_VAR_NOCULL, true);
						material->SetMaterialVarFlag(MATERIAL_VAR_NOFOG, true);
						material->SetMaterialVarFlag(MATERIAL_VAR_HALFLAMBERT, true);
						float hiddenColorArray[4] = { gui::settings::chams::hiddencolor.x, gui::settings::chams::hiddencolor.y, gui::settings::chams::hiddencolor.z, gui::settings::chams::hiddencolor.w };
						studioRender->SetColorModulation(hiddenColorArray);
						studioRender->SetAlphaModulation(1.0f);
						studioRender->ForcedMaterialOverride(material);

						DrawModelOriginal(studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
					}

					if (gui::settings::chams::enable) {
						material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
						float visibleColorArray[4] = { gui::settings::chams::visiblecolor.x, gui::settings::chams::visiblecolor.y, gui::settings::chams::visiblecolor.z, gui::settings::chams::visiblecolor.w };
						studioRender->SetColorModulation(visibleColorArray);
						studioRender->SetAlphaModulation(1.0f);
						studioRender->ForcedMaterialOverride(material);

						DrawModelOriginal(studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
					}

					studioRender->ForcedMaterialOverride(nullptr);
					return;
				}
			}
		}

		// Рисуем основную модель, если не применяются чамсы
		DrawModelOriginal(studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
	}

	void __fastcall SceneEndHook(void* thisptr, void* ecx)
	{
		SceneEndOriginal(renderView);

		if (!globals::localPlayer || (!gui::settings::chams::enable && !gui::settings::chams::invisible && !gui::settings::chams::self))
			return;

		const char* materialName = (gui::settings::misc::chamstype == 0) ? "debug/debugambientcube" : "debug/debugmrmwireframe";
		IMaterial* material = matSystem->FindMaterial(materialName);

		float hiddenColor[3] = { gui::settings::chams::hiddencolor.x, gui::settings::chams::hiddencolor.y, gui::settings::chams::hiddencolor.z };
		float visibleColor[3] = { gui::settings::chams::visiblecolor.x, gui::settings::chams::visiblecolor.y, gui::settings::chams::visiblecolor.z };

	

		for (auto* entity : hooks::playerList) {
			if (entity && entity->IsAlive() && !entity->IsDormant()) {
				if (entity == globals::localPlayer) {
					if (gui::settings::chams::self) {
						static auto startTime = std::chrono::steady_clock::now();
						static float blinkInterval = 1.0f;
						static float colorArray[3] = { 1.0f, 1.0f, 1.0f };
						static float minAlpha = 0.3f;
						static float maxAlpha = 1.0f;
						auto now = std::chrono::steady_clock::now();
						std::chrono::duration<float> elapsed = now - startTime;
						float phase = fmod(elapsed.count() / blinkInterval * 2.0f * 3.14159f, 2.0f * 3.14159f);
						float alphaValue = minAlpha + (maxAlpha - minAlpha) * (0.5f * (1.0f + sin(phase)));
						float localPlayerColor[3] = { gui::settings::chams::selfcolor.x, gui::settings::chams::selfcolor.y, gui::settings::chams::selfcolor.z };
						material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						material->SetMaterialVarFlag(MATERIAL_VAR_SELFILLUM, true);
						material->SetMaterialVarFlag(MATERIAL_VAR_ADDITIVE, true);
						material->SetMaterialVarFlag(MATERIAL_VAR_MODEL, false);
						renderView->SetColorModulation(localPlayerColor);
						renderView->SetBlend(alphaValue);
						modelRender->ForceMaterialOverride(material);
						entity->DrawModel(0x1);
						modelRender->ForceMaterialOverride(material);
					}
				}
				else if (entity->IsPlayer() && entity->m_iTeamNum != globals::localPlayer->m_iTeamNum) {
					if (gui::settings::chams::invisible) {
						material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						material->SetMaterialVarFlag(MATERIAL_VAR_SELFILLUM, true);
						material->SetMaterialVarFlag(MATERIAL_VAR_ADDITIVE, true);
						material->SetMaterialVarFlag(MATERIAL_VAR_MODEL, false);
						renderView->SetColorModulation(hiddenColor);
						renderView->SetBlend(1.f);
						modelRender->ForceMaterialOverride(material);
						entity->DrawModel(0x1);
					}

					if (gui::settings::chams::enable) {
						material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
						renderView->SetColorModulation(visibleColor);
						renderView->SetBlend(1.f);
						modelRender->ForceMaterialOverride(material);
						entity->DrawModel(0x1);
					}
				}

				modelRender->ForceMaterialOverride(nullptr);
			}
		}
	}


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

#include <iostream> // Для использования std::cerr

	void hooks::SetupHooks()
	{
		// Инициализация библиотеки MinHook
		if (MH_Initialize() != MH_OK) {
			std::cerr << "Failed to initialize MinHook library" << std::endl;
			return;
		}

		// CREATEMOVE
		if (MH_CreateHook(
			(*static_cast<void***>(g_ClientMode))[21],
			&CreateMoveHook,
			reinterpret_cast<void**>(&CreateMoveOriginal)) != MH_OK) {
			std::cerr << "Failed to create hook for CreateMove" << std::endl;
		}

		// PAINTTRAVERSE
		if (MH_CreateHook(
			(*reinterpret_cast<void***>(vguiPanel))[41],
			&PaintTraverseHook,
			reinterpret_cast<void**>(&PaintTraverseOriginal)) != MH_OK) {
			std::cerr << "Failed to create hook for PaintTraverse" << std::endl;
		}

		/*MH_CreateHook(
			(*reinterpret_cast<void***>(studioRender))[29],
			&DrawModelHook,
			reinterpret_cast<void**>(&DrawModelOriginal));
		*/

		// RENDERVIEW
		if (MH_CreateHook(
			(*reinterpret_cast<void***>(renderView))[9],
			&SceneEndHook,
			reinterpret_cast<void**>(&SceneEndOriginal)) != MH_OK) {
			std::cerr << "Failed to create hook for RenderView" << std::endl;
		}

		// LEVELINIT
		if (MH_CreateHook(
			(*reinterpret_cast<void***>(client))[6],
			&LevelInitPostEntityHook,
			reinterpret_cast<void**>(&LevelInitPostEntityOriginal)) != MH_OK) {
			std::cerr << "Failed to create hook for LevelInit" << std::endl;
		}

		// ENDSCENE
		if (MH_CreateHook(
			(*reinterpret_cast<void***>(gui::device))[42],
			&EndSceneHook,
			reinterpret_cast<void**>(&EndSceneOriginal)) != MH_OK) {
			std::cerr << "Failed to create hook for EndScene" << std::endl;
		}

		// Reset
		if (MH_CreateHook(
			(*reinterpret_cast<void***>(gui::device))[16],
			&ResetHook,
			reinterpret_cast<void**>(&ResetOriginal)) != MH_OK) {
			std::cerr << "Failed to create hook for Reset" << std::endl;
		}
		
		// LockCursor
		if (MH_CreateHook(
			(*reinterpret_cast<void***>(surface))[62],
			&LockCursorHook,
			reinterpret_cast<void**>(&LockCursorOriginal)) != MH_OK) {
			std::cerr << "Failed to create hook for LockCursor" << std::endl;
		}

		// Включение всех хуков
		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
			std::cerr << "Failed to enable hooks" << std::endl;
		}

		// Уничтожение DirectX
		gui::DestroyDirectX();
	}


	void hooks::RemoveHooks()
	{
		MH_DisableHook(MH_ALL_HOOKS);
		MH_RemoveHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}