		#include "../source/gui.h"
#include "../source/config.h"
			#include "../ext/imgui/imgui_impl_win32.h"
			#include "../ext/imgui/imgui_impl_dx9.h"
			#include "../ext/imgui/imgui.h"
#include "../ext/imgui/imgui_freetype.h"
#include "../ext/imgui/imgui_internal.h"
			#include "hooks.h"
#include "config.h"
#include <algorithm>
#include <cmath>
#pragma comment(lib, "Winmm.lib")
#include "interfaces/IGlobalVars.h"
#include <filesystem>
namespace fs = std::filesystem;
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
);

LRESULT CALLBACK WindowProcess(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
);
ImGuiContext* g_ImGuiContext = nullptr;
static bool nullbool = false;
HWND window = NULL;
LPDIRECT3D9 d3d9 = NULL;
LPDIRECT3DDEVICE9 device = NULL;
LPDIRECT3DTEXTURE9 imageTexture = NULL;
IDirect3DTexture9* Logotype = nullptr;
PDIRECT3DTEXTURE9 Logotype2 = nullptr;
ImFont* velocityFont = nullptr;
void playButtonSound() {
	PlaySound(NULL, NULL, SND_PURGE);
	PlaySound(TEXT("C:/1.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

		
			

void BuildFont(float fontSize, const std::string& fontPath, ImFont*& customfont)
{
	ImGuiIO& io = ImGui::GetIO();
	customfont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize);

	// Проверка на успешное добавление шрифта
	if (!customfont) {
		std::cerr << "Failed to load font: " << fontPath << std::endl;
		return;
	}

	// Используем FreeType для построения шрифта
	io.Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType(); // Задаём билдера для использования FreeType
	io.Fonts->FontBuilderFlags = ImGuiFreeTypeBuilderFlags_NoHinting; // Устанавливаем флаги, если нужно

	io.Fonts->Build(); // Строим атлас шрифтов
}


bool reloadFont = false;
		bool gui::SetupWindowClass(const char* windowClassName) noexcept
		{
			windowClass.cbSize = sizeof(WNDCLASSEX);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.lpfnWndProc = DefWindowProc;
			windowClass.cbClsExtra = 0;
			windowClass.cbWndExtra = 0;
			windowClass.hInstance = GetModuleHandle(NULL);
			windowClass.hIcon = NULL;
			windowClass.hCursor = NULL;
			windowClass.hbrBackground = NULL;
			windowClass.lpszMenuName = NULL;
			windowClass.lpszClassName = windowClassName;
			windowClass.hIconSm = NULL;
	
			//register class
			if (!RegisterClassEx(&windowClass))
				return false;

			return true;

		}
		void gui::DestroyWindowClass() noexcept
		{
			UnregisterClass(windowClass.lpszClassName,
				windowClass.hInstance);
		}

	
		


		bool gui::SetupWindow(const char* windowName) noexcept
		{

			window = CreateWindow(
				windowClass.lpszClassName,
				windowName,
				WS_OVERLAPPEDWINDOW,
				0,
				0,
				100,
				100,
				0,
				0,
				windowClass.hInstance,
				0
			);

			if (!window)
				return false;

			return true;
		}

		void gui::DestroyWindow() noexcept
		{
			if (window)
				::DestroyWindow(window);
		}

		bool gui::SetupDirectX() noexcept
		{
			const auto handle = GetModuleHandle("d3d9.dll");
			if (!handle)
				return false;

			using CreateFn = LPDIRECT3D9(__stdcall*)(UINT);

			const auto create = reinterpret_cast<CreateFn>(GetProcAddress(
				handle,
				"Direct3DCreate9"
			));

			if (!create)
				return false;

			d3d9 = create(D3D_SDK_VERSION);

			if (!d3d9)
				return false;

			D3DPRESENT_PARAMETERS params = {};
			params.BackBufferWidth = 0;
			params.BackBufferHeight = 0;
			params.BackBufferFormat = D3DFMT_UNKNOWN;
			params.BackBufferCount = 0;
			params.MultiSampleType = D3DMULTISAMPLE_NONE;
			params.MultiSampleQuality = NULL;
			params.SwapEffect = D3DSWAPEFFECT_DISCARD;
			params.hDeviceWindow = window;
			params.Windowed = 1;
			params.EnableAutoDepthStencil = 0;
			params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
			params.Flags = NULL;
			params.FullScreen_RefreshRateInHz = 0;
			params.PresentationInterval = 0;

			if (d3d9->CreateDevice(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_NULLREF,
				window,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
				&params,
				&device
			) < 0) return false;
		
			return true;
		}
		
		void gui::DestroyDirectX() noexcept
		{
			if (device)
			{
				device->Release();
				device = NULL;
			}

			if (d3d9)
			{
				d3d9->Release();
				d3d9 = NULL;
			}
			
			
		}
		

				void gui::InitFonts() {

					ImFontAtlas* fonts = ImGui::GetIO().Fonts;

					static const ImWchar ranges[] = { 0x0020, 0xFFFF, 0 };
					ImFontConfig config;
					config.MergeMode = false;
					config.PixelSnapH = true;
					fonts->AddFontFromFileTTF("C:/1.ttf", 18.0f, &config, ranges);
				
		
				}
				void InitWeaponFont() {

					BuildFont(36, "C:/hw$/fonts/weapon.ttf", gui::settings::fonts::weaponfont);


				}
				
					void gui::InitVeloFont() {
						ImFontAtlas* velofonts = ImGui::GetIO().Fonts;

						static const ImWchar veloranges[] = { 0x0020, 0xFFFF, 0 };
						ImFontConfig veloconfig;
						veloconfig.MergeMode = false;
						veloconfig.PixelSnapH = true;
					
						velofonts->AddFontFromFileTTF("C:/2.ttf", 30, &veloconfig, veloranges);

						// Запоминаем указатель на шрифт
						gui::settings::fonts::velocityfont = velofonts->AddFontFromFileTTF("C:/2.ttf", 30, &veloconfig, veloranges);
					}
				void gui::InitSpecFont() {
					ImFontAtlas* specfonts = ImGui::GetIO().Fonts;

					


					static const ImWchar specranges[] = { 0x0020, 0xFFFF, 0 };
					ImFontConfig specconfig;
					specconfig.MergeMode = false;
					specconfig.PixelSnapH = true;
					

					specfonts->AddFontFromFileTTF("C:/3.ttf", 18, &specconfig, specranges);

					// Запоминаем указатель на шрифт
					gui::settings::fonts::spectatorlistfont = specfonts->AddFontFromFileTTF("C:/3.ttf", 24, &specconfig, specranges);
				}
				

		void gui::Setup()
		{
			if (!SetupWindowClass("hackClass001"))
				throw std::runtime_error("Failed to create window class.");

			if (!SetupWindow("hotel white$"))
				throw std::runtime_error("Failed to create window.");
		
			if (!SetupDirectX())
				throw std::runtime_error("Failed to create device.");
			
		
		
			DestroyWindow();
			DestroyWindowClass();
		}
		void SetupImGuiStyle()
		{
			ImGuiStyle& style = ImGui::GetStyle();

			ImVec4* colors = style.Colors;
			ImVec4 active = ImVec4(gui::settings::misc::checkboxColor.x, gui::settings::misc::checkboxColor.y, gui::settings::misc::checkboxColor.z, gui::settings::misc::checkboxColor.w);

			colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			colors[ImGuiCol_WindowBg] = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
			colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
			colors[ImGuiCol_Border] = ImVec4(0.35f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_FrameBg] = ImVec4(0.04f, 0.04f, 0.04f, 0.54f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.22f, 0.23f, 0.40f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.67f);
			colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
			colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
			colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			colors[ImGuiCol_Button] = ImVec4(0.04f, 0.04f, 0.04f, 0.40f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 0.50f);
			colors[ImGuiCol_ButtonActive] = active;
			colors[ImGuiCol_Header] = ImVec4(0.03f, 0.03f, 0.03f, 1.0f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.03f, 0.03f, 0.03f, 1.00f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.03f, 0.03f, 0.03f, 1.00f);
			colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
			colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
			colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
			colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
			colors[ImGuiCol_Tab] = ImVec4(0.04f, 0.04f, 0.04f, 0.50f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.04f, 0.04f, 0.04f, 0.80f);
			colors[ImGuiCol_TabActive] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
			colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
			colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
			colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
			colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
			style.WindowMenuButtonPosition = ImGuiDir_None;
			style.FrameRounding = 12.0f;
			style.WindowRounding = 7.0f;
			style.ChildBorderSize = 0.0f;
			style.FrameBorderSize = 0.0f;
			style.SelectableTextAlign = ImVec2(0.5f, 0.5f);
			gui::settings::dlist1 = ImGui::GetBackgroundDrawList();
		}
		void gui::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept
		{
			auto params = D3DDEVICE_CREATION_PARAMETERS{};
			device->GetCreationParameters(&params);

			window = params.hFocusWindow;

			originalWindowProcess = reinterpret_cast<WNDPROC>(
				SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess))
				);

			ImGui::CreateContext();
		
			ImGui::StyleColorsDark();
			
			InitFonts();
			InitVeloFont();
			InitSpecFont();
		
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX9_Init(device);
			SetupImGuiStyle();
			setup = true;
		}

		void gui::Destroy() noexcept
		{
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			SetWindowLongPtr(
				window,
				GWLP_WNDPROC,
				reinterpret_cast<LONG_PTR>(originalWindowProcess)
			);

			DestroyDirectX();
		}


		enum TAB
		{
			VISUALS = 0,
			AIMBOT,
			MISC,
			SKINCHANGER,
		};
	
		static float globalalpha = 0;

