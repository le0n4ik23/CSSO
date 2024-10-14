	#pragma once
	#include <d3d9.h>
	#include <thread>
#include <cwchar> 
	#include <string> // Для использования строковых переменных
	#include "../ext/imgui/imgui.h"
#include <vector>;
#include <unordered_map>

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
		void InitFonts();
		void InitVeloFont();
		void InitSpecFont();
		
		void SetupMenu(LPDIRECT3DDEVICE9 device) noexcept;
		void Destroy() noexcept;
	

		void Render() noexcept;

		struct Color
		{
			float r, g, b, a;

			Color(float red, float green, float blue, float alpha = 1.0f) : r(red), g(green), b(blue), a(alpha) {}
		};

		
		namespace settings
		{
			namespace binds
			{
				inline  int autopeekbutton;
				inline  int aimKey;
				inline  int leftmanual;
				inline  int rightmanual;
				inline  int backmanual;
				inline  int jumpbug;
				inline  int longjump;
				inline  int minijump;
				inline  int edgebug;
				inline  int forcebaim;
			}
			inline ImDrawList* dlist1;
			inline int activeTab = 0;
			namespace aimbot
			{
				inline int weaptype = 0;
				inline static bool enable = false;
				inline static bool detectAutoPeek = false;
				inline static bool indicatorAutoPeek = false;
				inline bool autofire = false;
				inline bool antiaim = false;	
				inline bool attargets = false;
				inline bool jitter = false;
				inline bool adaptiveautostop = false;
				inline bool forcebaim = false;
				inline bool targetteammates = false;
				inline bool rcs = true;
				inline bool silent = false;
				inline bool autostop = false;
				inline float pistolfov = 00.f;
				inline int pistolsmooth = 0;
				inline int pistolrcs = 100;
				inline float riflefov = 10.f;
				inline int riflesmooth = 0;
				inline int riflercs = 100;
				inline float sniperfov = 10.f;
				inline int snipersmooth = 0;
				inline int sniperrcs = 100;
				inline std::vector<int> pistolhitboxes;
				inline std::vector<int> riflehitboxes;
				inline std::vector<int> sniperhitboxes;
				inline bool fakelags = false;
				inline bool adaptive = false;
				inline bool peek = false;
				
			}

			namespace esp
			{
				inline bool enable = true;
				inline bool removals = false;
				inline bool boxes = true;
				inline bool inner = false;
				inline bool outer = false;
				inline bool name = false;
				inline bool health = false;
				inline bool weapon = false;
				inline bool drawc4 = false;
				inline bool flags = false;
				inline bool fogenable = false;
				inline int espdistance = 2000;
				inline int offscreenradius = 0;
				inline int fogmin = 0;
				inline int fogmax = 0;
				inline int fogdensity = 0;
				inline ImVec4 fogcolor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline bool	healthbarcolorcustom = false;
				inline ImVec4 healthbarcolor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline float length = 0;
				inline bool offscreen = true;
				inline bool alwaysoffscreen = true;
				inline ImVec4 boxcolor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline ImVec4 outoffovcolor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline ImVec4 outoffovicolor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline bool disablezoom = true;
				inline bool disablescope = true;
				inline bool disablephysics = false;
				inline bool disableshadows = false;
				inline bool disablepostprocessing = false;
				inline bool disable3dsky = false;
				inline bool disableblood = false;

			}

			namespace misc
			{
				
				inline bool spectatorlist = false;
				inline bool watermark = false;
				inline static bool needset = false;
				inline bool indicators = false;
				inline bool rageindicators = false;
				inline bool bhop = false;
				inline bool faststop = false;
				inline static bool jumpbug = false;
				inline static bool longjump = false;
				inline static bool minijump = false;
				inline static bool edgebug = false;
				inline static bool autopeek = false;
				inline bool radar = false;
				inline bool bombinfo = false;
				inline bool velocity = false;
				inline bool stamina = false;
				inline bool onpeek = false;
				inline bool infospam = false;
				inline int target = 0;
				inline int chattarget = 0;
				inline int velocityposition = 0;
				inline int maxcolor = 270;
				inline int staminamaxcolor = 30;
				inline int maxalpha = 0;
				inline int pretype = 0;
				inline int velosize = 30;
				inline float sway = 1.6f;
				inline float nightmode = 1.6f;
				inline bool needswayset = false;
				inline bool neednightmodeset = false;
				inline int staminaposition = 0;
				inline int staminafontsize = 0;
				inline int velocityfontsize = 0;
				inline int indicatorposition = 0;
				inline bool updatefont = false;
				inline bool execute = false;
				inline int chamstype = 0;
				inline ImVec4 checkboxColor = ImVec4(0.96f, 0.53f, 0.99f, 1.0f);
				inline ImVec4 velocolor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline ImVec4 velocolor2 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline ImVec4 staminacolor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline ImVec4 staminacolor2 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
			
				inline ImVec4 firstindicatorscolor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline ImVec4 secondindicatorscolor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline ImVec4 precolor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline std::unordered_map<std::string, int> weaponSkinMap;
				inline float edgejump = 0;
				inline float alpha = 0;
				inline  std::string content;
				inline bool fakelags = false;
				inline bool adaptive = false;
				
				inline int fakelagticks = 0;

				//speclist
				inline int specx = 0;
				inline int specy = 0;

				//hitmarker
				inline bool hitmarker = false;
				inline float hitmarkertime = 0.f;
				inline float hitmarkersize = 0.f;
				inline float hitmarkerthickness = 0.0f;
				inline ImVec4 hitmarkercolor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				inline bool hitsound = false;
			}

			namespace chams
			{
				inline bool self = false;
				inline bool enable = false;
				inline bool invisible = false;
				inline ImVec4 selfcolor = ImVec4(0.5f, 0.5f, 0.5f, 0.f);
				inline ImVec4 visiblecolor = ImVec4(0.5f, 0.5f, 0.5f, 0.f);
				inline ImVec4 hiddencolor = ImVec4(0.5f, 0.5f, 0.5f, 0.f);
				inline  uintptr_t randomSeed;
			}
			namespace fonts
			{
				inline ImFont* velocityfont = nullptr;
				inline int velocityfontsize = 30;
				inline int velocityfontflag = 0;
				inline int velocityfontoffset = 0;
				inline ImFont* staminafont = nullptr;
				inline int staminafontsize = 30;
				inline int staminafontflag = 0;
				inline int staminafontoffset = 0;
				inline ImFont* indicatorsfont = nullptr;
				inline int indicatorsfontsize = 30;
				inline int indicatorsfontflag = 0;
				inline int indicatorsfontoffset = 0;
				inline ImFont* spectatorlistfont = nullptr;
				inline int spectatorlistfontsize = 30;
				inline int spectatorlistfontflag = 0;
				inline int spectatorlistfontoffset = 0;
				inline ImFont* nameespfont = nullptr;
				inline int nameespfontsize = 30;
				inline int nameespfontflag = 0;
				inline int nameeespfontoffset = 0;

				inline ImFont* weaponfont = nullptr;

				inline int currentoffset = 0;
				inline int currentsize = 0;
			}
		}
	}

