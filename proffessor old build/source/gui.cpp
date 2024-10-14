#include "../source/gui.h"
#include "../ext/imgui/imgui_impl_win32.h"
#include "../ext/imgui/imgui_impl_dx9.h"
#include "../ext/imgui/imgui.h"
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <iostream>
#include <string>
#include <cctype>
#include <fstream>
#include "../source/interfaces/interfaces.h"
#include <io.h>
#include <Psapi.h>
#include "../ext/imgui/imgui_internal.h"
#include "../ext/discord/discord_rpc.h"
#ifndef ImDrawCornerFlags_All
#define ImDrawCornerFlags_All (ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotLeft | ImDrawCornerFlags_BotRight)
#endif
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

void gui::Setup()
{
	if (!SetupWindowClass("hackClass001"))
		throw std::runtime_error("Failed to create window class.");

	if (!SetupWindow("cs:so"))
		throw std::runtime_error("Failed to create window.");

	if (!SetupDirectX())
		throw std::runtime_error("Failed to create device.");
	
	DestroyWindow();
	DestroyWindowClass();
}

void InitFonts() {
	ImFontAtlas* fonts = ImGui::GetIO().Fonts;

	static const ImWchar ranges[] = { 0x0020, 0xFFFF, 0 };
	ImFontConfig config;
	config.MergeMode = false;
	config.PixelSnapH = true;
	fonts->AddFontFromFileTTF("C:/HW$/Tahoma.ttf", 15.0f, &config, ranges);

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
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

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
	ESP = 0,
	AIMBOT,
	MISC,
	CHAMS,
	CONFIG
};

namespace fs = std::filesystem;

// Function to get files in a directory
std::vector<std::string> GetFilesInDirectory(const std::string& directory) {
	std::vector<std::string> files;
	for (const auto& entry : fs::directory_iterator(directory)) {
		if (entry.is_regular_file()) {
			files.push_back(entry.path().filename().string());
		}
	}
	return files;
}