#include <unordered_map>
	

		enum Hitbox {
			HEAD = 8,
			BODY = 6,
			RIGHT_HAND = 39,
			LEFT_HAND = 13,
			RIGHT_LEG = 73,
			LEFT_LEG = 66,
			RIGHT_FOOT = 74,
			LEFT_FOOT = 67
		};

		const char* hitboxNames[] = {
	"Head",       // 0
	"Body",       // 1
	"Right Hand", // 2
	"Left Hand",  // 3
	"Right Leg",  // 4
	"Left Leg",   // 5
	"Right Foot", // 6
	"Left Foot"   // 7
		};


	

		void ShowMultiCombo(const char* label, std::vector<int>& selectedHitboxes) {
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4* colors = style.Colors;

			// Сохраняем оригинальный цвет текста
			ImVec4 originalTextColor = colors[ImGuiCol_Text];

			// Устанавливаем новый цвет текста для выбранных элементов
			ImVec4 selectedTextColor = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); // Белый цвет
			colors[ImGuiCol_Text] = selectedTextColor;

			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.04f, 0.04f, 0.04f, 0.40f)); // Фон фрейма
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.50f)); // Фон фрейма при наведении
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.00f, 0.00f, 0.00f, 0.67f)); // Фон фрейма при активации

			if (ImGui::BeginCombo(label, "Select Hitboxes")) {
				for (int i = 0; i < 8; ++i) {
					bool isSelected = std::find(selectedHitboxes.begin(), selectedHitboxes.end(), i) != selectedHitboxes.end();
					if (ImGui::Selectable(hitboxNames[i], isSelected)) {
						if (isSelected) {
							selectedHitboxes.erase(std::remove(selectedHitboxes.begin(), selectedHitboxes.end(), i), selectedHitboxes.end());
						}
						else {
							selectedHitboxes.push_back(i);
						}
					}
				}

				// Восстанавливаем оригинальный цвет текста после отрисовки комбобокса
				colors[ImGuiCol_Text] = originalTextColor;

				ImGui::EndCombo();
			}

			ImGui::PopStyleColor(3); // Возвращаем оригинальные цвета фона фрейма
		}

		void RenderAlphaGradientBar(const ImVec2& p_min)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			float windowWidth = window->Size.x; // Get the width of the current window

			ImVec2 p_max = ImVec2(p_min.x + windowWidth + 3, p_min.y + 2); // Adjust height as needed

			ImColor activeColor(gui::settings::misc::checkboxColor.x, gui::settings::misc::checkboxColor.y, gui::settings::misc::checkboxColor.z, gui::settings::misc::checkboxColor.w);
			const int segments = 100; // Number of segments for the gradient

			for (int i = 0; i < segments; i++)
			{
				float t1 = (float)i / (float)segments;
				float t2 = (float)(i + 1) / (float)segments;

				// Compute the alpha value
				float alpha1 = (1.0f - fabsf(2.0f * t1 - 1.0f)) * 255.0f;
				float alpha2 = (1.0f - fabsf(2.0f * t2 - 1.0f)) * 255.0f;

				// Create colors with the computed alpha values
				ImU32 col1 = ImColor(activeColor.Value.x, activeColor.Value.y, activeColor.Value.z, alpha1 / 255.0f);
				ImU32 col2 = ImColor(activeColor.Value.x, activeColor.Value.y, activeColor.Value.z, alpha2 / 255.0f);

				// Compute the positions
				ImVec2 p1 = ImVec2(p_min.x + (p_max.x - p_min.x) * t1 , p_min.y);
				ImVec2 p2 = ImVec2(p_min.x + (p_max.x - p_min.x) * t2, p_max.y);

				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(p1, p2, col1, col2, col2, col1);
			}
		}

		void RenderCustomListBox(const char* identifier, int* selectedIndex, const std::vector<std::string>& items, float width, float height) {
			static float itemHeight = ImGui::GetTextLineHeightWithSpacing();
			ImVec4 activeColor = ImVec4(0.2f, 0.4f, 0.8f, 1.0f);

			ImGui::BeginChild(identifier, ImVec2(width, height), true);

			for (int i = 0; i < items.size(); ++i) {
				bool isSelected = (*selectedIndex == i);
				if (isSelected) {
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, activeColor);
				}

				if (ImGui::Selectable(items[i].c_str(), isSelected)) {
					*selectedIndex = i;
				}

				if (isSelected) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::EndChild();
		}

		
		static int* oldhits;

		static int checkbbbb = 100;
		struct CheckboxData {
			bool* v;
			bool* rmbClicked;
			float animationTime;
		};

		std::unordered_map<const char*, CheckboxData> checkboxDataMap;

		void CustomCheckbox(const char* label, bool* v, static bool* rmbClicked, ImColor& checkboxColor) {
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
				return;
			
			ImGuiContext& g = *GImGui;
			const ImGuiStyle& style = g.Style;
			const ImGuiID id = window->GetID(label);
			ImVec2 cursorPos = ImGui::GetCursorScreenPos();
			ImVec2 labelSize = ImGui::CalcTextSize(label);
			float checkboxSize = ImGui::GetFrameHeight();
			ImVec2 pressrectmin(cursorPos.x, cursorPos.y);
			ImVec2 pressrectmax(cursorPos.x + checkboxSize + labelSize.x, cursorPos.y + checkboxSize);
			ImRect pressrect(pressrectmin, pressrectmax);
			ImGui::Dummy(ImVec2(pressrectmax.x / 2, checkboxSize)); // Reserve space for the checkbox

			ImVec2 check_bb_min(cursorPos.x, cursorPos.y);
			ImVec2 check_bb_max(cursorPos.x + checkboxSize, cursorPos.y + checkboxSize);
			ImRect check_bb(check_bb_min, check_bb_max);

			ImGui::ItemAdd(check_bb, id);

			bool hovered, held;
			bool labelhovered, labelheld;
			bool pressed = ImGui::ButtonBehavior(pressrect, id, &hovered, &held);

			ImVec2 labelPos = ImVec2(cursorPos.x + checkboxSize + style.ItemInnerSpacing.x, cursorPos.y + (checkboxSize - labelSize.y) * 0.5f);

			bool labelPressed = ImGui::ButtonBehavior(ImRect(labelPos, ImVec2(labelPos.x + labelSize.x, labelPos.y + labelSize.y)), id, &labelhovered, &labelheld);

			if ((pressed || labelPressed) && v != nullptr)
				*v = !*v;
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && rmbClicked != nullptr && (labelhovered || hovered)) {
				*rmbClicked = !*rmbClicked;
			}
			
			// Update color based on state
			ImColor checkColor = *v ? ImColor(gui::settings::misc::checkboxColor.x, gui::settings::misc::checkboxColor.y, gui::settings::misc::checkboxColor.z, globalalpha) : ImColor(50, 50, 50, static_cast<int>(globalalpha * 255));
			ImVec4 colorVec4(0.04f, 0.04f, 0.04f, globalalpha);
			ImColor colorImColor(colorVec4.x * 255, colorVec4.y * 255, colorVec4.z * 255, globalalpha * 255);
			ImColor boxColor = *v ? colorImColor : colorImColor;

			if (pressed || labelPressed)
				checkColor = checkboxColor;
			boxColor = ImColor(10, 10, 10, static_cast<int>(globalalpha * 255));

			float rounding = 6.0f; // Adjust rounding factor as needed
			float thickness = -1.0f; // Fill the rectangle completely

			// Draw rounded rectangle background
			window->DrawList->AddRectFilled(check_bb_min, check_bb_max, boxColor, rounding + 4, ImDrawFlags_RoundCornersAll);

			ImVec2 fillMin = ImVec2(check_bb_min.x + 2, check_bb_min.y + 2); // Offset by 1 pixel
			ImVec2 fillMax = ImVec2(check_bb_max.x - 2, check_bb_max.y - 2); // Offset by 1 pixel

			// Get or initialize CheckboxData for this label
			CheckboxData& checkBoxData = checkboxDataMap[label];
			if ((pressed || labelPressed) || ImGui::IsItemDeactivatedAfterEdit()) {
				checkBoxData.animationTime = ImGui::GetTime();
			}
			float alpha;
			if (gui::MenuOpen && globalalpha >= 1.f) {
				alpha = *v ? 1.0f : 0.0f; // Set initial alpha based on checkbox state
				if (checkBoxData.animationTime > 0.0f) {
					float t = (ImGui::GetTime() - checkBoxData.animationTime) * 4.0f; // Animation speed
					alpha = *v ? ImSaturate(t) : 1.0f - ImSaturate(t); // Linearly interpolate alpha based on time
					if (t >= 1.0f)
						checkBoxData.animationTime = 0.0f;
				}
			}
			else {
				alpha = globalalpha;
			}
			ImU32 color = (ImColor(checkColor.Value.x, checkColor.Value.y, checkColor.Value.z, alpha));

			window->DrawList->AddRectFilled(fillMin, fillMax, color, rounding, ImDrawFlags_RoundCornersAll);
			window->DrawList->AddRect(check_bb_min, check_bb_max, boxColor, rounding, ImDrawFlags_RoundCornersAll, thickness);

			// Determine text color based on hover state
			ImColor textColor = labelhovered ? ImColor(255, 255, 255, static_cast<int>(globalalpha * 255)) : ImColor(180, 180, 180, static_cast<int>(globalalpha * 255));
			if (rmbClicked != nullptr) {
				// Создаем новую строку с добавленным символом '^'
				std::string newLabel = std::string(label) + " ^";

				// Преобразуем новую строку в const char* для использования в ImGui
				const char* newLabelCStr = newLabel.c_str();

				// Рисуем текст с обновленной меткой
				window->DrawList->AddText(labelPos, textColor, newLabelCStr);
			}
			else {
				// Рисуем текст с исходной меткой
				window->DrawList->AddText(labelPos, textColor, label);
			}


			ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + checkboxSize + style.ItemSpacing.y)); // Возвращаем курсор в исходное положение
		}





	

		 



		//BUTTONPICKER
		struct BindState {
			int& key;                // Ссылка на переменную, например, gui::settings::misc::minijump
			bool waitingForKey;      // Флаг ожидания нажатия клавиши

			// Конструктор принимает ссылку на переменную клавиши
			BindState(int& keyRef) : key(keyRef), waitingForKey(false) {}
		};

		

		// Функция для получения имени клавиши с использованием WinAPI
		const char* GetKeyName(int key) {
			if (key == 0) return "None";  // Если клавиша не выбрана

			// Проверка для мышиных кнопок
			switch (key) {
			case VK_LBUTTON: return "Left Mouse Button";
			case VK_RBUTTON: return "Right Mouse Button";
			case VK_MBUTTON: return "Middle Mouse Button";
			case VK_XBUTTON1: return "Mouse Button 4";
			case VK_XBUTTON2: return "Mouse Button 5";
			default: break;
			}

			// Если это не кнопка мыши, используем стандартный способ для клавиатуры
			static char name[128];
			if (GetKeyNameTextA(MapVirtualKeyA(key, 0) << 16, name, 128) != 0) {
				return name;
			}
			return "Unknown";
		}

		void ButtonPicker(const char* description, BindState& bindState) {
			ImGui::Text(description);  // Display the description text
			ImGui::SameLine();  // Keep the text and button on the same line

			static bool wasWaiting = false;  // Статическая переменная для отслеживания состояния

			// Кнопка для привязки клавиш
			if (ImGui::Button(bindState.waitingForKey ? "Press any key..." : GetKeyName(bindState.key))) {
				if (!bindState.waitingForKey) {
					bindState.waitingForKey = true;  // Начать ожидание нажатия клавиши
					wasWaiting = true;  // Установить состояние ожидания
				}
			}

			// Если мы ожидаем нажатие клавиши
			if (bindState.waitingForKey) {
				if (wasWaiting) {  // Если ожидание было установлено
					for (int key = 0x01; key <= 0xFE; ++key) {  // Перебираем виртуальные коды клавиш
						if (GetAsyncKeyState(key) & 0x8000) {  // Проверяем, нажата ли клавиша
							bindState.key = key;  // Сохраняем нажатую клавишу
							bindState.waitingForKey = false;  // Останавливаем ожидание нажатия клавиши
							wasWaiting = false;  // Сброс состояния ожидания
							break;
						}
					}
				}

				// Для лучшего пользовательского опыта добавляем сообщение
				ImGui::Text("Press any key...");
			}
		}


		
		static bool buttonhovered1 = false;
		static bool buttonhovered2 = false;
		static bool buttonhovered3 = false;
		static bool buttonhovered4 = false;
		

		template<typename T>
		void RenderSliderWithColor(const char* label, T* value, T minValue, T maxValue, bool isFloat = false) {

			ImVec2 cursorPos = ImGui::GetCursorScreenPos();

			// Лейбл слева
			ImGui::Text(label);
			ImGui::SameLine();

			// Вычисление ширины для значения слайдера и смещение его максимально вправо
			float windowWidth = ImGui::GetWindowWidth();
			float valueWidth = ImGui::CalcTextSize("0000").x;  // Примерная ширина для отображения значения
			ImGui::SameLine(windowWidth - valueWidth - ImGui::GetStyle().ItemSpacing.x * 2);  // Смещаем значение максимально вправо

			// Форматирование и вывод значения слайдера
			char buffer[16];
			if (isFloat) {
				snprintf(buffer, sizeof(buffer), "%.2f", static_cast<float>(*value));
			}
			else {
				snprintf(buffer, sizeof(buffer), "%d", static_cast<int>(*value));
			}

			ImGui::Text(buffer);  // Отображаем значение

			// Уменьшение вертикального расстояния между текстом и слайдером
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 2.0f));  // Изменение отступов: 2.0f - уменьшение вертикального пробела

			// Настройка высоты слайдера
			float sliderHeight = ImGui::GetFrameHeight() * 0.4f; // Измените на 0.4f для уменьшения высоты
			ImGui::PushID(label);  // Используем label как ID

			// Расчёт прогресса
			float fraction = static_cast<float>(*value - minValue) / static_cast<float>(maxValue - minValue);
			ImVec2 sliderSize = ImVec2(ImGui::GetContentRegionAvail().x - 10.0f, sliderHeight);
			ImColor activeColor(gui::settings::misc::checkboxColor.x, gui::settings::misc::checkboxColor.y, gui::settings::misc::checkboxColor.z, globalalpha);
			ImColor inactiveColor(10, 10, 10, static_cast<int>(globalalpha * 255));
			ImColor backgroundColor(50, 50, 50, 100);  // Цвет фона для слайдера

			// Слайдер на новой строке с уменьшенным вертикальным отступом
			ImGui::InvisibleButton("##Slider", sliderSize);
			if (ImGui::IsItemClicked() || ImGui::IsItemActive()) {
				ImVec2 mousePos = ImGui::GetMousePos();
				fraction = (mousePos.x - ImGui::GetItemRectMin().x) / sliderSize.x;
				fraction = ImClamp(fraction, 0.0f, 1.0f);
				*value = static_cast<T>(fraction * (maxValue - minValue) + minValue);
			}

			// Рисование фона слайдера
			ImVec2 start = ImGui::GetItemRectMin();
			ImVec2 sliderBgEnd = ImVec2(start.x + sliderSize.x, start.y + sliderHeight);
			ImGui::GetWindowDrawList()->AddRectFilled(start, sliderBgEnd, backgroundColor, 4.0f);  // Добавляем фон

			// Рисование активной части слайдера
			ImVec2 activeEndVec = ImVec2(start.x + fraction * sliderSize.x, start.y + sliderHeight);
			ImGui::GetWindowDrawList()->AddRectFilled(start, activeEndVec, activeColor, 4.0f);  // Активная часть слайдера

			// Рисование кружка на активной части слайдера
			float circleRadius = 8.0f; // Радиус кружка
			ImVec2 circlePos = ImVec2(start.x + fraction * sliderSize.x, start.y + sliderHeight / 2.0f);
			ImGui::GetWindowDrawList()->AddCircleFilled(circlePos, circleRadius, activeColor); // Цвет кружка (белый)

			ImGui::PopID();
			ImGui::PopStyleVar();  // Возвращаем старый стиль
		}



		void AnimateSubwindow(bool isActive, float& animationHeight, float& childAlpha, ImVec4& bgColor, float targetHeight, float speed = 0.1f)
		{
			float minHeight = ImGui::GetFrameHeight(); // минимальная высота закрытого окна

			if (isActive) {
				animationHeight = std::lerp(animationHeight, targetHeight, speed);
				childAlpha = std::lerp(childAlpha, 0.5f, speed);
			}
			else {
				animationHeight = std::lerp(animationHeight, minHeight, speed);
				childAlpha = std::lerp(childAlpha, 0.0f, speed);
			}

			bgColor = ImVec4(0.0f, 0.0f, 0.0f, childAlpha);
		}


		

		// Функция для вычисления расстояния между двумя точками
		float Distance(const ImVec2& p1, const ImVec2& p2) {
			return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
		}

		void RenderBackground() {
			ImDrawList* draw_listback = gui::settings::dlist1;
			ImVec2 viewport_size = ImGui::GetMainViewport()->Size;

			// Рисуем фон
			draw_listback->AddRectFilled(ImVec2(0, 0), viewport_size, IM_COL32(2, 2, 2, 220));

			
			
		}
		void DrawWindowBorder() {
			// Получаем положение и размер окна
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();

					ImDrawList* draw_list = gui::settings::dlist1;

			// Основной цвет подсветки
			ImColor backlightColor(gui::settings::misc::checkboxColor.x, gui::settings::misc::checkboxColor.y, gui::settings::misc::checkboxColor.z, gui::settings::misc::checkboxColor.w);
			float radius = 12.0f; // Радиус размытия
			float alpha;
			// Количество шагов для градиента
			int steps = 10;
			for (int i = 0; i < steps; ++i) {
				
					alpha = (globalalpha / 2) * ((float)(steps - i * 2) / steps); // Уменьшение прозрачности с увеличением расстояния
				
				ImColor color = ImColor(backlightColor.Value.x, backlightColor.Value.y, backlightColor.Value.z, alpha);

				// Установка положения и размера каждого слоя подсветки
				float offset = radius * (float)i / steps;
				ImVec2 innerPos = ImVec2((windowPos.x - offset) , (windowPos.y - offset) );
				ImVec2 outerPos = ImVec2((windowPos.x + windowSize.x + offset) ,( windowPos.y + windowSize.y + offset) ) ;

				// Рисуем слой подсветки
				draw_list->AddRectFilled(innerPos, outerPos, color, radius);
			}
		}

		// Структура для хранения информации о дочернем окне
	

		// Функции для сложения и вычитания ImVec2
		ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
			return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
		}

		ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) {
			return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
		}

		
	

	
		static bool boxsettings = false;
		static bool removalssettings = false;
		static bool hitmarkersettings = false;
		static bool velocitysettings = false;
		static bool staminasettings = false;
		static bool indicatorssettings = false;
		static bool offscreensettings = false;
		static bool fogsettings = false;
		static bool antiaimsettings = false;
		static bool healthbarsettings = false;
		static bool fakelagsettings = false;
		static char configNameBuffer[256] = "";
		static char folderbuffer[256] = "";
		static std::string tabvisuals = "Visuals";;
		static std::string tabaimbot = "Aimbot";;
		static std::string tabmisc = "Misc";;
		static std::string tabskins = "Skins";;
		static bool showboxwindow = false;
		static bool showvelowindow = false;
		static bool showstaminawindow = false;
		static bool showoffscreenwindow = false;
		static bool showfogwindow = false;
		static bool showhealthwindow = false;
		ImColor checkboxColor = gui::settings::misc::velocolor;
		
		
		



		
		static ImVec2 childSize = ImVec2(150, 0); // Укажите начальный размер по умолчанию
		static float buttonWidth1;
		static float buttonWidth2;
		static float buttonWidth3;
		static float buttonWidth4;
	    static float startPosX = 5 ; 
	
		static float buttonrounding1;
		static float buttonrounding2;
		static float buttonrounding3;
		static float buttonrounding4;
		
		int my_image_width = 0;
		int my_image_height = 0;
		PDIRECT3DTEXTURE9 my_texture = NULL;
	
		static 	BindState lm(gui::settings::binds::leftmanual);
		static 	BindState bm(gui::settings::binds::backmanual);
		static 	BindState rm(gui::settings::binds::rightmanual);
		static BindState aimBind(gui::settings::binds::aimKey);
		 BindState fb(gui::settings::binds::forcebaim);
		static BindState jbBindState(gui::settings::binds::jumpbug);  // Ссылка на переменную привязки
		static BindState ljBindState(gui::settings::binds::longjump);
		static BindState mjBindState(gui::settings::binds::minijump);
		static BindState ebBindState(gui::settings::binds::edgebug);
		static BindState ap(gui::settings::binds::autopeekbutton);

			 int currentoffset;

		void DrawMouseMovementVisualization(float mousedx, float mousedn) {
			// Получаем указатель на список отрисовки фона
			ImDrawList* drawList = ImGui::GetBackgroundDrawList();

			// Определяем размеры и положение прямоугольников
			ImVec2 rectSize(50.0f, 200.0f); // Размеры прямоугольников
			ImVec2 startPos = ImGui::GetCursorScreenPos(); // Позиция начала отрисовки

			// Отрисовка фона для mousedx
			ImVec2 rect1Pos = startPos + ImVec2(10.0f, 0.0f); // Позиция первого прямоугольника
			drawList->AddRectFilled(rect1Pos, rect1Pos + rectSize, IM_COL32(50, 50, 50, 255), 10.0f); // Серый фон

			// Отрисовка заполнения для mousedx
			float absoluteMousedx = fabs(mousedx); // Убираем минус для mousedx
			float filledHeight1 = (absoluteMousedx / 100.0f) * rectSize.y; // Пример, максимальная высота 200.0
			float fillPosY1 = rect1Pos.y + rectSize.y - filledHeight1; // Позиция заполнения снизу вверх
			drawList->AddRectFilled(ImVec2(rect1Pos.x, fillPosY1), rect1Pos + ImVec2(rectSize.x, rect1Pos.y + rectSize.y), IM_COL32(255, 0, 0, 255), 10.0f); // Красный цвет, скругление 10

			// Отрисовка фона для mousedn
			ImVec2 rect2Pos = startPos + ImVec2(70.0f, 0.0f); // Позиция второго прямоугольника
			drawList->AddRectFilled(rect2Pos, rect2Pos + rectSize, IM_COL32(50, 50, 50, 255), 10.0f); // Серый фон

			// Отрисовка заполнения для mousedn
			float absoluteMousedn = fabs(mousedn); // Убираем минус для mousedn
			float filledHeight2 = (absoluteMousedn / 100.0f) * rectSize.y; // Пример, максимальная высота 200.0
			float fillPosY2 = rect2Pos.y + rectSize.y - filledHeight2; // Позиция заполнения снизу вверх
			drawList->AddRectFilled(ImVec2(rect2Pos.x, fillPosY2), rect2Pos + ImVec2(rectSize.x, rect2Pos.y + rectSize.y), IM_COL32(0, 0, 255, 255), 10.0f); // Синий цвет, скругление 10
		}
	

		static std::string fontfile = "";
		static int font = 0;
		void gui::Render() noexcept
		{
			ImGuiIO& io = ImGui::GetIO();
			if (reloadFont)
			{
				if (font == 1)
				{
					BuildFont(80, fontfile, gui::settings::fonts::velocityfont);
				}
				else if (font == 2)
				{
					BuildFont(80, fontfile, gui::settings::fonts::staminafont);
				}
		    	else if (font == 3)
				{
					BuildFont(80, fontfile, gui::settings::fonts::indicatorsfont);
				}
				else if (font == 4)
				{
					BuildFont(80, fontfile, gui::settings::fonts::spectatorlistfont);
				}
			else if (font == 5)
				{
					BuildFont(80, fontfile, gui::settings::fonts::nameespfont);
				}
			
				ImGui_ImplDX9_InvalidateDeviceObjects();
				ImGui_ImplDX9_CreateDeviceObjects();
				reloadFont = false;
			}
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			
			ImGui::NewFrame();
			gui::settings::dlist1 = ImGui::GetBackgroundDrawList();
			if (hooks::setup == true)
			{
				if (gui::settings::esp::enable)
				{
					ESP::HandleESP();
				}
				if (gui::settings::aimbot::antiaim)
				{
					Aimbot::ManualIndicator();
				}
				if (gui::settings::misc::rageindicators)
				{
					Misc::RageIndicators(hooks::bSendPacket);

				}

				if (gui::settings::misc::spectatorlist)
				{
					Misc::SpectatorList();

				}
				if (gui::settings::misc::velocity)
				{
					Misc::Velocity();
				}

				if (gui::settings::misc::indicators)
				{
					Misc::Indicators();


				}
			}
			if (gui::settings::misc::watermark)
			{

				Misc::ClanTag();

				Misc::Watermarkk();


			}
			
			std::vector<std::string> files = GetFilesInDirectory("C:/hw$"); // Укажите путь к папке, если требуется другая директория
			bool nomove = false;
			
			
			
			
			
			
		
			int screenheight, screenwidth;
			engine->GetScreenSize(screenwidth, screenheight);
			

		
		
		
			
		
			
			

			static float boxanimation = ImGui::GetFrameHeight();
			static float boxchlidalpha = 0.0f;
			ImVec4 boxbgcolor = ImVec4(0.0f, 0.0f, 0.0f, boxchlidalpha);

			static float removalsanimation = ImGui::GetFrameHeight();
			static float removalschildalpha = 0.0f;
			ImVec4 removalsbgcolor = ImVec4(0.0f, 0.0f, 0.0f, removalschildalpha);

			static float hitmarkeranimation = ImGui::GetFrameHeight();
			static float hitmarkerchildalpha = 0.0f;
			ImVec4 hitmarkerbgcolor = ImVec4(0.0f, 0.0f, 0.0f, hitmarkerchildalpha);

			static float indicatorschlidalpha = 0.0f;
			static float indicatorsanimation = ImGui::GetFrameHeight();
			ImVec4 indicatorsbgcolor = ImVec4(0.0f, 0.0f, 0.0f, indicatorschlidalpha);

			static float fakelagsanimation = ImGui::GetFrameHeight();
			static float fakelagschlidalpha = 0.0f;
			ImVec4 fakelagsbgcolor = ImVec4(0.0f, 0.0f, 0.0f, fakelagschlidalpha);

			static float foganimation = ImGui::GetFrameHeight();
			static float fogchlidalpha = 0.0f;
			ImVec4 fogbgcolor = ImVec4(0.0f, 0.0f, 0.0f, fogchlidalpha);

			static float oofanimation = ImGui::GetFrameHeight();
			static float oofchlidalpha = 0.0f;
			ImVec4 oofbgcolor = ImVec4(0.0f, 0.0f, 0.0f, oofchlidalpha);

			static float healthanimation = ImGui::GetFrameHeight();
			static float healthchlidalpha = 0.0f;
			ImVec4 healthbgcolor = ImVec4(0.0f, 0.0f, 0.0f, healthchlidalpha);

			static float staminaanimation = ImGui::GetFrameHeight();
			static float staminachildalpha = 0.0f;
			ImVec4 staminabgcolor = ImVec4(0.0f, 0.0f, 0.0f, staminachildalpha);

			static float veloanimation = ImGui::GetFrameHeight();
			static float velochlidalpha = 0.0f;
			ImVec4 velobgcolor = ImVec4(0.0f, 0.0f, 0.0f, velochlidalpha);

			static float autofireanimation = ImGui::GetFrameHeight();
			static float autofirechlidalpha = 0.0f;
			ImVec4 autofirebgcolor = ImVec4(0.0f, 0.0f, 0.0f, autofirechlidalpha);

			static float antiaimanimation = ImGui::GetFrameHeight();
			static float antiaimchlidalpha = 0.0f;
			ImVec4 antiaimbgcolor = ImVec4(0.0f, 0.0f, 0.0f, antiaimchlidalpha);

			static ImVec2 cursorPos;
			constexpr float AnimationSpeed = 1.0f; // Adjust as needed

			// Анимация открытия
			if (gui::MenuOpen && globalalpha < 1.f) {
				float fc = 255.f / 0.2f * ImGui::GetIO().DeltaTime * AnimationSpeed;
				globalalpha = std::clamp(globalalpha + fc / 255.f, 0.f, 1.f);
				ImGui::GetStyle().Alpha = globalalpha;
			}

			// Анимация закрытия
			if (!gui::MenuOpen && globalalpha > 0.f) {
				float fc = 255.f / 0.2f * ImGui::GetIO().DeltaTime * AnimationSpeed;
				globalalpha = std::clamp(globalalpha - fc / 255.f, 0.f, 1.f);
				ImGui::GetStyle().Alpha = globalalpha;
			
			}
			if (ImGui::GetStyle().Alpha > 0.f) {
				surface->UnlockCursor();
			
				ShowCursor(true);
				RenderBackground();
				
				ImGui::Begin("hotel white$", nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
				ImGui::SetNextWindowSize(ImVec2(800, 600));
				
				
				float border_thickness = 2.0f;
				DrawWindowBorder();
				// Получаем позицию верхнего левого угла окна и его размеры
				ImVec2 window_pos = ImGui::GetWindowPos();
				ImVec2 content_region_max = ImVec2(screenwidth, screenheight);

				// Рассчитываем координаты для рисования линии под заголовком
				ImVec2 p_min = ImGui::GetCursorScreenPos();
				ImGui::Dummy(ImVec2(ImGui::GetWindowWidth(), 5));
				// Render the gradient bar
				RenderAlphaGradientBar(ImVec2(p_min.x - 6, p_min.y));

				// Add some spacing to avoid overlapping
				bool buttonpressed = false;
						
				ImVec2 window_size = ImGui::GetWindowSize();
				float tab_height = 100.0f; // Высота вкладок
				ImVec2 child_size = ImVec2(window_size.x, window_size.y - tab_height);
					// Начало чайлда с указанным размером
		


	ImGui::BeginChild("TabContent", child_size, true);
				switch (gui::settings::activeTab) {
					{
				case TAB::VISUALS: //ESP
				{



					float childWidth = ImGui::GetContentRegionAvail().x / 2.0f;

					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.f, 0.f, 0.f, 0.5f));
					ImGui::BeginChild("Child1", ImVec2(childWidth, 0), true);
					ImGui::Text("Players");
					ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

					CustomCheckbox("Enable ESP", &gui::settings::esp::enable, nullptr, checkboxColor);
					float espdistance = static_cast<float>(gui::settings::esp::espdistance); // Convert to float
					RenderSliderWithColor("ESP Distance", &espdistance, 0.0f, 2222.0f);
					gui::settings::esp::espdistance = static_cast<int>(espdistance); // Convert back to int if necessary

					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
					ImGui::PushStyleColor(ImGuiCol_ChildBg, boxbgcolor);
					ImGui::BeginChild("boxwindow", ImVec2(0, boxanimation), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
					AnimateSubwindow(boxsettings, boxanimation, boxchlidalpha, boxbgcolor, 145.0f);
					CustomCheckbox("Boxes", &gui::settings::esp::boxes, &boxsettings, checkboxColor);
					ImGui::ColorEdit3("Box Color", (float*)&gui::settings::esp::boxcolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
					CustomCheckbox("Outer", &gui::settings::esp::outer, nullptr, checkboxColor);
					CustomCheckbox("Inner", &gui::settings::esp::inner, nullptr, checkboxColor);
					float length = static_cast<float>(gui::settings::esp::length);
					RenderSliderWithColor("Boxes Length ", &length, 0.0f, 10.0f);
					gui::settings::esp::length = static_cast<int>(length);
					ImGui::EndChild();
					ImGui::PopStyleColor();
					ImGui::PopStyleVar();

					CustomCheckbox("Name", &gui::settings::esp::name, nullptr, checkboxColor);
					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
					ImGui::PushStyleColor(ImGuiCol_ChildBg, healthbgcolor);
					ImGui::BeginChild("healthwindow", ImVec2(0, healthanimation), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
					AnimateSubwindow(healthbarsettings, healthanimation, healthchlidalpha, healthbgcolor, 65);
					CustomCheckbox("Health", &gui::settings::esp::health, &healthbarsettings, checkboxColor);

					CustomCheckbox("Custom Color Healthbar", &gui::settings::esp::healthbarcolorcustom, nullptr, checkboxColor);
					if (gui::settings::esp::healthbarcolorcustom)
					{
						ImGui::SameLine();

						ImGui::ColorEdit3("Healthbar Color", (float*)&gui::settings::esp::healthbarcolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					}

					ImGui::EndChild();
					ImGui::PopStyleVar();
					ImGui::PopStyleColor();
					CustomCheckbox("Weapon", &gui::settings::esp::weapon, nullptr, checkboxColor);
					CustomCheckbox("Draw C4", &gui::settings::esp::drawc4, nullptr, checkboxColor);
					CustomCheckbox("Player Flags", &gui::settings::esp::flags, nullptr, checkboxColor);
					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
					ImGui::PushStyleColor(ImGuiCol_ChildBg, oofbgcolor);
					ImGui::BeginChild("offscreenwindow", ImVec2(0, oofanimation), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
					CustomCheckbox("Enable Offscreen", &gui::settings::esp::offscreen, &offscreensettings, checkboxColor);

					float offscreenradius = static_cast<float>(gui::settings::esp::offscreenradius); // Convert to float
					RenderSliderWithColor("Radius", &offscreenradius, 0.0f, 100.0f);
					gui::settings::esp::offscreenradius = static_cast<int>(offscreenradius); // Convert back to int if necessary
					ImGui::ColorEdit3("Visible Color", (float*)&gui::settings::esp::outoffovicolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit3("Invisible Color", (float*)&gui::settings::esp::outoffovcolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
					if (offscreensettings && oofanimation != 120) {
						oofanimation = std::lerp(oofanimation, 120, 0.1f);
						oofchlidalpha = std::lerp(oofchlidalpha, 0.5f, 0.1f);

					}
					else if (!offscreensettings && oofanimation != ImGui::GetFrameHeight())
					{
						oofanimation = std::lerp(oofanimation, ImGui::GetFrameHeight(), 0.1f);
						oofchlidalpha = std::lerp(oofchlidalpha, 0.0f, 0.1f);
					}

					ImGui::EndChild();
					ImGui::PopStyleVar();
					ImGui::PopStyleColor();






					CustomCheckbox("Local Player Chams", &gui::settings::chams::self, nullptr, checkboxColor);
					if (gui::settings::chams::self) {
						ImGui::SameLine();
						ImGui::ColorEdit3("     ", (float*)&gui::settings::chams::selfcolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					}

					CustomCheckbox("Enable Chams", &gui::settings::chams::enable, nullptr, checkboxColor);
					if (gui::settings::chams::enable) {
						ImGui::SameLine();
						ImGui::ColorEdit3("    ", (float*)&gui::settings::chams::visiblecolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					}
					CustomCheckbox("Invisible Chams", &gui::settings::chams::invisible, nullptr, checkboxColor);
					if (gui::settings::chams::invisible) {
						ImGui::SameLine();
						ImGui::ColorEdit3("   ", (float*)&gui::settings::chams::hiddencolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					}




					ImGui::EndChild();
					ImGui::EndChild();
					ImGui::SameLine();

					ImGui::BeginChild("Child2", ImVec2(childWidth, 0), true);
					ImGui::Text("Misc");
					ImGui::BeginChild("ScrollRegion2", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
					ImGui::PushStyleColor(ImGuiCol_ChildBg, fogbgcolor);
					ImGui::BeginChild("fogwindow", ImVec2(0, foganimation), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
					AnimateSubwindow(fogsettings, foganimation, fogchlidalpha, fogbgcolor, 170.0f);
					CustomCheckbox("Fog Modulation", &gui::settings::esp::fogenable, &fogsettings, checkboxColor);

					float fogmin = static_cast<float>(gui::settings::esp::fogmin);
					RenderSliderWithColor("Fog Start", &fogmin, 0.0f, 2000.0f);
					gui::settings::esp::fogmin = static_cast<int>(fogmin);

					float fogmax = static_cast<float>(gui::settings::esp::fogmax);
					RenderSliderWithColor("Fog End", &fogmax, 0.0f, 2000.0f);
					gui::settings::esp::fogmax = static_cast<int>(fogmax);

					float fogdensity = static_cast<float>(gui::settings::esp::fogdensity);
					RenderSliderWithColor("Fog Density", &fogdensity, 0.0f, 100.0f);
					gui::settings::esp::fogdensity = static_cast<int>(fogdensity);
					ImGui::ColorEdit3("Visible Color", (float*)&gui::settings::esp::fogcolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);

					ImGui::PopStyleVar();
					ImGui::PopStyleColor();
					ImGui::EndChild();

					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
					ImGui::PushStyleColor(ImGuiCol_ChildBg, removalsbgcolor);
					ImGui::BeginChild("removals", ImVec2(0, removalsanimation), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
					AnimateSubwindow(removalssettings, removalsanimation, removalschildalpha, removalsbgcolor, 250.0f);
					CustomCheckbox("Removals", &gui::settings::esp::removals, &removalssettings, checkboxColor);
					CustomCheckbox("Disable Ragdolls Physics", &gui::settings::esp::disablephysics, nullptr, checkboxColor);
					CustomCheckbox("Disable Shadows", &gui::settings::esp::disableshadows, nullptr, checkboxColor);
					CustomCheckbox("Disable Postprocessing", &gui::settings::esp::disablepostprocessing, nullptr, checkboxColor);
					CustomCheckbox("Disable Blood", &gui::settings::esp::disableblood, nullptr, checkboxColor);
					CustomCheckbox("Disable 3DSky", &gui::settings::esp::disable3dsky, nullptr, checkboxColor);
					CustomCheckbox("Disable Scope Overlay", &gui::settings::esp::disablescope, nullptr, checkboxColor);
					CustomCheckbox("Disable Scope Zoom", &gui::settings::esp::disablezoom, nullptr, checkboxColor);
					if (ImGui::Button("Apply")) gui::settings::misc::needset = true;
					ImGui::EndChild();
					ImGui::PopStyleColor();
					ImGui::PopStyleVar();

					float sway = static_cast<float>(gui::settings::misc::sway);
					RenderSliderWithColor("Sway Scale", &sway, 0.0f, 3.0f, true);
					if (gui::settings::misc::sway != static_cast<float>(sway))
					{
						gui::settings::misc::sway = static_cast<float>(sway);
						gui::settings::misc::needswayset = true;

					}
					//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
					float nm = static_cast<float>(gui::settings::misc::nightmode);
					RenderSliderWithColor("NightMode", &nm, 0.0f, 3.0f, true);
					if (gui::settings::misc::nightmode != static_cast<float>(nm))
					{
						gui::settings::misc::nightmode = static_cast<float>(nm);
						gui::settings::misc::neednightmodeset = true;

					}
					ImGui::PopStyleColor();
					ImGui::EndChild();
					ImGui::EndChild();


					break;
				}


			case TAB::AIMBOT:
			{
				CustomCheckbox("Enable Aimbot", &gui::settings::aimbot::enable, nullptr, checkboxColor);

				if (gui::settings::aimbot::enable)
				{
					ImGui::Columns(2, nullptr, false);  // Разделяем на два столбца

					// Левый чилд - категории оружия и настройки
					ImGui::BeginChild("LeftChild", ImVec2(0, 0), true);

					ButtonPicker("Bind for Aim", aimBind);
					CustomCheckbox("Autofire", &gui::settings::aimbot::autofire, nullptr, checkboxColor);
					CustomCheckbox("Adaptive Autostop", &gui::settings::aimbot::adaptiveautostop, nullptr, checkboxColor);
					CustomCheckbox("Target Teammates", &gui::settings::aimbot::targetteammates, nullptr, checkboxColor);
					

					// --- Вкладки категорий оружия ---
					static float currentTabX = 0.0f;
					float buttonWidth = ImGui::GetWindowWidth() / 3;

					ImGui::BeginChild("WeaponTabs", ImVec2(0, 50.0f), false);
					if (ImGui::Button("Pistols", ImVec2(buttonWidth, 50)))
						gui::settings::aimbot::weaptype = 0;
					ImGui::SameLine();
					if (ImGui::Button("Rifles", ImVec2(buttonWidth, 50)))
						gui::settings::aimbot::weaptype = 1;
					ImGui::SameLine();
					if (ImGui::Button("Snipers", ImVec2(buttonWidth, 50)))
						gui::settings::aimbot::weaptype = 2;
					ImGui::EndChild();

					// Настройки для выбранного типа оружия
					if (gui::settings::aimbot::weaptype == 0) // Pistols
					{
						ShowMultiCombo("Hitboxes", gui::settings::aimbot::pistolhitboxes);
						RenderSliderWithColor("FOV", &gui::settings::aimbot::pistolfov, 0.0f, 180.0f, true);
						RenderSliderWithColor("Smooth", &gui::settings::aimbot::pistolsmooth, 0, 99, false);
						RenderSliderWithColor("RCS Amount", &gui::settings::aimbot::pistolrcs, 0, 100, true);
					}
					else if (gui::settings::aimbot::weaptype == 1) // Rifles
					{
						ShowMultiCombo("Hitboxes", gui::settings::aimbot::riflehitboxes);
						RenderSliderWithColor("FOV", &gui::settings::aimbot::riflefov, 0.0f, 180.0f, true);
						RenderSliderWithColor("Smooth", &gui::settings::aimbot::riflesmooth, 0, 99, false);
						RenderSliderWithColor("RCS Amount", &gui::settings::aimbot::riflercs, 0, 100, false);
					}
					else if (gui::settings::aimbot::weaptype == 2) // Snipers
					{
						ShowMultiCombo("Hitboxes", gui::settings::aimbot::sniperhitboxes);
						RenderSliderWithColor("FOV", &gui::settings::aimbot::sniperfov, 0.0f, 180.0f, true);
						RenderSliderWithColor("Smooth", &gui::settings::aimbot::snipersmooth, 0, 99, false);
						RenderSliderWithColor("RCS Amount", &gui::settings::aimbot::sniperrcs, 0, 100, false);
					}

					ImGui::EndChild();  // Закрываем левый чилд

					ImGui::NextColumn();  // Переход в правый столбец

					// Правый чилд - остальной интерфейс
					ImGui::BeginChild("RightChild", ImVec2(0, 0), true);
					CustomCheckbox("Force Body Aim", &gui::settings::aimbot::forcebaim, nullptr, checkboxColor);
					ImGui::SameLine();
					ButtonPicker("forcebaim", fb);
					CustomCheckbox("Antiaims", &gui::settings::aimbot::antiaim, &antiaimsettings, checkboxColor);
					CustomCheckbox("At Targets", &gui::settings::aimbot::attargets, nullptr, checkboxColor);
					CustomCheckbox("Jitter", &gui::settings::aimbot::jitter, nullptr, checkboxColor);
					ButtonPicker("Left Manual", lm);
					ButtonPicker("Off Manual", bm);
					ButtonPicker("Right Manual", rm);

					ImGui::EndChild();  // Закрываем правый чилд

					ImGui::Columns(1);  // Возвращаемся к одной колонке

				}
				break;
			}


				case TAB::MISC: //MISC
				{
				

					float childWidth = ImGui::GetContentRegionAvail().x / 2.0f;
					float childHeight = ImGui::GetContentRegionAvail().y / 2.0f;
					ImGui::BeginChild("Child4", ImVec2(childWidth, 0), true);
					ImGui::Text("Misc");
					ImGui::BeginChild("ScrollRegion4", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

					CustomCheckbox("Spectator List", &gui::settings::misc::spectatorlist, nullptr, checkboxColor);
					CustomCheckbox("Enable Radarhack", &gui::settings::misc::radar, nullptr, checkboxColor);
					CustomCheckbox("Bomb Info", &gui::settings::misc::bombinfo, nullptr, checkboxColor);



					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
					ImGui::PushStyleColor(ImGuiCol_ChildBg, velobgcolor);
					ImGui::BeginChild("velowindow", ImVec2(0, veloanimation), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse); // Устанавливаем флаги для отключения хедера и ресайза
					CustomCheckbox("Enable Velocity", &gui::settings::misc::velocity, &velocitysettings, checkboxColor);
					ImGui::ColorEdit3("First Color", (float*)&gui::settings::misc::velocolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
					ImGui::SameLine();
					ImGui::ColorEdit3("Second Color", (float*)&gui::settings::misc::velocolor2, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
					int maxvelocolor = static_cast<int>(gui::settings::misc::maxcolor);
					RenderSliderWithColor("Velocity Color Cap", &maxvelocolor, 0, 333, false);
					gui::settings::misc::maxcolor = static_cast<int>(maxvelocolor);

					int veloy = static_cast<int>(gui::settings::misc::velocityposition);
					RenderSliderWithColor("Velocity Position ", &veloy, 0, screenheight, false);
					gui::settings::misc::velocityposition = static_cast<int>(veloy);
					static const char* pretypes[] = { "None", "Default", "KamiStyle" };
					static int pretype = 0;
					ImGui::Combo("Velo Pre", &gui::settings::misc::pretype, pretypes, IM_ARRAYSIZE(pretypes));
					if (gui::settings::misc::pretype != 0) {
						ImGui::ColorEdit3("Pre Color", (float*)&gui::settings::misc::precolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
					}
					if (velocitysettings && veloanimation != 180) {
						veloanimation = std::lerp(veloanimation, 180, 0.1f);
						velochlidalpha = std::lerp(velochlidalpha, 0.5f, 0.1f);

					}
					else if (!velocitysettings && veloanimation != ImGui::GetFrameHeight())
					{
						veloanimation = std::lerp(veloanimation, ImGui::GetFrameHeight(), 0.1f);
						velochlidalpha = std::lerp(velochlidalpha, 0.0f, 0.1f);
					}
					ImGui::PopStyleColor();
					ImGui::PopStyleVar();
					ImGui::EndChild();



					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
					ImGui::PushStyleColor(ImGuiCol_ChildBg, staminabgcolor);
					ImGui::BeginChild("staminawindow", ImVec2(0, staminaanimation), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse); // Устанавливаем флаги для отключения хедера и ресайза
					AnimateSubwindow(staminasettings, staminaanimation, staminachildalpha, staminabgcolor, 200);
					CustomCheckbox("Enable Stamina", &gui::settings::misc::stamina, &staminasettings, checkboxColor);
					ImGui::ColorEdit3("First Stamina Color", (float*)&gui::settings::misc::staminacolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
					ImGui::SameLine();
					ImGui::ColorEdit3("Second Stamina Color", (float*)&gui::settings::misc::staminacolor2, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
					int maxstaminacolor = static_cast<int>(gui::settings::misc::staminamaxcolor);
					RenderSliderWithColor("Cap", &maxstaminacolor, 0, 40, false);
					gui::settings::misc::staminamaxcolor = static_cast<int>(maxstaminacolor);
					int staminay = static_cast<int>(gui::settings::misc::staminaposition);
					RenderSliderWithColor("Stamina Position ", &staminay, 0, screenheight, false);
					gui::settings::misc::staminaposition = static_cast<int>(staminay);

					ImGui::PopStyleColor();
					ImGui::PopStyleVar();
					ImGui::EndChild();
					//AAAAAAAAAAAAAAAAAAAAAAAAAAAa
					ImGui::BeginChild("indicatorswindow", ImVec2(0, indicatorsanimation), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse); // Устанавливаем флаги для отключения хедера и ресайза
					CustomCheckbox("Enable Indicators", &gui::settings::misc::indicators, &indicatorssettings, checkboxColor);
					ImGui::ColorEdit3("Default Color", (float*)&gui::settings::misc::firstindicatorscolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit3("Active Color", (float*)&gui::settings::misc::secondindicatorscolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
					int indiy = static_cast<int>(gui::settings::misc::indicatorposition);
					RenderSliderWithColor("Indicators Position ", &indiy, 0, screenheight, false);
					gui::settings::misc::indicatorposition = static_cast<int>(indiy);
					if (indicatorssettings && indicatorsanimation != 120) {
						indicatorsanimation = std::lerp(indicatorsanimation, 120, 0.1f);
						indicatorschlidalpha = std::lerp(indicatorschlidalpha, 0.5f, 0.1f);

					}
					else if (!indicatorssettings && indicatorsanimation != ImGui::GetFrameHeight())
					{
						indicatorsanimation = std::lerp(indicatorsanimation, ImGui::GetFrameHeight(), 0.1f);
						indicatorschlidalpha = std::lerp(indicatorschlidalpha, 0.0f, 0.1f);
					}
					ImGui::PopStyleColor();
					ImGui::PopStyleVar();
					ImGui::EndChild();
					CustomCheckbox("Enable Rage Indicators", &gui::settings::misc::rageindicators, nullptr, checkboxColor);
					CustomCheckbox("Enable FastStop", &gui::settings::misc::faststop, nullptr, checkboxColor);
					//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
					ImGui::PushStyleColor(ImGuiCol_ChildBg, hitmarkerbgcolor);
					ImGui::BeginChild("hitmarkerwindow", ImVec2(0, hitmarkeranimation), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
					CustomCheckbox("Hitmarker", &gui::settings::misc::hitmarker, &hitmarkersettings, checkboxColor);
					ImGui::ColorEdit3("Hitmarker Color", (float*)&gui::settings::misc::hitmarkercolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);

					float hitmarkersize = static_cast<float>(gui::settings::misc::hitmarkersize);
					RenderSliderWithColor("Hitmarker Size ", &hitmarkersize, 0.0f, 100.0f);
					gui::settings::misc::hitmarkersize = static_cast<int>(hitmarkersize);

					float hitmarkertime = static_cast<float>(gui::settings::misc::hitmarkertime);
					RenderSliderWithColor("Hitmarker Time ", &hitmarkertime, 0.0f, 10.0f, true);
					gui::settings::misc::hitmarkertime = static_cast<float>(hitmarkertime);

					float hitmarkerthickness = static_cast<float>(gui::settings::misc::hitmarkerthickness);
					RenderSliderWithColor("Hitmarker Thickness ", &hitmarkerthickness, 0.0f, 10.0f);
					gui::settings::misc::hitmarkerthickness = static_cast<int>(hitmarkerthickness);
					ImGui::EndChild();
					ImGui::PopStyleColor();
					ImGui::PopStyleVar();
					if (hitmarkersettings && hitmarkeranimation != 145) {
						hitmarkeranimation = std::lerp(hitmarkeranimation, 145, 0.1f);
						hitmarkerchildalpha = std::lerp(hitmarkerchildalpha, 0.5f, 0.1f);

					}
					else if (!hitmarkersettings && hitmarkeranimation != ImGui::GetFrameHeight())
					{
						hitmarkeranimation = std::lerp(hitmarkeranimation, ImGui::GetFrameHeight(), 0.1f);
						hitmarkerchildalpha = std::lerp(hitmarkerchildalpha, 0.0f, 0.1f);
					}
					CustomCheckbox("HitSound", &gui::settings::misc::hitsound, nullptr, checkboxColor);
					CustomCheckbox("Enable Autopeek", &gui::settings::misc::autopeek, &fakelagsettings, checkboxColor);
					if (gui::settings::misc::autopeek)
					{
						ImGui::SameLine();

						ButtonPicker("", ap);
					}

					//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
					ImGui::PushStyleColor(ImGuiCol_ChildBg, fakelagsbgcolor);
					ImGui::BeginChild("fakelags", ImVec2(0, fakelagsanimation), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse); // Устанавливаем флаги для отключения хедера и ресайза
					CustomCheckbox("Enable Fakelags", &gui::settings::misc::fakelags, &fakelagsettings, checkboxColor);
					if (gui::settings::misc::fakelags)
					{
						CustomCheckbox("Adaptive Fakelags", &gui::settings::aimbot::adaptive, nullptr, checkboxColor);
						CustomCheckbox("On Peek Fakelags", &gui::settings::aimbot::peek, nullptr, checkboxColor);
					}





					if (fakelagsettings && fakelagsanimation != 90) {
						fakelagsanimation = std::lerp(fakelagsanimation, 90, 0.1f);
						fakelagschlidalpha = std::lerp(fakelagschlidalpha, 0.5f, 0.1f);

					}
					else if (!fakelagsettings && fakelagsanimation != ImGui::GetFrameHeight())
					{
						fakelagsanimation = std::lerp(fakelagsanimation, ImGui::GetFrameHeight(), 0.1f);
						fakelagschlidalpha = std::lerp(fakelagschlidalpha, 0.0f, 0.1f);
					}
					ImGui::PopStyleColor();
					ImGui::PopStyleVar();
					ImGui::EndChild();

					//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA



					//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

					CustomCheckbox("Info Spam", &gui::settings::misc::infospam, nullptr, checkboxColor);
					static const char* targetitems[] = { "Enemy", "Teammates" };
					static const char* chatitems[] = { "Global", "Teammates" };
					static int target = 0;
					static int chattarget = 0;


					if (ImGui::BeginCombo("Target", targetitems[target])) {
						for (int i = 0; i < IM_ARRAYSIZE(targetitems); i++) {
							bool isSelected = (target == i);
							if (ImGui::Selectable(targetitems[i], isSelected)) {
								target = i;
								gui::settings::misc::target = i;
							}
							if (isSelected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					if (ImGui::BeginCombo("Chat", chatitems[chattarget])) {
						for (int i = 0; i < IM_ARRAYSIZE(chatitems); i++) {
							bool isSelected = (chattarget == i);
							if (ImGui::Selectable(chatitems[i], isSelected)) {
								chattarget = i;
								gui::settings::misc::chattarget = i;
							}
							if (isSelected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
					CustomCheckbox("Enable Watermark", &gui::settings::misc::watermark, nullptr, checkboxColor);
					//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
					ImGui::PushStyleColor(ImGuiCol_ChildBg, indicatorsbgcolor);
					static char text[128] = "";
					ImGui::InputText("  ", text, IM_ARRAYSIZE(text));
					if (ImGui::Button("Update Tag"))
					{
						Misc::SetClanTag(text);
					}
					ImGui::ColorEdit4("Accent Color", (float*)&gui::settings::misc::checkboxColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
					std::vector<std::string> configFiles = GetFilesInDirectory("C:/HW$");
					static int currentItem = -1;

					std::vector<const char*> configFilesCStr;
					for (const auto& configFile : configFiles) {
						configFilesCStr.push_back(configFile.c_str());
					}

					// Change ImGui style to modify hovered color
					ImVec4 oldHoveredColor = ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered];
					ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4f, 0.4f, 0.8f, 1.0f); // Change the color as desired

					ImGui::ListBox(" ", &currentItem, configFilesCStr.data(), static_cast<int>(configFilesCStr.size()));

					// Restore the old hovered color after ListBox
					ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = oldHoveredColor;

					// Rest of your code remains unchanged
					if (currentItem >= 0 && currentItem < configFiles.size()) {
						std::string fileName = configFiles[currentItem];
						size_t pos = fileName.find_last_of(".");
						if (pos != std::string::npos) {
							fileName = fileName.substr(0, pos);
						}

						strcpy_s(configNameBuffer, sizeof(configNameBuffer), fileName.c_str());
					}

					ImGui::InputText(" ", configNameBuffer, sizeof(configNameBuffer));

					ImGui::SameLine();

					// Implement functionality for the "Create" button
					if (ImGui::Button("Create")) {
						std::string configName(configNameBuffer);
						std::string fullFileName = configName + ".hw$";
						if (!std::filesystem::exists("C:/HW$/" + fullFileName)) {
							SaveConfig(fullFileName);
						}
						else {
							ImGui::OpenPopup("Error##CreateConfig");
						}
					}

					if (ImGui::BeginPopupModal("Error##CreateConfig", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
						ImGui::Text("A configuration file with this name already exists.");
						if (ImGui::Button("OK")) {
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}

					if (ImGui::Button("Save")) {
						std::string configName = configNameBuffer; // Retrieve the text from the input field
						std::string fullFileName = configName + ".hw$";
						if (!configName.empty()) {
							if (std::filesystem::exists("C:/hw$/" + fullFileName)) {
								SaveConfig(fullFileName);
							}
							else {
								ImGui::OpenPopup("Error##SaveConfig");
							}
						}

						if (ImGui::BeginPopupModal("Error##SaveConfig", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
							if (configName.empty()) {
								ImGui::Text("Please enter a valid configuration file name.");
							}
							else {
								ImGui::Text("A configuration file with this name does not exist.");
							}
							if (ImGui::Button("OK")) {
								ImGui::CloseCurrentPopup();
							}
							ImGui::EndPopup();
						}
					}
					ImGui::SameLine();

					if (ImGui::Button("Load")) {
						std::string configName(configNameBuffer);
						std::string fullFileName = configName + ".hw$";
						LoadConfig(fullFileName);
					}
					ImGui::SameLine();
					if (ImGui::Button("Delete")) {
						std::string configName(configNameBuffer);
						std::string fullFileName = configName + ".hw$";
						if (!configName.empty()) {
							std::filesystem::path filePath = "C:/hw$/" + fullFileName;
							if (std::filesystem::exists(filePath)) {
								std::filesystem::remove(filePath);
							}
						}
					}
					ImGui::EndChild();
					ImGui::EndChild();
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.f, 0.f, 0.f, 0.5f));
					ImGui::BeginChild("Child3", ImVec2(childWidth, childHeight), true);
					ImGui::Text("Movement");
					ImGui::BeginChild("ScrollRegion3", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
					CustomCheckbox("Auto Bunnyhop", &gui::settings::misc::bhop, nullptr, checkboxColor);
					CustomCheckbox("Jumpbug", &gui::settings::misc::jumpbug, nullptr, checkboxColor);
					if (gui::settings::misc::jumpbug) {
						
						ButtonPicker("Jumpbug key", jbBindState);
					}
					CustomCheckbox("Longjump", &gui::settings::misc::longjump, nullptr, checkboxColor);
					if (gui::settings::misc::longjump) {
						  
						ButtonPicker("Longjump key", ljBindState);
					}
					CustomCheckbox("Minijump", &gui::settings::misc::minijump, nullptr, checkboxColor);
					if (gui::settings::misc::minijump) {
						
						ButtonPicker("Minijump key", mjBindState);
					}
					CustomCheckbox("Edgebug", &gui::settings::misc::edgebug, nullptr, checkboxColor);
					if (gui::settings::misc::edgebug) {
						
						ButtonPicker("Edgebug key", ebBindState);
					}
					ImGui::EndChild();
					ImGui::EndChild();
					std::vector<std::string> fontFiles = GetFilesInDirectory("C:/hw$/fonts");
					static int currentItemLeft = 0;  // Индекс выбранного UI элемента в левом списке
					static int currentItemRight = -1; // Индекс выбранного шрифта в правом списке

					// Список UI элементов
					std::vector<const char*> uiElements = { "Velocity", "Stamina", "Indicators", "Spectator List", "Name ESP" };

					// Преобразование списка шрифтов в формат, необходимый для ListBox
					std::vector<const char*> fontFilesCStr;
					for (const auto& fontFile : fontFiles) {
						fontFilesCStr.push_back(fontFile.c_str());
					}

					// Начало чилда
					ImGui::SetCursorPosX(childWidth);
					ImGui::SetCursorPosY(childHeight);
					ImGui::BeginChild("Font Manager", ImVec2(childWidth, childHeight), true);

					// Листбокс для выбора UI элемента (левая колонка)
					
					// Рассчитываем ширину для каждого листбокса, чтобы они поместились рядом
					float availableWidth = ImGui::GetContentRegionAvail().x;  // Получаем доступную ширину
					float listBoxWidth = (availableWidth / 2.0f) - ImGui::GetStyle().ItemSpacing.x;  // Делим на две равные части, учитывая отступы

					// Листбокс для выбора UI элемента (левая колонка)
					ImGui::PushItemWidth(listBoxWidth);  // Устанавливаем ширину для первого листбокса
					ImGui::ListBox("##leftFontList", &currentItemLeft, uiElements.data(), static_cast<int>(uiElements.size()));
					ImGui::PopItemWidth();  // Возвращаем стандартную ширину

					ImGui::SameLine();  // Перемещаем следующий элемент в ту же строку

					// Листбокс для выбора нового шрифта (правая колонка)
					ImGui::PushItemWidth(listBoxWidth);  // Устанавливаем ширину для второго листбокса
					ImGui::ListBox("##rightFontList", &currentItemRight, fontFilesCStr.data(), static_cast<int>(fontFilesCStr.size()));
					ImGui::PopItemWidth();  // Возвращаем стандартную ширину



					// Комбобокс для выбора флагов шрифта
					static int currentfontflag = 0;
					static const char* fontflags[] = { "None", "Dropshadow", "Outline" };
					ImGui::Combo("Fontflags", &currentfontflag, fontflags, IM_ARRAYSIZE(fontflags));

					float cs = static_cast<float>(gui::settings::fonts::currentsize);
					RenderSliderWithColor("Font Size ", &cs, 0.0f, 40.0f);
					gui::settings::fonts::currentsize = static_cast<int>(cs);
					if (currentfontflag != 0) {
						float co = static_cast<float>(gui::settings::fonts::currentoffset);
						RenderSliderWithColor("Offset ", &co, 0.0f, 10.0f);
						gui::settings::fonts::currentoffset = static_cast<int>(co);
					}

					// Кнопка "Apply Font" во всю ширину
					if (ImGui::Button("Apply Font", ImVec2(ImGui::GetContentRegionAvail().x, 0)) && currentItemLeft >= 0 && currentItemRight >= 0) {
						switch (currentItemLeft) {
						case 0:
							fontfile = "C:/hw$/fonts/" + fontFiles[currentItemRight];
							font = 1;
							gui::settings::fonts::velocityfontflag = currentfontflag;
							gui::settings::fonts::velocityfontoffset = gui::settings::fonts::currentoffset;
							gui::settings::fonts::velocityfontsize = gui::settings::fonts::currentsize;
							break;
						case 1:
							fontfile = "C:/hw$/fonts/" + fontFiles[currentItemRight];
							font = 2;
							gui::settings::fonts::staminafontflag = currentfontflag;
							gui::settings::fonts::staminafontoffset = gui::settings::fonts::currentoffset;
							gui::settings::fonts::staminafontsize = gui::settings::fonts::currentsize;
							break;
						case 2:
							fontfile = "C:/hw$/fonts/" + fontFiles[currentItemRight];
							font = 3;
							gui::settings::fonts::indicatorsfontflag = currentfontflag;
							gui::settings::fonts::indicatorsfontoffset = gui::settings::fonts::currentoffset;
							gui::settings::fonts::indicatorsfontsize = gui::settings::fonts::currentsize;
							break;
						case 3:
							fontfile = "C:/hw$/fonts/" + fontFiles[currentItemRight];
							font = 4;
							gui::settings::fonts::spectatorlistfontflag = currentfontflag;
							gui::settings::fonts::spectatorlistfontoffset = gui::settings::fonts::currentoffset;
							gui::settings::fonts::spectatorlistfontsize = gui::settings::fonts::currentsize;
							break;
						case 4:
							fontfile = "C:/hw$/fonts/" + fontFiles[currentItemRight];
							font = 5;
							gui::settings::fonts::nameespfontflag = currentfontflag;
							gui::settings::fonts::nameeespfontoffset = gui::settings::fonts::currentoffset;
							gui::settings::fonts::nameespfontsize = gui::settings::fonts::currentsize;
							break;
						}
						reloadFont = true;
					}

					ImGui::EndChild();



				
					
					break;
				}
				
case TAB::SKINCHANGER:
{
	std::vector<std::string> weapons = GetDirectoriesInDirectory("C:/hw$/skins");
	static int selectedWeapon = 0;
	static std::string weaponName = "";
	static int selectedSkin = 0;
	ImVec2 GlobalSize = ImGui::GetContentRegionAvail();
	ImVec2 mousePos;
	mousePos = ImGui::GetMousePos();
	bool isCursorOnWeapons = false;
	bool isCursorOnSkins = false;
	static float weaponwidth = GlobalSize.x / 2; // Начальная ширина ListBox для оружия
	static float skinswidth = GlobalSize.x / 2 - 3; // Начальная ширина ListBox для скинов

	{
		RenderCustomListBox("##Weapons", &selectedWeapon, weapons, weaponwidth, GlobalSize.y - 30);

		ImVec2 weaponsPos = ImGui::GetItemRectMin();
		ImVec2 weaponsSize = ImGui::GetItemRectSize();
		mousePos = ImGui::GetMousePos();
		isCursorOnWeapons = (mousePos.x >= weaponsPos.x && mousePos.x <= weaponsPos.x + weaponsSize.x &&
			mousePos.y >= weaponsPos.y && mousePos.y <= weaponsPos.y + weaponsSize.y);

		if (selectedWeapon >= 0 && selectedWeapon < weapons.size()) {
			weaponName = GetDirectoryName(weapons[selectedWeapon]);
		}
	}

	ImGui::SameLine();

	{
		if (!weaponName.empty()) {
			std::vector<std::string> skins = GetDirectoriesInDirectory("C:/hw$/skins/" + weaponName);
			if (!skins.empty()) {
				// Проверить, есть ли выбранный скин для данного оружия
				if (gui::settings::misc::weaponSkinMap.find(weaponName) != gui::settings::misc::weaponSkinMap.end()) {
					selectedSkin = gui::settings::misc::weaponSkinMap[weaponName];
				}

				RenderCustomListBox("##Skins", &selectedSkin, skins, skinswidth, GlobalSize.y - 30);

				ImVec2 skinsPos = ImGui::GetItemRectMin();
				ImVec2 skinsSize = ImGui::GetItemRectSize();

				isCursorOnSkins = (mousePos.x >= skinsPos.x && mousePos.x <= skinsPos.x + skinsSize.x &&
					mousePos.y >= skinsPos.y && mousePos.y <= skinsPos.y + skinsSize.y);

				if (selectedSkin >= 0 && selectedSkin < skins.size()) {
					std::string skinName = GetDirectoryName(skins[selectedSkin]);
					if (selectedSkin != gui::settings::misc::weaponSkinMap[weaponName]) { // Проверка корректности
						gui::settings::misc::weaponSkinMap[weaponName] = selectedSkin; // Сохранить выбранный скин для текущего оружия

						std::string sourceDir = "C:/hw$/skins/" + weaponName + "/" + skinName + "/";
						std::string destDir = "C:/Program Files (x86)/Steam/steamapps/sourcemods/csso/custom/" + weaponName;
					
						CopyDirectory(sourceDir, destDir);
						
					}
				}
				if (ImGui::Button("Update", ImVec2(GlobalSize.x, 0)))
				{
					if (weaponName == "AWP") {
						engine->ClientCmd("mat_reloadmaterial awp");
					}
					else if (weaponName == "AK-47") {
						engine->ClientCmd("mat_reloadmaterial ak47");
					}
					else if (weaponName == "Bayonet") {
						engine->ClientCmd("mat_reloadmaterial knife_bayonet");
					}
					else if (weaponName == "Desert Eagle") {
						engine->ClientCmd("mat_reloadmaterial pist_deagle");
					}
					else if (weaponName == "Butterfly Knife") {
						engine->ClientCmd("mat_reloadmaterial knife_butterfly");
					}
					else if (weaponName == "Five-Seven") {
						engine->ClientCmd("mat_reloadmaterial fiveseven");
					}
					else if (weaponName == "Glock-18") {
						engine->ClientCmd("mat_reloadmaterial glock");
					}
					else if (weaponName == "Karambit") {
						engine->ClientCmd("mat_reloadmaterial karam");
					}
					else if (weaponName == "M4A1-S") {
						engine->ClientCmd("mat_reloadmaterial rif_m4a1_silencer");
					}
					else if (weaponName == "M4A4") {
						engine->ClientCmd("mat_reloadmaterial rif_m4a4");
					}
					else if (weaponName == "M9") {
						engine->ClientCmd("mat_reloadmaterial m9");
					}
					else if (weaponName == "SSG 08") {
						engine->ClientCmd("mat_reloadmaterial ssg08");
					}
					else if (weaponName == "USP") {
						engine->ClientCmd("mat_reloadmaterial pist_usp");
					}
				}
			}
			else {
				ImGui::Text("No skins available");
			}
		}
	}

	// Плавное изменение размера ListBox при наведении курсора
	const float animationSpeed = 10.0f; // Скорость анимации (можно настроить)
	if (isCursorOnWeapons) {
		weaponwidth = std::lerp(weaponwidth, GlobalSize.x / 1.5f, animationSpeed * ImGui::GetIO().DeltaTime);
		skinswidth = std::lerp(skinswidth, GlobalSize.x / 2.5f, animationSpeed * ImGui::GetIO().DeltaTime);
	}
	else if (isCursorOnSkins) {
		skinswidth = std::lerp(skinswidth, GlobalSize.x / 1.5f, animationSpeed * ImGui::GetIO().DeltaTime);
		weaponwidth = std::lerp(weaponwidth, GlobalSize.x / 2.5f, animationSpeed * ImGui::GetIO().DeltaTime);
	}
	else {
		// Если курсор не на ListBox, вернуть к начальному размеру
		weaponwidth = std::lerp(weaponwidth, GlobalSize.x / 2.0f, animationSpeed * ImGui::GetIO().DeltaTime);
		skinswidth = std::lerp(skinswidth, GlobalSize.x / 2.0f - 3, animationSpeed * ImGui::GetIO().DeltaTime);
	}

	break;
}

			

					
				
			
					}
					
				
					}
					ImGui::EndChild();
					
					// Переменные для анимации (лучше вынести в глобальный или статический контекст)
					static float currentTabX = 0.0f;
					float animationSpeed = 0.1f; // Скорость анимации

					ImGui::BeginChild("TabButtons", ImVec2(window_size.x, tab_height), false, ImGuiWindowFlags_NoScrollbar);

					float buttonWidth = window_size.x / 4; // Делаем кнопки равной ширине окна / количества вкладок
					float buttonHeight = 50.0f; // Высота кнопок

					// Настраиваем цвета
					ImVec4 inactiveColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Светло-серый для неактивной вкладки
					ImVec4 activeColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // Белый для активной вкладки
					ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);   // Прозрачный фон

					ImDrawList* draw_list = ImGui::GetWindowDrawList(); // Получаем текущий список отрисовки
					ImVec2 child_pos = ImGui::GetCursorScreenPos();    // Получаем позицию начала child-окна

					// Определяем targetTabX на основе активной вкладки
					float targetTabX = 0.0f;
					switch (gui::settings::activeTab) {
					case TAB::VISUALS: targetTabX = 0.0f; break;
					case TAB::AIMBOT: targetTabX = buttonWidth; break;
					case TAB::MISC: targetTabX = 2 * buttonWidth; break;
					case TAB::SKINCHANGER: targetTabX = 3 * buttonWidth; break;
					}

					// Вкладка 1
					auto setButtonStyle = [&](bool isActive) {
						ImGui::PushStyleColor(ImGuiCol_Button, transparent);
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, transparent); // Убираем эффект наведения
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, transparent);  // Убираем эффект активации
						ImGui::PushStyleColor(ImGuiCol_Text, isActive ? activeColor : inactiveColor);
						};

					setButtonStyle(gui::settings::activeTab == TAB::VISUALS);
					if (ImGui::Button(tabvisuals.c_str(), ImVec2(buttonWidth, buttonHeight))) {
						gui::settings::activeTab = TAB::VISUALS;
						playButtonSound();
						buttonpressed = true;
					}
					ImGui::PopStyleColor(4);

					ImGui::SameLine();

					setButtonStyle(gui::settings::activeTab == TAB::AIMBOT);
					if (ImGui::Button(tabaimbot.c_str(), ImVec2(buttonWidth, buttonHeight))) {
						gui::settings::activeTab = TAB::AIMBOT;
						playButtonSound();
						buttonpressed = true;
					}
					ImGui::PopStyleColor(4);

					ImGui::SameLine();

					setButtonStyle(gui::settings::activeTab == TAB::MISC);
					if (ImGui::Button(tabmisc.c_str(), ImVec2(buttonWidth, buttonHeight))) {
						gui::settings::activeTab = TAB::MISC;
						playButtonSound();
						buttonpressed = true;
					}
					ImGui::PopStyleColor(4);

					ImGui::SameLine();

					setButtonStyle(gui::settings::activeTab == TAB::SKINCHANGER);
					if (ImGui::Button(tabskins.c_str(), ImVec2(buttonWidth, buttonHeight))) {
						gui::settings::activeTab = TAB::SKINCHANGER;
						playButtonSound();
						buttonpressed = true;
					}
					ImGui::PopStyleColor(4);

					// Обновляем позицию линии с помощью Lerp для плавной анимации
					currentTabX = ImLerp(currentTabX, targetTabX, animationSpeed);

					// Рисуем линию под активной вкладкой
					ImVec2 line_start = ImVec2(child_pos.x + currentTabX, child_pos.y + buttonHeight + 2.0f);
					ImVec2 line_end = ImVec2(line_start.x + buttonWidth, line_start.y);
					draw_list->AddLine(line_start, line_end, ImColor(1.0f, 1.0f, 1.0f, 1.0f), 3.0f); // Белая линия толщиной 3.0f

					ImGui::EndChild();


					

					ImGui::End();
			}
			
		
			ImGui::EndFrame();
		
			ImGui::Render();
		
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		}
	


		LRESULT CALLBACK WindowProcess(
			HWND hWnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam
		)
		{
			//toggle menu
			if (GetAsyncKeyState(VK_INSERT) & 1)
				gui::MenuOpen = !gui::MenuOpen;

			if (gui::MenuOpen && ImGui_ImplWin32_WndProcHandler(
				hWnd, msg, wParam, lParam))
				return 1L;


			return CallWindowProc(
				gui::originalWindowProcess,
				hWnd,
				msg,
				wParam,
				lParam
			);
		}