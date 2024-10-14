#include "misc.h"
#include <string>
#define FL_ONGROUND (1<<0) //0001
#include "../interfaces/IGlobalVars.h"
#include "renderer.h"
#include "renderer.cpp"
#include "/Users/Goshan/Desktop/source cssoupd/ext/imgui/imgui.h"
#include "/Users/Goshan/Desktop/source cssoupd/ext/imgui/imgui_internal.h"
#include "../interfaces/gameevent.h"
#include "../interfaces/qangle.hpp"
#include "../interfaces/matrix.cpp"
#define MASK_SOLID_BRUSHONLY  (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define MASK_SOLID_PLAYERSOLID
#define CONTENTS_SOLID 0x1
#define CONTENTS_MOVEABLE 0x4000
#define CONTENTS_WINDOW 0x2
#define CONTENTS_GRATE 0x8
#define CONTENTS_MONSTER       0x2000000
#define MASK_PLAYERSOLID       (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)

#include "../interfaces/IGlobalVars.h"
#include "../prediction/engine_prediction.hpp"
#define M_PI       3.14159265358979323846   // pi
#include <Psapi.h>

//void Misc::Bhop(UserCmd* cmd)
//{
//    // Check if Bunny Hop is enabled
//    if (!gui::settings::misc::bhop)
//        return;
//
//    // Check if local player is valid
//    if (!globals::localPlayer)
//        return;
//
//    // Define the trace ray
//    Ray_t ray;
//    trace_t traceResult;
//
//    // Set the ray start position at the player's feet
//    ray.m_Start = globals::localPlayer->m_vecOrigin;
//    ray.m_Start.z -= 2.0f; // Adjust for player height
//
//    // Set the ray end position slightly below the player
//    ray.m_Delta = Vector3(0.0f, 0.0f, -10.0f);
//
//    // Perform the trace ray to check if the player is on the ground
//    engineTrace->TraceRay(ray, MASK_PLAYERSOLID, nullptr, &traceResult);
//
//    // If the trace hit something solid, consider the player as being on the ground
//    bool onGround = (traceResult.fraction < 1.0f);
//
//    // If the player is not on the ground, simulate jumping
//    if (!onGround)
//    {
//        cmd->buttons |= IN_JUMP; // Set the jump button
//    }
//    else
//    {
//        cmd->buttons &= ~IN_JUMP; // Clear the jump button
//    }
//}

void Misc::RadarHack()
{
    if (!globals::playerResource || !gui::settings::misc::radar)
        return;

    for (int i = 1; i <= 32; ++i)
    {
        globals::playerResource->m_bPlayerSpotted[i] = true;
        globals::playerResource->m_bHostageSpotted[i] = true;
    }

    if (globals::C4 || globals::plantedC4)
        globals::playerResource->m_bBombSpotted = true;
}



void Misc::Bhop(UserCmd* cmd)
{
    if (!gui::settings::misc::bhop)
        return;

    if (globals::localPlayer && !(globals::localPlayer->m_fFlags & FL_ONGROUND))
    {
        cmd->buttons |= IN_JUMP; // Execute jump command
        cmd->buttons &= ~IN_JUMP; // Clear jump command to prevent continuous jumping
    }
}

void Misc::JumpBug(UserCmd* cmd)
{
    if (!gui::settings::misc::jumpbug)
        return;

    static bool unduck = false;
    float max_radius = M_PI * 2;
    float step = max_radius / 128;
    float xThick = 23;

    if (unduck)
    {
        cmd->buttons &= ~IN_DUCK; // Remove duck command
        unduck = false;
    }

    // Check if the player is on the ground before applying JumpBug
    if (!(globals::localPlayer->m_fFlags & FL_ONGROUND))
    {
        // Get predicted player origin after jump
        Vector3 predictedPos = globals::localPlayer->m_vecOrigin + (globals::localPlayer->m_vecVelocity * globals::intervalPerTick * 2); // Adjust the multiplier as needed

        bool shouldDuck = false;

        for (float a = 0.f; a < max_radius; a += step)
        {
            Vector3 pt;
            pt.x = (xThick * cos(a)) + predictedPos.x;
            pt.y = (xThick * sin(a)) + predictedPos.y;
            pt.z = predictedPos.z;

            Vector3 pt2 = pt;
            pt2.z -= 6;

            trace_t trace;
            Ray_t ray;
            ray.Init(pt, pt2);

            CTraceFilter filter(globals::localPlayer);
            engineTrace->TraceRay(ray, MASK_SOLID_BRUSHONLY, &filter, &trace);

            if (trace.fraction != 1.f && trace.fraction != 0.f)
            {
                shouldDuck = true;
                break; // Exit the loop if we should duck
            }
        }

        if (shouldDuck)
        {
            cmd->buttons |= IN_DUCK; // Set duck command
            unduck = true;
            return; // Exit the function after setting duck
        }
    }
}