void gui::settings::configs::SaveConfig(const std::string& fileName) {
	fs::path documentsFolder = "C:/HW$";
	fs::create_directories(documentsFolder);
	fs::path configPath = documentsFolder / fileName;

	std::ofstream file(configPath);
	if (file.is_open()) {
		file << std::boolalpha;
		// Convert ImVec4 color to string
		std::string checkboxColorStr = std::to_string(gui::settings::misc::checkboxColor.x) + ","
			+ std::to_string(gui::settings::misc::checkboxColor.y) + ","
			+ std::to_string(gui::settings::misc::checkboxColor.z) + ","
			+ std::to_string(gui::settings::misc::checkboxColor.w);
		/*std::string visColorStr = std::to_string(gui::settings::chams::visiblecolor.x) + ","
			+ std::to_string(gui::settings::chams::visiblecolor.y) + ","
			+ std::to_string(gui::settings::chams::visiblecolor.z) + ","
			+ std::to_string(gui::settings::chams::visiblecolor.w);
		std::string InvisColorStr = std::to_string(gui::settings::chams::hiddencolor.x) + ","
			+ std::to_string(gui::settings::chams::hiddencolor.y) + ","
			+ std::to_string(gui::settings::chams::hiddencolor.z) + ","
			+ std::to_string(gui::settings::chams::hiddencolor.w);*/
		// Save the configuration to file
		file << "MenuColor=" << checkboxColorStr << '\n';

		file << "Bunnyhop=" << gui::settings::misc::bhop << '\n'
			<< "Radar=" << gui::settings::misc::radar << '\n'
			<< "Velocity=" << gui::settings::misc::velocity << '\n'
			<< "Stamina=" << gui::settings::misc::stamina << '\n'
			<< "Velocityposition=" << gui::settings::misc::velocityposition << '\n'
			<< "Staminaposition=" << gui::settings::misc::staminaposition << '\n'
			<< "ShowLastVelocity=" << gui::settings::misc::showLastVelocity << '\n'
			<< "Indicators=" << gui::settings::misc::indicators << '\n'
			<< "Indicatorspos=" << gui::settings::misc::indicatorpos<< '\n'
			<< "JumpBug=" << gui::settings::misc::jumpbug << '\n'
			<< "EdgeBug=" << gui::settings::misc::edgebug << '\n'
			<< "Edgejump=" << gui::settings::misc::edgejump << '\n'
			/*<< "ChamsVis=" << visColorStr << '\n'
			<< "ChamsInVis=" << InvisColorStr << '\n'*/
			<< "Espenable=" << gui::settings::esp::enable << '\n'
			<< "Name=" << gui::settings::esp::name << '\n'
			<< "Health=" << gui::settings::esp::health << '\n'
			<< "Weapon=" << gui::settings::esp::weapon << '\n'
			<< "Boxes=" << gui::settings::esp::boxes << '\n'
			<< "DrawC4=" << gui::settings::esp::drawc4 << '\n'
			<< "BombInfo=" << gui::settings::misc::bomb << '\n'
			<< "Flags=" << gui::settings::esp::flags << '\n'
			<< "Aimbotenable=" << gui::settings::aimbot::enable << '\n'
			<< "Rcs=" << gui::settings::aimbot::rcs << '\n'
			<< "Silent=" << gui::settings::aimbot::silent << '\n'
			<< "AutoStop=" << gui::settings::aimbot::autostop << '\n'
			<< "Fov=" << gui::settings::aimbot::fov << '\n'
			<< "Rcsammount=" << gui::settings::aimbot::rcsAmmount << '\n'
			<< "Smooth=" << gui::settings::aimbot::smooth << '\n'
			<< "Watermark=" << gui::settings::misc::watermark << '\n'
			<< "Spectatorlist=" << gui::settings::misc::spectatorlist << '\n'
			<< "SpecX=" << gui::settings::misc::spectatorListPosX << '\n'
			<< "SpecY=" << gui::settings::misc::spectatorListPosY << '\n'
			<< "MusicDisplay=" << gui::settings::misc::show_browser_page << '\n'
			<< "MenuColor=" << checkboxColorStr << '\n';
			std::cout << "Config saved to " << configPath << std::endl;
	}
	else {
		std::cerr << "Unable to open file " << configPath << " for writing." << std::endl;
	}
}

	void gui::settings::configs::LoadConfig(const std::string& fileName) {
		fs::path documentsFolder = "C:/HW$";
		fs::path configPath = documentsFolder / fileName;

		std::ifstream file(configPath);
		if (file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {
				size_t pos = line.find('=');
				if (pos != std::string::npos) {
					std::string key = line.substr(0, pos);
					std::string value = line.substr(pos + 1);
					std::string checkboxColorStr = std::to_string(gui::settings::misc::checkboxColor.x) + ","
						+ std::to_string(gui::settings::misc::checkboxColor.y) + ","
						+ std::to_string(gui::settings::misc::checkboxColor.z) + ","
						+ std::to_string(gui::settings::misc::checkboxColor.w);
					/*std::string visColorStr = std::to_string(gui::settings::chams::visiblecolor.x) + ","
						+ std::to_string(gui::settings::chams::visiblecolor.y) + ","
						+ std::to_string(gui::settings::chams::visiblecolor.z) + ","
						+ std::to_string(gui::settings::chams::visiblecolor.w);
					std::string InvisColorStr = std::to_string(gui::settings::chams::hiddencolor.x) + ","
						+ std::to_string(gui::settings::chams::hiddencolor.y) + ","
						+ std::to_string(gui::settings::chams::hiddencolor.z) + ","
						+ std::to_string(gui::settings::chams::hiddencolor.w);*/
					if (key == "Bunnyhop") gui::settings::misc::bhop = (value == "true");
					else if (key == "Radar") gui::settings::misc::radar = (value == "true");
					else if (key == "Velocity") gui::settings::misc::velocity = (value == "true");
					else if (key == "Stamina") gui::settings::misc::stamina = (value == "true");
					else if (key == "Velocityposition") gui::settings::misc::velocityposition = std::stoi(value);
					else if (key == "Staminaposition") gui::settings::misc::staminaposition = std::stoi(value);
					else if (key == "ShowLastVelocity") gui::settings::misc::showLastVelocity = (value == "true");
					else if (key == "Indicators") gui::settings::misc::indicators = (value == "true");
					else if (key == "Indicatorspos") gui::settings::misc::indicatorpos = std::stoi(value);
					else if (key == "JumpBug") gui::settings::misc::jumpbug = (value == "true");
					else if (key == "EdgeBug") gui::settings::misc::edgebug = (value == "true");
					else if (key == "Edgejump") gui::settings::misc::edgejump = (value == "true");
					/*else if (key == "ChamsVis") gui::settings::chams::visible = (value == "true");
					else if (key == "ChamsInVis") gui::settings::chams::invisible = (value == "true");*/
					else if (key == "Espenable") gui::settings::esp::enable = (value == "true");
					else if (key == "Name") gui::settings::esp::name = (value == "true");
					else if (key == "Health") gui::settings::esp::health = (value == "true");
					else if (key == "Weapon") gui::settings::esp::weapon = (value == "true");
					else if (key == "Boxes") gui::settings::esp::boxes = (value == "true");
					else if (key == "DrawC4") gui::settings::esp::drawc4 = (value == "true");
					else if (key == "BombInfo") gui::settings::misc::bomb = (value == "true");
					else if (key == "Flags") gui::settings::esp::flags = (value == "true");
					else if (key == "Aimbotenable") gui::settings::aimbot::enable = (value == "true");
					else if (key == "Rcs") gui::settings::aimbot::rcs = (value == "true");
					else if (key == "Smooth") gui::settings::aimbot::smooth = std::stof(value);
					else if (key == "Silent") gui::settings::aimbot::silent = (value == "true");
					else if (key == "AutoStop") gui::settings::aimbot::autostop = (value == "true");
					else if (key == "Fov") gui::settings::aimbot::fov = std::stof(value);
					else if (key == "Rcsammount") gui::settings::aimbot::rcsAmmount = std::stoi(value);
					else if (key == "SpecY") gui::settings::misc::spectatorListPosY = std::stoi(value);
					else if (key == "SpecX") gui::settings::misc::spectatorListPosX = std::stoi(value);
					else if (key == "Spectatorlist") gui::settings::misc::spectatorlist = (value == "true");
					else if (key == "Watermark") gui::settings::misc::watermark = (value == "true");
					else if (key == "MusicDisplay") gui::settings::misc::show_browser_page = (value == "true");
					else if (key == "MenuColor") {
						// Split the color string into components
						std::istringstream iss(value);
						std::string component;
						std::vector<float> components;
						while (std::getline(iss, component, ',')) {
							components.push_back(std::stof(component));
						}

						// Construct ImVec4 from components
						if (components.size() == 4) {
							ImVec4 loadedColor = ImVec4(components[0], components[1], components[2], components[3]);
							gui::settings::misc::checkboxColor = loadedColor;
							/*gui::settings::chams::visiblecolor = loadedColor;
							gui::settings::chams::hiddencolor = loadedColor;*/
						}
						else if (key == "ChamsVis") {
							// Split the color string into components
							std::istringstream iss(value);
							std::string component;
							std::vector<float> components;
							while (std::getline(iss, component, ',')) {
								components.push_back(std::stof(component));
							}

							// Construct ImVec4 from components
							if (components.size() == 4) {
								ImVec4 loadedColor1 = ImVec4(components[0], components[1], components[2], components[3]);
								gui::settings::chams::visiblecolor = loadedColor1;
							}
						}
						else if (key == "ChamsInVis") {
							// Split the color string into components
							std::istringstream iss(value);
							std::string component;
							std::vector<float> components;
							while (std::getline(iss, component, ',')) {
								components.push_back(std::stof(component));
							}

							// Construct ImVec4 from components
							if (components.size() == 4) {
								ImVec4 loadedColor2 = ImVec4(components[0], components[1], components[2], components[3]);
								gui::settings::chams::hiddencolor = loadedColor2;
							}
						}
					}
				}
			}
		}
	}

