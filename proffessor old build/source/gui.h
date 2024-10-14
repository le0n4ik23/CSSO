#pragma once
#include <d3d9.h>
#include <thread>
#include <string>
#include "../ext/imgui/imgui.h"


namespace gui
{
	inline bool MenuOpen = true;

	inline bool setup = false;


	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = {};
	inline WNDPROC originalWindowProcess = nullptr;


	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline LPDIRECT3D9 d3d9 = nullptr;


	bool SetupWindowClass(const char* windowClassName) noexcept;
	void DestroyWindowClass() noexcept;

	bool SetupWindow(const char* windowName) noexcept;
	void DestroyWindow() noexcept;

	bool SetupDirectX() noexcept;
	void DestroyDirectX() noexcept;

	void Setup();

	void SetupMenu(LPDIRECT3DDEVICE9 device) noexcept;
	void Destroy() noexcept;

	void Initialize() noexcept;
	void Render() noexcept;
	void handle_browser();


	namespace settings
	{
		inline int activeTab = 0;
		namespace aimbot
		{
			inline bool enable = false;
			inline bool rcs = true;
			inline bool silent = false;
			inline bool autostop = false;
			inline float fov = 10.f;
			inline int smooth = 0;
			inline int rcsAmmount = 100;
			constexpr float autowallMinDamage = 50.0f;
		}

		namespace esp
		{
			inline bool enable = false;
			inline bool name = false;
			inline bool health = false;
			inline bool weapon = false;
			inline bool droppedweapons = false;
			inline bool boxes = false;
			inline bool drawc4 = false;
			inline bool flags = false;
			inline bool enableBoxOutline = true;

		}

		namespace misc
		{
			inline bool bhop = false;
			inline bool radar = false;
			inline bool velocity = false;
			inline bool stamina = false;
			inline bool indicators = false;
			inline int velocityposition = 755;
			inline int staminaposition = 830;
			inline int indicatorpos = 790;
			inline int spectatorListPosX = 0;
			inline int spectatorListPosY = 0;
			inline bool jumpbug = false;
			inline bool spectatorlist = false;
			inline bool edgejump = false;
			inline bool edgebug = false;
			inline bool watermark = false;
			inline bool hit_sound;
			inline bool hitbones = false;
			inline bool discord = false;
			inline bool bomb = false;
			inline std::string current_browser_page;
			inline bool show_browser_page = false;
			inline bool showLastVelocity = false;
			inline bool spectators_list = false;
			inline ImVec4 specclr = ImVec4(0.96f, 0.53f, 0.99f, 1.0f);
			inline ImVec4 checkboxColor = ImVec4(0.96f, 0.53f, 0.99f, 1.0f);
		}

		namespace chams
		{
			inline bool visible = false;
			inline bool invisible = false;
			inline ImVec4 visiblecolor = ImVec4(0.5f, 0.5f, 0.5f, 0.f);
			inline ImVec4 hiddencolor = ImVec4(0.5f, 0.5f, 0.5f, 0.f);
		}
		namespace configs
		{
			void SaveConfig(const std::string& fileName);
			void LoadConfig(const std::string& fileName);
		}
	}
}