void Misc::EdgeJump(UserCmd* cmd)
{
    if (!gui::settings::misc::edgejump || !globals::localPlayer || !(globals::localPlayer->m_fFlags & FL_ONGROUND))
        return;

    // Save current tick base
    int originalTickBase = globals::localPlayer->m_nTickBase;

    // Adjust the tick base
    globals::localPlayer->m_nTickBase += 1;

    // Get predicted position after jump
    Vector3 predictedPos = globals::localPlayer->m_vecOrigin + (globals::localPlayer->m_vecVelocity * globals::intervalPerTick * 0.70); // Adjust the multiplier as needed

    // Reset tick base to original value
    globals::localPlayer->m_nTickBase = originalTickBase;

    const float xThick = 23;
    const float max_radius = M_PI * 2;
    const float step = max_radius / 128;

    Vector3 endPos = predictedPos + (globals::localPlayer->m_vecVelocity * globals::intervalPerTick * 0.30); // Calculate the endpoint of the predicted trajectory

    bool isNearEdge = false; // Flag to indicate if the player is near an edge

    for (float a = 0.f; a < max_radius; a += step)
    {
        Vector3 pt;
        pt.x = (xThick * cos(a)) + endPos.x;
        pt.y = (xThick * sin(a)) + endPos.y;
        pt.z = endPos.z;

        Vector3 pt2 = pt;
        pt2.z -= 32; // Adjust the distance to check for the edge

        trace_t trace;
        Ray_t ray;
        ray.Init(pt, pt2);

        CTraceFilter filter(globals::localPlayer);
        engineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

        if (trace.fraction == 1.f) // If the ray doesn't hit anything
        {
            isNearEdge = true; // Set flag to indicate near edge
            break; // Exit the loop
        }
    }

    if (isNearEdge)
    {
        cmd->buttons |= IN_JUMP; // Execute jump command
    }
}







void Misc::EdgeBug(UserCmd* cmd)
{
    if (!gui::settings::misc::edgebug)
        return;

    static bool ducked = false;
    const float max_radius = M_PI * 2;
    const float step = max_radius / 128;
    const float xThick = 23;

    if (ducked)
    {
        cmd->buttons &= ~IN_DUCK; // Remove duck command
        ducked = false;
    }

    Vector3 pos = globals::localPlayer->m_vecOrigin;

    for (float a = 0.f; a < max_radius; a += step)
    {
        Vector3 pt;
        pt.x = (xThick * cos(a)) + pos.x;
        pt.y = (xThick * sin(a)) + pos.y;
        pt.z = pos.z;

        Vector3 pt2 = pt;
        pt2.z -= 32; // Adjust the distance to check for the edge

        trace_t trace;
        Ray_t ray;
        ray.Init(pt, pt2);

        CTraceFilter filter(globals::localPlayer);
        engineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

        if (trace.fraction == 1.f) // If the ray doesn't hit anything
        {
            cmd->buttons |= IN_DUCK; // Set duck command
            ducked = true;
            return;
        }
    }
}