void CenterText(const char* format, const float y_padding = 0.0f, ImColor color = ImColor(255, 255, 255)) {
	const ImVec2 text_size = ImGui::CalcTextSize(format);
	ImVec2 window_pos = ImGui::GetWindowPos();
	ImVec2 window_size = ImGui::GetWindowSize();

	// Set bigger font and bold style
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Using font index 1 for bold font
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.5f, 1.5f));
	ImGui::PushStyleColor(ImGuiCol_Text, color.Value); // Use color.Value to pass the actual color value
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5)); // Adjust spacing between tabs
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 10)); // Adjust padding within tabs

	ImGui::SameLine((window_size.x / 2) - (text_size.x / 2));
	if (y_padding > 0.0f)
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y_padding);
	ImGui::Text(format);

	ImGui::PopStyleVar(3); // Pop style changes
	ImGui::PopFont(); // Pop font change

	// Render gradient line below the centered text
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 line_start(window_pos.x, window_pos.y + text_size.y + y_padding + 5); // Adjust Y position
	ImVec2 line_end(window_pos.x + window_size.x, window_pos.y + text_size.y + y_padding + 5); // Adjust Y position

	// Draw gradient line
	for (int i = 0; i <= window_size.x; ++i) {
		if (i < window_size.x / 2) {
			ImVec4 line_color = ImVec4(255, 255, 255, 255 - (i * 2)); // Alpha gradient from transparent to opaque
			draw_list->AddLine(ImVec2(line_start.x + i, line_start.y), ImVec2(line_start.x + i, line_end.y), ImGui::ColorConvertFloat4ToU32(line_color), 2.0f); // Line thickness 2.0f
		}
		else {
			ImVec4 line_color = ImVec4(255, 255, 255, (i - window_size.x / 2) * 2); // Alpha gradient from opaque to transparent
			draw_list->AddLine(ImVec2(line_start.x + i, line_start.y), ImVec2(line_start.x + i, line_end.y), ImGui::ColorConvertFloat4ToU32(line_color), 2.0f); // Line thickness 2.0f
		}
	}
}