void Misc::Velocity(vgui::HFont indiFont)
{

    if (!globals::localPlayer)
        return;

    int screenWidth, screenHeight;
    engine->GetScreenSize(screenWidth, screenHeight);

    if (gui::settings::misc::velocity)
    {
        Vector3 velocity = globals::localPlayer->m_vecVelocity;
        float speed = velocity.Length2D();
        int roundspeed = std::round(speed);
        int alpha;
        Color textColor(255, 255, 255, 255); // Default color (white)
        static int lastVelocity = 0;
        static bool showLastVelocity = false;

        // Determine color based on velocity change
        if (roundspeed > lastVelocity)
            textColor = Color(30, 255, 109, 255); // Green when velocity increases
        else if (roundspeed < lastVelocity)
            textColor = Color(255, 75, 75, 255); // Red when velocity decreases
        else
            textColor = Color(255, 225, 55, 255); // Yellow when velocity remains same

        // Update last velocity if not on ground and set showLastVelocity
        if (!(globals::localPlayer->m_fFlags & FL_ONGROUND) && !showLastVelocity) {
            lastVelocity = roundspeed;
            showLastVelocity = true;
        }
        else if ((globals::localPlayer->m_fFlags & FL_ONGROUND) && showLastVelocity) {
            showLastVelocity = false; // Set to false when player is on the ground
        }

        // Convert speed to string
        std::wstring speedString = std::to_wstring(roundspeed);

        // Display current velocity
        int veloWidth, veloHeight;
        surface->GetTextSize(indiFont, speedString.c_str(), veloWidth, veloHeight);
        int veloPosX = (screenWidth - veloWidth) / 2;
        int veloPosY = gui::settings::misc::velocityposition;
        std::string narrowSpeedString(speedString.begin(), speedString.end());

        int preWidth = 0;
        int preHeight;
        int centralize;

        if (showLastVelocity) {
            std::wstring preSpeedString = L"(" + std::to_wstring(lastVelocity) + L")";
            surface->GetTextSize(indiFont, preSpeedString.c_str(), preWidth, preHeight);
            int prePosX = (screenWidth / 2); // Centering the second element
            int prePosY = gui::settings::misc::velocityposition;
            std::string narrowPreSpeedString(preSpeedString.begin(), preSpeedString.end());
            centralize = 0;
            surface->DrawTextSurface(indiFont, Vector3(prePosX + 1, prePosY, 0.f), narrowPreSpeedString.c_str(), textColor);
        }
        else
        {
            preWidth = 0;
            centralize = 0;
        }

        surface->DrawTextSurface(indiFont, Vector3(veloPosX - (preWidth / 2) - 1, veloPosY, 0.f), narrowSpeedString.c_str(), textColor);
    }
}

void Misc::stamina(vgui::HFont indiFont) 
{
    if (!globals::localPlayer)
        return;

    int screenWidth, screenHeight;
    engine->GetScreenSize(screenWidth, screenHeight);
    if (gui::settings::misc::stamina)
    {
        float stamina = globals::localPlayer->m_flStamina;
        int roundstamina = std::round(stamina);
        std::string staminaString = std::to_string(stamina);
        int staminaalpha;
        if ((roundstamina * 13) >= 255)
        {
            staminaalpha = 255;
        }
        else
        {
            staminaalpha = roundstamina * 13;
        }

        int maxnums;
        if (stamina >= 10)
        {
            maxnums = 5;
        }
        else
        {
            maxnums = 3;
        }
        std::string trimmedStaminaString = staminaString.substr(0, maxnums);
        std::wstring cstaminaString(trimmedStaminaString.begin(), trimmedStaminaString.end());
        int staminaWidth, staminaHeight;
        surface->GetTextSize(indiFont, cstaminaString.c_str(), staminaWidth, staminaHeight);
        int staminaposX = (screenWidth - staminaWidth) / 2;
        int staminaposY = (gui::settings::misc::staminaposition);
        std::string narrowstaminaString(cstaminaString.begin(), cstaminaString.end());
        surface->DrawTextSurface(indiFont, Vector3(staminaposX, staminaposY, 0.f), narrowstaminaString.c_str(), Color(255, 255, 255, staminaalpha));
    }
}

void Misc::Indicators(vgui::HFont indiFont, bool jumpBugKey, bool edgeBugKey , bool edgejumpKey) {
    if (gui::settings::misc::indicators) {
        int screenWidth, screenHeight;
        engine->GetScreenSize(screenWidth, screenHeight);
        Color textColor(255, 255, 255, 255); // Default color (white)

        int middleX = screenWidth / 2; // Calculate the middle of the screen
        int indicatorsY = gui::settings::misc::indicatorpos; // Get Y position from settings

        // Calculate the position to center the indicators
        int indicatorSpacing = 25; // Adjust as needed
        int indicatorX = middleX - 8; // Initial X position, adjusted to the left

        if (jumpBugKey) {
            surface->DrawTextSurface(indiFont, Vector3(indicatorX, indicatorsY, 0.f), "jb", textColor);
            indicatorsY += indicatorSpacing; // Move to the next line
        }

        if (edgeBugKey) {
            surface->DrawTextSurface(indiFont, Vector3(indicatorX, indicatorsY, 0.f), "eb", textColor);
            indicatorsY += indicatorSpacing; // Move to the next line
        }
        if (edgejumpKey) {
            surface->DrawTextSurface(indiFont, Vector3(indicatorX, indicatorsY, 0.f), "ej", textColor);
            indicatorsY += indicatorSpacing; // Move to the next line
        }
    }
}