#ifndef ImDrawCornerFlags_TopLeft
#define ImDrawCornerFlags_TopLeft      1 << 0
#define ImDrawCornerFlags_TopRight     1 << 1
#define ImDrawCornerFlags_BotLeft      1 << 2
#define ImDrawCornerFlags_BotRight     1 << 3
#endif

struct CheckboxData {
	bool* v;
	float animationTime;
};
#include "unordered_map"
#include "features/misc.h"
////////////////////////////////////////////
std::unordered_map<const char*, CheckboxData> checkboxDataMap;
void CustomCheckbox(const char* label, bool* v, ImColor& checkboxColor) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImVec2 labelSize = ImGui::CalcTextSize(label);
	float checkboxSize = ImGui::GetFrameHeight();
	ImGui::Dummy(ImVec2(checkboxSize, checkboxSize)); // Reserve space for the checkbox

	ImVec2 check_bb_min(cursorPos.x, cursorPos.y);
	ImVec2 check_bb_max(cursorPos.x + checkboxSize, cursorPos.y + checkboxSize);
	ImRect check_bb(check_bb_min, check_bb_max);
	 
	ImVec2 pressrectmin(cursorPos.x, cursorPos.y);
	ImVec2 pressrectmax(cursorPos.x + checkboxSize + labelSize.x, cursorPos.y + checkboxSize);
	ImRect pressrect(pressrectmin, pressrectmax);
	ImGui::ItemAdd(check_bb, id);

	bool hovered, held;

	bool labelhovered, labelheld;
	bool pressed = ImGui::ButtonBehavior(pressrect, id, &hovered, &held);

	ImVec2 labelPos = ImVec2(cursorPos.x + checkboxSize + style.ItemInnerSpacing.x, cursorPos.y + (checkboxSize - labelSize.y) * 0.5f);
	bool labelPressed = ImGui::ButtonBehavior(ImRect(labelPos, ImVec2(labelPos.x + labelSize.x, labelPos.y + labelSize.y)), id, &labelhovered, &labelheld);


	if ((pressed || labelPressed) && v != nullptr)
		*v = !*v;

	// Update color based on state
	ImColor checkColor = *v ? ImColor(gui::settings::misc::checkboxColor.x, gui::settings::misc::checkboxColor.y, gui::settings::misc::checkboxColor.z, gui::settings::misc::checkboxColor.w) : ImColor(50, 50, 50);

	ImColor boxColor = *v ? ImColor(50, 50, 50, 255) : ImColor(30, 30, 30);

	if (pressed || labelPressed)
		checkColor = checkboxColor;
	boxColor = ImColor(30, 30, 30);

	float rounding = 4.0f; // Adjust rounding factor as needed
	float thickness = -1.0f; // Fill the rectangle completely

	// Draw rounded rectangle background
	window->DrawList->AddRectFilled(check_bb_min, check_bb_max, ImColor(25, 25, 25), rounding + 4, ImDrawCornerFlags_All);

	ImVec2 fillMin = ImVec2(check_bb_min.x + 1, check_bb_min.y + 1); // Offset by 1 pixel
	ImVec2 fillMax = ImVec2(check_bb_max.x - 1, check_bb_max.y - 1); // Offset by 1 pixel


	// Get or initialize CheckboxData for this label
	CheckboxData& checkBoxData = checkboxDataMap[label];
	if ((pressed || labelPressed) || ImGui::IsItemDeactivatedAfterEdit()) {
		checkBoxData.animationTime = ImGui::GetTime();
	}

	float alpha = *v ? 1.0f : 0.0f; // Set initial alpha based on checkbox state
	if (checkBoxData.animationTime > 0.0f) {
		float t = (ImGui::GetTime() - checkBoxData.animationTime) * 4.0f; // Animation speed
		alpha = *v ? ImSaturate(t) : 1.0f - ImSaturate(t); // Linearly interpolate alpha based on time
		if (t >= 1.0f)
			checkBoxData.animationTime = 0.0f;
	}

	ImU32 color = (ImColor(checkColor.Value.x, checkColor.Value.y, checkColor.Value.z, alpha));

	window->DrawList->AddRectFilled(fillMin, fillMax, color, rounding, ImDrawCornerFlags_All);


	window->DrawList->AddRect(check_bb_min, check_bb_max, boxColor, rounding, ImDrawCornerFlags_All, thickness);


	window->DrawList->AddText(labelPos, ImGui::GetColorU32(ImGuiCol_Text), label);
}



/// ///////////////////////////////////////////////
void RenderSliderWithColor(const char* label, float* value, float minValue, float maxValue) {
	ImGui::Text(label);
	ImGui::SameLine();
	// Adjust height
	float sliderHeight = ImGui::GetFrameHeight() * 0.6;
	float yOffset = 2.0f; // Offset value for downward shift

	ImGui::PushID(label);
	float fraction = (*value - minValue) / (maxValue - minValue);
	ImVec2 sliderSize = ImVec2(ImGui::GetContentRegionAvail().x - 100.0f, sliderHeight); // Decrease width for value display
	ImColor activeColor = ImColor(gui::settings::misc::checkboxColor.x, gui::settings::misc::checkboxColor.y, gui::settings::misc::checkboxColor.z, gui::settings::misc::checkboxColor.w);
	ImColor inactiveColor(15, 15, 15, 255);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 0.0f)); // Adjust spacing between slider and value
	ImGui::InvisibleButton("##Slider", sliderSize);
	if (ImGui::IsItemClicked() || ImGui::IsItemActive()) { // Update when clicked or active
		ImVec2 mousePos = ImGui::GetMousePos();
		fraction = (mousePos.x - ImGui::GetItemRectMin().x) / sliderSize.x;
		fraction = ImClamp(fraction, 0.0f, 1.0f);
		*value = fraction * (maxValue - minValue) + minValue;
	}

	ImVec2 start = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + yOffset);
	ImVec2 end = ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y + yOffset);

	ImVec2 activeEnd(start.x + fraction * sliderSize.x, end.y);

	// Add rounded rectangle for inactive part
	ImGui::GetWindowDrawList()->AddRectFilled(start, end, inactiveColor, 4.0f);

	// Add rounded rectangle for active part
	ImGui::GetWindowDrawList()->AddRectFilled(start, activeEnd, activeColor, 4.0f);
	ImGui::SetItemAllowOverlap();

	ImGui::PopStyleVar(); // Pop item spacing

	// Show value to the right of the slider as an integer
	ImGui::SameLine();
	ImGui::Text("%d", static_cast<int>(*value));

	ImGui::PopID();
}