void Misc::nevidimajaxuynya(vgui::HFont bigFont)
{
    int screenx, screeny;
    engine->GetScreenSize(screenx, screeny);
    int tW, tH;
    const wchar_t* text = L" v";
    surface->GetTextSize(bigFont, text, tW, tH);

    surface->DrawTextSurface(bigFont, Vector3{ (float)(screenx / 22) - (tW / 200) , (float)screeny / 10 }, " v", Color(255, 255, 255, 0));
}



////////////////////////////
//FOR NBOMB
// Draw black background
    //draw_list->AddRectFilled(rect_min, rect_max, IM_COL32(0, 0, 0, 240), rounding);

    //// Draw outlines
    //draw_list->AddRect(
    //    ImVec2(rect_min.x - 1, rect_min.y - 1),
    //    ImVec2(rect_max.x + 1, rect_max.y + 1),
    //    IM_COL32(0, 0, 0, 255), // Black color for outlines
    //    rounding,
    //    ImDrawFlags_RoundCornersAll,
    //    1.f
    //);
    //draw_list->AddRect(
    //    ImVec2(rect_min.x - 0.1, rect_min.y - 0.1),
    //    ImVec2(rect_max.x + 0.1, rect_max.y + 0.1),
    //    ImColor(gui::settings::misc::checkboxColor), // Use checkbox color for inner outlines
    //    rounding,
    //    ImDrawFlags_RoundCornersAll,
    //    0.1f
    //);

    //// Draw outlines for bombtimer using checkbox color
    //draw_list->AddRect(
    //    ImVec2(rect_min.x - 1, rect_min.y - 1),
    //    ImVec2(rect_max.x + 1, rect_max.y + 1),
    //    ImColor(gui::settings::misc::checkboxColor), // Use checkbox color for bombtimer outlines
    //    rounding,
    //    ImDrawFlags_RoundCornersAll,
    //    1.f
    //);
    //draw_list->AddRect(
    //    ImVec2(rect_min.x - 0.1, rect_min.y - 0.1),
    //    ImVec2(rect_max.x + 0.1, rect_max.y + 0.1),
    //    IM_COL32(255, 255, 255, 255), // White color for inner outlines
    //    rounding,
    //    ImDrawFlags_RoundCornersAll,
    //    0.1f
    //);
/// ///////////////////////////
void Misc::BombInfo()
{
    int screenWidth, screenHeight;
    engine->GetScreenSize(screenWidth, screenHeight);
    if (!globals::C4 || globals::C4 != (CC4*)entityList->GetEntityFromClass(ClassID::CC4))
        globals::C4 = (CC4*)entityList->GetEntityFromClass(ClassID::CC4);

    if (!globals::plantedC4 || globals::plantedC4 != (CPlantedC4*)entityList->GetEntityFromClass(ClassID::CPlantedC4))
        globals::plantedC4 = (CPlantedC4*)entityList->GetEntityFromClass(ClassID::CPlantedC4);

    if (!globals::plantedC4 || !globals::plantedC4->m_bBombTicking)
        return;

    auto draw_list = ImGui::GetBackgroundDrawList();

    ImVec2 windowSize = ImGui::GetIO().DisplaySize;

    float timeRemaining = globals::plantedC4->m_flC4Blow - globalVars->currentTime;
    std::string bombtimer = std::to_string(static_cast<float>(timeRemaining));

    ImVec2 timersize = ImGui::CalcTextSize(bombtimer.c_str());

    std::string explosionText = "bomb explodes in: ";
    ImVec2 explosionSize = ImGui::CalcTextSize(explosionText.c_str());

    float totalTextWidth = explosionSize.x + timersize.x;

    float rectWidth = totalTextWidth + 10;

    ImVec2 rect_min = ImVec2((screenWidth / 2) - (rectWidth / 2) - 5, 200);
    ImVec2 rect_max = ImVec2(rect_min.x + rectWidth + 10, 230);

    float rounding = 1; // Получаем радиус скругления углов из стиля ImGui

    // Draw black background
    // draw_list->AddRectFilled(rect_min, rect_max, IM_COL32(0, 0, 0, 240), rounding);

    ImVec2 explosionTextPos = ImVec2(rect_min.x + 5 + 5, rect_min.y + (rect_max.y - rect_min.y) / 2 - explosionSize.y / 2);
    draw_list->AddText(explosionTextPos, IM_COL32(255, 255, 255, 255), explosionText.c_str());

    ImVec2 bombtimerPos = ImVec2(explosionTextPos.x + explosionSize.x, explosionTextPos.y);

    // Change color based on time remaining
    ImU32 textColor = IM_COL32(0, 255, 0, 255); // Default green color
    if (timeRemaining < 10) {
        textColor = IM_COL32(255, 255, 0, 255); // Yellow color if time is below 10
    }
    if (timeRemaining < 5) {
        textColor = IM_COL32(255, 0, 0, 255); // Red color if time is below 5
    }

    draw_list->AddText(bombtimerPos, textColor, bombtimer.c_str());
}