void gui::Render() noexcept 
{

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	static char configNameBuffer[256] = "";
	std::cout << "Rendering GUI..." << std::endl; // Debug message
	Misc::Watermark();
	Misc::Musicdisplay();
	Misc::BombInfo();
	Misc::SpectatorList();
	// Set ImGui window style
	// Set ImGui window style
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

	style.Colors[ImGuiCol_FrameBg] = ImColor(0.2f, 0.2f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(0.3f, 0.3f, 0.3f, 0.9f);
	style.Colors[ImGuiCol_FrameBgActive] = ImColor(0.4f, 0.4f, 0.4f, 0.8f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.3f, 0.8f, 0.3f, 1.0f); // Green color when checked
	style.Colors[ImGuiCol_Button] = ImColor(0.4f, 0.4f, 0.4f, 0.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(0.30f, 0.30f, 0.30f, 0.1f);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(0.50f, 0.50f, 0.50f, 0.2f);

	style.Colors[ImGuiCol_ResizeGrip] = ImColor(0, 0, 0, 0);
	style.Colors[ImGuiCol_ResizeGripActive] = ImColor(0, 0, 0, 0);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImColor(0, 0, 0, 0);



	ImColor().Value; // Default white color
	ImColor checkboxColor = gui::settings::misc::checkboxColor;
	if (gui::MenuOpen) {
		// Begin rendering the menu windowB
		if (ImGui::Begin("Hotel_White", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse & MenuOpen)) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
			CenterText("Hotel_White", -30.0f, ImColor(255, 255, 255));
			// Render gradient line underneath the text
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5)); // Adjust spacing between tabs
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 10)); // Adjust padding within tabs

			// Begin child window
			ImGui::BeginChild("TabButtons", ImVec2(140, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

			if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
				// Render Aimbot tab
				if (ImGui::Button("% Aimbot", ImVec2(120, 50))) {
					gui::settings::activeTab = TAB::AIMBOT;
				}
				// Render ESP tab
				if (ImGui::Button("~ ESP", ImVec2(120, 50))) {
					gui::settings::activeTab = TAB::ESP;
				}
				// Render Misc tab
				if (ImGui::Button("} Misc", ImVec2(120, 50))) {
					gui::settings::activeTab = TAB::MISC;
				}
				// Render Config tab
				if (ImGui::Button("# Config", ImVec2(120, 50))) {
					gui::settings::activeTab = TAB::CONFIG;
				}

				ImGui::EndChild();
				ImGui::SameLine();
			}

			ImGui::PopStyleVar(2); // Pop style changes
			ImGui::BeginChild("TabContent", ImVec2(0, 0), true, ImGuiWindowFlags_NoBackground);
			switch (gui::settings::activeTab) {
			case TAB::AIMBOT: // AIMBOT
				CustomCheckbox("Enable Aimbot", &gui::settings::aimbot::enable, checkboxColor);
				if (gui::settings::aimbot::enable) {
					CustomCheckbox("Silent", &gui::settings::aimbot::silent, checkboxColor);
					float fov = static_cast<float>(gui::settings::aimbot::fov); // Convert to float
					RenderSliderWithColor("FOV", &fov, 0.0f, 180.0f);
					gui::settings::aimbot::fov = static_cast<int>(fov); // Convert back to int if necessary
					float smooth = static_cast<float>(gui::settings::aimbot::smooth); // Convert to float
					RenderSliderWithColor("Smooth", &smooth, 0.0f, 99.0f);
					gui::settings::aimbot::smooth = static_cast<int>(smooth); // Convert back to int if necessary
					CustomCheckbox("RCS", &gui::settings::aimbot::rcs, checkboxColor);
					if (gui::settings::aimbot::rcs) {
						float rcsAmount = static_cast<float>(gui::settings::aimbot::rcsAmmount); // Convert to float
						RenderSliderWithColor("Amount", &rcsAmount, 0, 100);
						gui::settings::aimbot::rcsAmmount = static_cast<int>(rcsAmount); // Convert back to int if necessary
					}
					CustomCheckbox("AutoStop", &gui::settings::aimbot::autostop, checkboxColor);
				}
				break;


			case TAB::ESP: // ESP
				CustomCheckbox("Enable ESP", &gui::settings::esp::enable, checkboxColor);

				if (gui::settings::esp::enable) {
					CustomCheckbox("Boxes", &gui::settings::esp::boxes, checkboxColor);
					CustomCheckbox("Name", &gui::settings::esp::name, checkboxColor);
					CustomCheckbox("Health", &gui::settings::esp::health, checkboxColor);
					CustomCheckbox("Weapon", &gui::settings::esp::weapon, checkboxColor);
					CustomCheckbox("DroppedWeapon", &gui::settings::esp::droppedweapons, checkboxColor);
					CustomCheckbox("Draw C4", &gui::settings::esp::drawc4, checkboxColor);
					CustomCheckbox("Bomb Info", &gui::settings::misc::bomb, checkboxColor);
					CustomCheckbox("Player Flags", &gui::settings::esp::flags, checkboxColor);
					ImGui::Text("Chams");
					ImGui::Separator();
					CustomCheckbox("Visible Chams", &gui::settings::chams::visible, checkboxColor);
					if (gui::settings::chams::visible) {
						ImGui::ColorEdit3("Visible", (float*)&gui::settings::chams::visiblecolor, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					}
					CustomCheckbox("Invinsible Chams", &gui::settings::chams::invisible, checkboxColor);
					if (gui::settings::chams::invisible) {}
					ImGui::ColorEdit3("Invisible", (float*)&gui::settings::chams::hiddencolor, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				}
				break;
			case TAB::MISC: // MISC
				ImGui::Text("Movement");
				ImGui::Separator();
				CustomCheckbox("Auto Bunnyhop", &gui::settings::misc::bhop, checkboxColor);
				CustomCheckbox("Enable Radarhack", &gui::settings::misc::radar, checkboxColor);
				CustomCheckbox("Enable Velocity", &gui::settings::misc::velocity, checkboxColor);
				if (gui::settings::misc::velocity) {
					float velocityY = static_cast<float>(gui::settings::misc::velocityposition); // Convert to float
					RenderSliderWithColor("Velocity Y", &velocityY, 0, 1920);
					gui::settings::misc::velocityposition = static_cast<int>(velocityY); // Convert back to int if necessary
					CustomCheckbox("Show Last Velocity", &gui::settings::misc::showLastVelocity, checkboxColor);
				}
				CustomCheckbox("Enable Stamina", &gui::settings::misc::stamina, checkboxColor);
				if (gui::settings::misc::stamina) {
					float staminaY = static_cast<float>(gui::settings::misc::staminaposition); // Convert to float
					RenderSliderWithColor("Stamina Y", &staminaY, 0, 1920);
					gui::settings::misc::staminaposition = static_cast<int>(staminaY); // Convert back to int if necessary
				}
				CustomCheckbox("Enable Indicators", &gui::settings::misc::indicators, checkboxColor);
				if (gui::settings::misc::indicators) {
					float indicatorsY = static_cast<float>(gui::settings::misc::indicatorpos); // Convert to float
					RenderSliderWithColor("Indicators Y", &indicatorsY, 0, 1920);
					gui::settings::misc::indicatorpos = static_cast<int>(indicatorsY); // Convert back to int if necessary
				}
				CustomCheckbox("Jumpbug [MOUSE4], WORKING WITH CONSOLE BHOP", &gui::settings::misc::jumpbug, checkboxColor);
				CustomCheckbox("Edgebug [MOUSE5]", &gui::settings::misc::edgebug, checkboxColor);
				CustomCheckbox("Edgejump [MOUSE3], SHITWORKING", &gui::settings::misc::edgejump, checkboxColor);

				ImGui::TextColored(ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 1.0f), "Visual");
				ImGui::Separator();
				CustomCheckbox("Watermark", &gui::settings::misc::watermark, checkboxColor);
				CustomCheckbox("Spectator List", &gui::settings::misc::spectatorlist, checkboxColor);
				ImGui::ColorEdit3("Spectating Color", (float*)&gui::settings::misc::specclr, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoInputs);
				if (gui::settings::misc::spectatorlist) {
					float specindicatorsX = static_cast<float>(gui::settings::misc::spectatorListPosX); // Convert to float
					RenderSliderWithColor("X", &specindicatorsX, 0, 1920);
					gui::settings::misc::spectatorListPosX = static_cast<int>(specindicatorsX); // Convert back to int if necessary
					float specindicatorsY = static_cast<float>(gui::settings::misc::spectatorListPosY); // Convert to float
					RenderSliderWithColor("Y", &specindicatorsY, 0, 1920);
					gui::settings::misc::spectatorListPosY = static_cast<int>(specindicatorsY); // Convert back to int if necessary
				}
				CustomCheckbox("Discord", &gui::settings::misc::discord, checkboxColor);
				ImGui::ColorEdit3("Menu color", (float*)&gui::settings::misc::checkboxColor, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoInputs);
				CustomCheckbox("Music Display", &gui::settings::misc::show_browser_page, checkboxColor);
				break;

			case TAB::CONFIG:
			{
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
						gui::settings::configs::SaveConfig(fullFileName);
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
						if (std::filesystem::exists("C:/HW$/" + fullFileName)) {
							gui::settings::configs::SaveConfig(fullFileName);
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
					gui::settings::configs::LoadConfig(fullFileName);
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete")) {
					std::string configName(configNameBuffer);
					std::string fullFileName = configName + ".hw$";
					if (!configName.empty()) {
						std::filesystem::path filePath = "C:/HW$/" + fullFileName;
						if (std::filesystem::exists(filePath)) {
							std::filesystem::remove(filePath);
						}
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset")) {
					gui::settings::esp::enable = false;
					gui::settings::esp::boxes = false;
					gui::settings::esp::name = false;
					gui::settings::esp::health = false;
					gui::settings::esp::weapon = false;
					gui::settings::esp::drawc4 = false;
					gui::settings::esp::flags = false;

					gui::settings::aimbot::enable = false;
					gui::settings::aimbot::silent = false;
					gui::settings::aimbot::fov = 0.0f;
					gui::settings::aimbot::smooth = 0;
					gui::settings::aimbot::rcs = false;
					gui::settings::aimbot::rcsAmmount = 0;
					gui::settings::aimbot::autostop = false;

					gui::settings::misc::bhop = false;
					gui::settings::misc::radar = false;
					gui::settings::misc::velocity = false;
					gui::settings::misc::velocityposition = 0;
					gui::settings::misc::stamina = false;
					gui::settings::misc::staminaposition = 0;
					gui::settings::misc::jumpbug = false;
					gui::settings::misc::watermark = false;
					gui::settings::misc::showLastVelocity = false;
					gui::settings::misc::show_browser_page = false;
					gui::settings::chams::visible = false;
					gui::settings::chams::invisible = false;
				}
				}
				break;
			}
			}
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
	std::cout << "WindowProc received message: " << msg << std::endl;
	// Toggle menu on INSERT key press
	if (msg == WM_KEYDOWN && wParam == VK_INSERT)
	{
		gui::MenuOpen = !gui::MenuOpen;
		return true; // Prevent further processing of input
	}

	// If the menu is open, process ImGui input events
	if (gui::MenuOpen && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	// If the menu is open, prevent input events from reaching the game window
	if (gui::MenuOpen)
		return true;

	// Forward input events to the game window
	return CallWindowProc(gui::originalWindowProcess,
		hWnd,
		msg,
		wParam,
		lParam
	);
}