std::string GetObserverModeString(int observerMode)
{
    switch (observerMode)
    {
    case OBS_MODE_NONE:
        return "";
    case OBS_MODE_DEATHCAM:
        return "death cam";
    case OBS_MODE_FREEZECAM:
        return "freeze cam";
    case OBS_MODE_FIXED:
        return "fix";
    case OBS_MODE_IN_EYE:
        return "first person";
    case OBS_MODE_CHASE:
        return "third person";
    case OBS_MODE_ROAMING:
        return "obs";
    default:
        return "";
    }
}

void Misc::SpectatorList()
{
    if (!gui::settings::misc::spectatorlist || !entityList || !engine)
        return;

    int screenWidth, screenHeight;
    engine->GetScreenSize(screenWidth, screenHeight);

    auto draw_list = ImGui::GetBackgroundDrawList();

    // Initialize position variables
    float startPosY = gui::settings::misc::spectatorListPosY;
    float offsetY = 20.0f;

    // Iterate through all players
    for (int i = 1; i <= 32; ++i) // Include player index 32
    {
        CEntity* player = entityList->GetClientEntity(i);
        if (!player)
            continue;

        player_info_t pInfo;
        if (!engine->GetPlayerInfo(player->GetIndex(), &pInfo))
            continue;

        // Check if the player is spectating
        if (player->m_hObserverTarget == 0 || player->m_iObserverMode == OBS_MODE_NONE)
            continue;

        // Get the observer target for the current player
        CEntity* observerTarget = entityList->GetClientEntityFromHandle(player->m_hObserverTarget);
        if (!observerTarget)
            continue;

        player_info_t targetInfo;
        if (!engine->GetPlayerInfo(observerTarget->GetIndex(), &targetInfo))
            continue;

        std::string playerName = pInfo.name;
        std::string targetName = targetInfo.name;

        // Get the observer mode for the current spectator
        int observerMode = observerTarget->m_iObserverMode;
        std::string observerModeString = (observerMode == OBS_MODE_NONE) ? "first person" : GetObserverModeString(observerMode);

        // Calculate position for the current spectator relationship
        ImVec2 text_pos = ImVec2(gui::settings::misc::spectatorListPosX, startPosY);

        // Determine the color based on whether the player is watching or being watched
        ImU32 textColor = IM_COL32(255, 255, 255, 255);
        if (observerTarget == globals::localPlayer)
        {
            textColor = ImColor(gui::settings::misc::specclr);// Red color for spectator watching the local player
        }

        // Draw the text including the observer mode
        std::string text = playerName + " -> " + targetName + " | " + observerModeString;
        draw_list->AddText(text_pos, textColor, text.c_str());

        // Increment Y position for the next spectator relationship
        startPosY += offsetY;
    }
}







void Misc::Watermark() {
    if (gui::settings::misc::watermark)
    {

        auto draw_list = ImGui::GetBackgroundDrawList();
        auto watermark = "HotelWhite | "; // Add separator between watermark and FPS
        auto watermark_size = ImGui::CalcTextSize(watermark);
        auto yOffset = 0;
        auto xOffset = ImGui::GetIO().DisplaySize.x - watermark_size.x - 27 + 10; // Adjusted the offset as needed
        auto offset = 4;

        // Draw filled rectangle for the background
        draw_list->AddRectFilled(ImVec2(xOffset - 65 + 5, 6 + yOffset), ImVec2(xOffset + (4 * 2) + watermark_size.x - 5 + 5, 35 + yOffset), ImColor(0.0f, 0.0f, 0.0f, 1.0f));

        // Draw custom color outline using watermarkColor
        draw_list->AddRect(ImVec2(xOffset - 65 + 5, 6 + yOffset), ImVec2(xOffset + (4 * 2) + watermark_size.x - 5 + 5, 35 + yOffset), ImColor(gui::settings::misc::checkboxColor));


        // Draw watermark text
        draw_list->AddText(ImVec2(xOffset - 53 + 5, 12.5 + yOffset), ImColor(255, 255, 255, 255), watermark); // Adjusted the x position slightly to the left

        // Show FPS
        auto fps = std::to_string(int(ImGui::GetIO().Framerate)) + " fps";
        auto fps_size = ImGui::CalcTextSize(fps.c_str());
        auto totalWidth = watermark_size.x + ImGui::CalcTextSize(" ").x + fps_size.x; // Calculate total width of watermark and FPS text
        auto fps_xOffset = xOffset + (watermark_size.x - totalWidth) + 10; // Adjusted xOffset for FPS

        // Draw FPS text
        draw_list->AddText(ImVec2(fps_xOffset + 60, 12.5 + yOffset), ImColor(70, 70, 70, 255), fps.c_str());

    }
}

void Misc::Musicdisplay() {
    {
        static HWND browser_hwnd = nullptr;
        static float last_hwnd_time = 0.f;
        auto draw_list = ImGui::GetBackgroundDrawList();
        // Add a whitelist of allowed process names
        const std::vector<std::wstring> whitelist = { L"chrome.exe", L"brave.exe" , L"sidekick.exe" };

        try {
            if ((!browser_hwnd || browser_hwnd == INVALID_HANDLE_VALUE) && last_hwnd_time < globals::realTime1 + 2.f) {
                for (HWND hwnd = GetTopWindow(0); hwnd; hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
                    last_hwnd_time = globals::realTime1;

                    if (!(IsWindowVisible)(hwnd))
                        continue;

                    int length = (GetWindowTextLengthW)(hwnd);
                    if (length == 0)
                        continue;

                    WCHAR filename[300];
                    DWORD pid;
                    (GetWindowThreadProcessId)(hwnd, &pid);

                    const auto browser_handle = (OpenProcess)(PROCESS_QUERY_INFORMATION, FALSE, pid);
                    (K32GetModuleFileNameExW)(browser_handle, nullptr, filename, 300);

                    std::wstring sane_filename{ filename };

                    (CloseHandle)(browser_handle);

                    if (std::any_of(whitelist.begin(), whitelist.end(),
                        [&](const std::wstring& allowedName) { return sane_filename.find(allowedName) != std::wstring::npos; })) {
                        browser_hwnd = hwnd;
                        break;
                    }
                }
            }
            else if (browser_hwnd && browser_hwnd != INVALID_HANDLE_VALUE) {
                if (!(IsWindowVisible)(browser_hwnd) || !(IsWindow)(browser_hwnd)) {
                    DWORD processId;
                    GetWindowThreadProcessId(browser_hwnd, &processId);
                    const auto browser_process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);

                    if (browser_process) {
                        // Wait for a short period to allow the process to close
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));

                        // Check again if the process is still running
                        if (WaitForSingleObject(browser_process, 0) == WAIT_OBJECT_0) {
                            // The process is still running, wait longer or take appropriate action
                            CloseHandle(browser_process);
                        }
                        else {
                            // Reset related variables to avoid using stale data
                            gui::settings::misc::current_browser_page.clear();
                            browser_hwnd = nullptr;
                        }
                    }
                }
                else {
                    WCHAR title[300];

                    if (!(GetWindowTextW)(browser_hwnd, title, 300)) {
                        // Reset related variables to avoid using stale data
                        gui::settings::misc::current_browser_page.clear();
                        browser_hwnd = nullptr;
                    }
                    else {
                        std::wstring sane_title{ title };
                        std::string page_title(sane_title.begin(), sane_title.end());
                        gui::settings::misc::current_browser_page = page_title;
                    }
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Exception occurred: " << e.what() << std::endl;
        }

        static std::string old_content = gui::settings::misc::current_browser_page;

    }
    // Define variables to store the position of the text
    // Define variables to store the position of the text
    static ImVec2 musicTitlePosition = ImVec2(0, 0);
    static ImVec2 authorPosition = ImVec2(0, 0);

    if (gui::settings::misc::show_browser_page) {
        int w, h;
        engine->GetScreenSize(w, h);
        auto draw_list = ImGui::GetBackgroundDrawList();
        auto txt = gui::settings::misc::current_browser_page;
        auto txt_size = ImGui::CalcTextSize(txt.c_str());
        auto y1Offset = 0;
        auto x1Offset = ImGui::GetIO().DisplaySize.x - txt_size.x - 27; // Adjusted the offset as needed
        auto text_size = ImGui::CalcTextSize(gui::settings::misc::current_browser_page.c_str(), nullptr, true, 0.f);

        // Extract music title from the current browser page title
        std::string music_title;
        std::string music_author;
        size_t found = gui::settings::misc::current_browser_page.find_last_of("|");
        if (found != std::string::npos) {
            music_title = gui::settings::misc::current_browser_page.substr(0, found);
            music_author = gui::settings::misc::current_browser_page.substr(found + 1);
        }
        else {
            music_title = gui::settings::misc::current_browser_page;
            music_author = "Unknown";
        }

        // Set position of the text if it's not already set
        if (musicTitlePosition.x == 0 && musicTitlePosition.y == 0) {
            musicTitlePosition = ImVec2(x1Offset - 30, 43 + y1Offset);
            authorPosition = ImVec2(x1Offset + txt_size.x - ImGui::CalcTextSize(music_author.c_str()).x, 43 + y1Offset + text_size.y + 2);
        }

        // Display the music title aligned to the right
        draw_list->AddText(musicTitlePosition, ImColor(255, 255, 255, 255), music_title.c_str());

        // Display the author aligned to the right below the music title
        draw_list->AddText(authorPosition, ImColor(255, 255, 255, 255), music_author.c_str());
    }

}
//void Misc::DrawHitmarker() {
//
//
//    if (!globals::localPlayer || globals::localPlayer->IsAlive()) return;
//
//
//    auto curtime = globalVars->realTime;
//    float lineSize = 5.f;
//    float flHurtTime;
//    if (flHurtTime + 0.8f >= curtime) {
//        int szX, szY;
//        int sx, sy;
//        engine->GetScreenSize(szX, szY);
//
//        sx = szX / 2;
//        sy = szY / 2;
//        Color bg = Color(0, 0, 0, 200);
//        Color white = Color(255, 255, 255, 255);
//
//        Render::Get().RenderLine(sx - lineSize, sy - lineSize, sx - (lineSize / 4), sy - (lineSize / 4), Color(255, 255, 255, 255), 1.0f);
//        Render::Get().RenderLine(sx - lineSize, sy + lineSize, sx - (lineSize / 4), sy + (lineSize / 4), Color(255, 255, 255, 255), 1.0f);
//        Render::Get().RenderLine(sx + lineSize, sy + lineSize, sx + (lineSize / 4), sy + (lineSize / 4), Color(255, 255, 255, 255), 1.0f);
//        Render::Get().RenderLine(sx + lineSize, sy - lineSize, sx + (lineSize / 4), sy - (lineSize / 4), Color(255, 255, 255, 255), 1.0f);
//
//
//    }
//}

//void Misc::Clan() {
//    if (!gui::settings::misc::clantag)
//        return;
//
//    static bool Reset = true;
//    static float lastUpdateTime = 0.0f;
//    static int currentTagIndex = 0;
//    static const std::vector<std::string> clantags = {
//        "v1.6_g", "v1.6_g#", "v1.6_gl", "v1.6_gl?", "v1.6_gl0|",
//        "v1.6_gl0s", "v1.6_gl0s8", "v1.6_gl0ss", "v1.6_gl0ss+",
//        "v1.6_gloss", "v1.6_gl?+s", "v1.6_gl?+", "v1.6+gl?s",
//        "v1.6_glo", "v1.6_g-", "v1.6_gloss"
//    };
//
//    float currentTime = globalVars->currentTime;
//
//    if (!engine->IsInGame() || !engine->IsConnected())
//        return;
//
//    if (currentTime - lastUpdateTime > 1.0f) {
//        if (Reset || currentTime>= lastUpdateTime + 1.0f) {
//            currentTagIndex = (currentTagIndex + 1) % clantags.size();
//            util::SetClantag(clantags[currentTagIndex].c_str());
//            lastUpdateTime = currentTime;
//            Reset = false;
//        }
//    }
//
//    if (currentTime < lastUpdateTime - 1.0f)
//        Reset = true;
//}
