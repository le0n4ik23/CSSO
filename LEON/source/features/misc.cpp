#include "misc.h"
#include "../hooks.h"
#include "../classes/c_move_data.h"
#include "../classes/client_prediction.h"
#include "../classes/game_movement.h"
#include "../interfaces/QAngle.hpp"
#include "../util/mem.h"
#undef max
#undef min

#include <cmath>
#include <algorithm>
#include <cwchar> 
#include "../pch.h"
#include <iostream>
#include <sstream>
#include <string>
#include "../interfaces/IGlobalVars.h"
#include "../interfaces/engine_prediction.h"
#include "../interfaces/interfaces.h"
#define MASK_SOLID_BRUSHONLY  (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define	MASK_ALL					(0xFFFFFFFF)
#define CONTENTS_SOLID 0x1
#define CONTENTS_MOVEABLE 0x4000
#define CONTENTS_WINDOW 0x2
#define CONTENTS_GRATE 0x8
#define M_PI       3.14159265358979323846   // pi
#define VK_KEY_W 0x57 // Клавиша W
#define VK_KEY_A 0x41 // Клавиша A
#define VK_KEY_S 0x53 // Клавиша S
#define VK_KEY_D 0x44 // Клавиша D
#define MASK_PLAYERSOLID       (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define IM_PI 3.14159265358979323846f

inline float RAD2DEG(float rad) {
    return rad * (180.0f / static_cast<float>(M_PI));
}
void DrawDropShadowText(ImFont* font, float size, ImVec2 pos, ImU32 color, const char* text) {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    int alpha = (color >> IM_COL32_A_SHIFT) & 0xFF;
    ImU32 shadowColor = ImColor(0, 0, 0, alpha / 1.3);
    ImVec2 newPos = ImVec2(pos.x + 2, pos.y + 2);
    drawList->AddText(font, size, newPos, shadowColor, text);
    drawList->AddText(font, size, pos, color, text);
}
// Converts degrees to radians
inline float DEG2RAD(float deg) {
    return deg * (static_cast<float>(M_PI) / 180.0f);
}

bool bShouldJump = true;
void Misc::Bhop(UserCmd* cmd)
{
    if (!gui::settings::misc::bhop)
        return;
    

    static bool bFirstJump, bFakeJump;

    if (cmd->buttons & IN_JUMP) {
        if (!bFirstJump) {
            bFirstJump = bFakeJump = true;
        }
        else if (!(c_engine_prediction::Get().flags & FL_ONGROUND)) {
            if (bFakeJump && globals::localPlayer->m_vecVelocity.z < 0.0f)
                bFakeJump = false;
            else
                cmd->buttons &= ~IN_JUMP;
        }
        else {
            bFakeJump = true;
        }
    }
    else {
        bFirstJump = false;
    }
}

void angle_vectorss(const Vector3& angles, Vector3& forward) {
    float sp = sin(DEG2RAD(angles.x));
    float cp = cos(DEG2RAD(angles.x));
    float sy = sin(DEG2RAD(angles.y));
    float cy = cos(DEG2RAD(angles.y));

    forward.x = cp * cy;
    forward.y = cp * sy;
    forward.z = -sp;
}

void Misc::FastStop(UserCmd* cmd) {
   

    // Получаем скорость игрока
    Vector3 velocity = globals::localPlayer->m_vecVelocity;
    float speed = velocity.Length2D();


    bool holding_w = cmd->buttons & IN_FORWARD;
    bool holding_a = cmd->buttons & IN_MOVELEFT;
    bool holding_s = cmd->buttons & IN_BACK;
    bool holding_d = cmd->buttons & IN_MOVERIGHT;
    if (globals::localPlayer->m_fFlags & 1)
        return;
    if (gui::settings::aimbot::detectAutoPeek)
        return;
    if (!(!holding_a && !holding_d && !holding_w && !holding_s))
        return;
        // Вычисляем направление остановки
        Vector3 direction = velocity.ToAngle();
        direction.y = cmd->viewAngles.y - direction.y;

        // Создаем вектор направления, умножая на отрицательную скорость
        Vector3 negated_direction = direction.ToVector() * -speed;

        // Устанавливаем значения forwardMove и sideMove
        cmd->forwardMove = negated_direction.x; // Устанавливаем обратное движение по оси X
        cmd->sideMove = negated_direction.y;     // Устанавливаем обратное движение по оси Y
    
}


int iFlagsBackup;
int flZVelBackup;
Vector3 backuporigin;
Vector3 backupvelocity;
float flBugSpeed;

int iEdgebugButtons;
#define FL_DUCKING (1 << 1)
float NormalizeAngle(float angle) {
    while (angle > 180.0f)
        angle -= 360.0f;
    while (angle < -180.0f)
        angle += 360.0f;
    return angle;
}


void VectorAngless(const Vector3& vecForward, Vector3& vecAngles)
{
    if (vecForward.x == 0.f && vecForward.y == 0.f)
    {
        vecAngles.x = (vecForward.z > 0.f) ? 90.f : -90.f;
        vecAngles.y = 0.f;
    }
    else
    {
        vecAngles.y = std::atan2(vecForward.y, vecForward.x) * 180.f / M_PI;

        if (vecAngles.y < 0.f)
            vecAngles.y += 360.f;

        float forwardLength = std::sqrt(vecForward.x * vecForward.x + vecForward.y * vecForward.y);
        vecAngles.x = std::atan2(vecForward.z, forwardLength) * 180.f / M_PI;
    }

    vecAngles.z = 0.f;
}
void VectorAngless(const Vector3& vecForward, QAngle& vecAngles)
{
    if (vecForward.x == 0.f && vecForward.y == 0.f)
    {
        vecAngles.pitch = (vecForward.z > 0.f) ? 90.f : -90.f;
        vecAngles.yaw = 0.f;
    }
    else
    {
        vecAngles.yaw = std::atan2(vecForward.y, vecForward.x) * 180.f / M_PI;

        if (vecAngles.yaw < 0.f)
            vecAngles.yaw += 360.f;

        float forwardLength = std::sqrt(vecForward.x * vecForward.x + vecForward.y * vecForward.y);
        vecAngles.pitch = std::atan2(vecForward.z, forwardLength) * 180.f / M_PI;
    }

    vecAngles.roll = 0.f; // Угол по оси Z всегда 0 в этой функции
}
void Misc::EdgeBug(UserCmd* cmd) {
   
    if (globals::localPlayer->m_fFlags & 1)
        return;
    



    
    static bool edgebugging = false;
    static int edgebugging_tick = 0;

    if (!edgebugging) {
        int flags = globals::localPlayer->m_fFlags;
        float z_velocity = floor(globals::localPlayer->m_vecVelocity.z);

        for (int i = 0; i < 64; i++) {
            // Run prediction
            c_engine_prediction::Get().start(cmd, globals::localPlayer);
            {

                // Check for edgebug
                if (z_velocity < -7 && floor(globals::localPlayer->m_vecVelocity.z) == -7 && !(flags & FL_ONGROUND)) {
                    edgebugging = true;
                    edgebugging_tick = cmd->tick_count + i;
                    break;
                }
                else {
                    z_velocity = floor(globals::localPlayer->m_vecVelocity.z);
                    flags = globals::localPlayer->m_fFlags;
                }
            }

            // End prediciton
            c_engine_prediction::Get().end(cmd, globals::localPlayer);
        }
    }
    else {
        // Lock the movement however you want
        cmd->sideMove = 0.f;
        cmd->forwardMove = 0.f;
        cmd->upmove = 0.f;
        cmd->mousedx = 0.f;

        // Check if edgebug over
        if (cmd->tick_count > edgebugging_tick) {
            edgebugging = false;
            edgebugging_tick = 0;
        }
    }
}
#define MASK_SHOT (0x1 | 0x4000 | 0x2000000 | 0x2 | 0x4000000 | 0x40000000)
void Misc::Align(UserCmd* cmd) {
    // Проверяем, что игрок в игре и жив
   
    if (globals::localPlayer->m_fFlags & FL_ONGROUND) {
        return; // Игрок на земле, выходим из функции
    }

    Vector3 wishdir;
    bool done = false;

    // Определяем направление движения на основе нажатых кнопок
    if ((cmd->buttons & IN_FORWARD) && !(cmd->buttons & IN_BACK) && !(cmd->buttons & IN_MOVELEFT) && !(cmd->buttons & IN_MOVERIGHT)) {
        wishdir = { std::cos(DEG2RAD(cmd->viewAngles.y)) * 17.f, std::sin(DEG2RAD(cmd->viewAngles.y)) * 17.f, 0.f };
        done = true;
    }
    if ((cmd->buttons & IN_BACK) && !(cmd->buttons & IN_FORWARD) && !(cmd->buttons & IN_MOVELEFT) && !(cmd->buttons & IN_MOVERIGHT)) {
        wishdir = { std::cos(DEG2RAD(cmd->viewAngles.y + 180.f)) * 17.f, std::sin(DEG2RAD(cmd->viewAngles.y + 180.f)) * 17.f, 0.f };
        done = true;
    }
    if ((cmd->buttons & IN_MOVELEFT) && !(cmd->buttons & IN_BACK) && !(cmd->buttons & IN_FORWARD) && !(cmd->buttons & IN_MOVERIGHT)) {
        wishdir = { std::cos(DEG2RAD(cmd->viewAngles.y + 90.f)) * 17.f, std::sin(DEG2RAD(cmd->viewAngles.y + 90.f)) * 17.f, 0.f };
        done = true;
    }
    if ((cmd->buttons & IN_MOVERIGHT) && !(cmd->buttons & IN_BACK) && !(cmd->buttons & IN_MOVELEFT) && !(cmd->buttons & IN_FORWARD)) {
        wishdir = { std::cos(DEG2RAD(cmd->viewAngles.y - 90.f)) * 17.f, std::sin(DEG2RAD(cmd->viewAngles.y - 90.f)) * 17.f, 0.f };
        done = true;
    }

    if (!done) return;

    const auto startPos = globals::localPlayer->m_vecOrigin;
    const auto endPos = (startPos + (wishdir * 1.1f));

    trace_t trace;
    CTraceFilter filter(globals::localPlayer);
    Ray_t ray;
    ray.Init(startPos, endPos);
    engineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

    // Проверяем, попал ли луч в стену
    if (trace.fraction < 1.f) {
        // Определяем вектор к стене (перпендикуляр)
        Vector3 toWall = (endPos - startPos).Normalized();

        // Определяем направление движения для выравнивания
        float mVel = hypotf(globals::localPlayer->m_vecVelocity.x, globals::localPlayer->m_vecVelocity.y);
        float ideal = RAD2DEG(atanf(28.33f / mVel));

        // Рассчитываем углы
        Vector3 newAngles = toWall.ToAngle();
        Vector3 dvelo = globals::localPlayer->m_vecVelocity;
        dvelo.z = 0.f;
        Vector3 velo_angle = dvelo.ToAngle();
        Vector3 delta = velo_angle - newAngles;
        delta.Normalize();

        if (delta.y >= 0.f)
            newAngles.y += ideal;
        else
            newAngles.y -= ideal;

        // Рассчитываем новые движения
        float rotation = DEG2RAD(newAngles.y - cmd->viewAngles.y);
        float cos_rot = cos(rotation);
        float sin_rot = sin(rotation);

        cmd->forwardMove = cos_rot * 450.f;
        cmd->sideMove = -sin_rot * 450.f;
    }
}


void Misc::AutoDuck(UserCmd* cmd) {
     bool should_duck = false;
    if (globals::localPlayer->m_fFlags & 1) 
       
        return;

        
    float z_pos = globals::localPlayer->m_vecOrigin.z;
    
    static int duck_tick = 0;

    // Создаем копию команды
    UserCmd predictcmd = *cmd;

    if (!globals::localPlayer->m_fFlags & 1) {
        int flags = globals::localPlayer->m_fFlags;

        for (int i = 0; i < 4; i++) {
            // Передаем указатель на predictcmd в метод start
            c_engine_prediction::Get().start(&predictcmd, globals::localPlayer);
            {
                Vector3 predictedorigin = c_engine_prediction::Get().predicted_position;
                predictcmd.buttons |= IN_DUCK;
                if ((z_pos + 32) < predictedorigin.z &&
                    (globals::localPlayer->m_fFlags & 1) &&
                    globals::localPlayer->m_vecVelocity.z == 0) {
                  
                    duck_tick = cmd->tick_count + i;
                    break;
                }
            }
            predictcmd.buttons &= IN_DUCK;
            c_engine_prediction::Get().end(&predictcmd, globals::localPlayer);
        }
    }
    if (cmd->tick_count > duck_tick) {
        cmd->buttons |= IN_DUCK;
    }
}



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
std::string state;
static bool jumpBug = false;
static bool edgeJump = false;
static bool miniJump = false;
static float alphaJumpBug = 0.0f;
static float alphaEdgeJump = 0.0f;
static float alphaMiniJump = 0.0f;
static bool dtmj;
static bool dtlj;
static bool dtjb;
void Misc::Indicators()
{
    ImDrawList* draw_list = gui::settings::dlist1;
    ImGui::PushFont(gui::settings::fonts::velocityfont);

    static int screenHeight = hooks::screenheight;
    static int screenWidth = hooks::screenwidth;
    ImVec2 center = ImVec2(screenWidth / 2, screenHeight / 2);
    float offset_y = 0.0f;

    // Позиции для отображения индикаторов (центрированные)
    ImVec2 jumpBugPos = ImVec2(center.x, gui::settings::misc::indicatorposition + offset_y);
    ImVec2 edgeJumpPos = ImVec2(center.x, gui::settings::misc::indicatorposition + offset_y + 30);
    ImVec2 miniJumpPos = ImVec2(center.x, gui::settings::misc::indicatorposition + offset_y + 60);

    ImVec4 ljcolor;

    ImVec4 mjcolor;

    ImVec4 jbcolor;

    // Обработка alpha-анимации для каждого индикатора
    float alpha_decay_rate = 0.1f;
    float alpha_increase_rate = 0.1f;

    if (jumpBug) {
        if (alphaJumpBug < 1.0f) {
            alphaJumpBug += alpha_increase_rate;
            if (alphaJumpBug > 1.0f) alphaJumpBug = 1.0f;
        }
    }
    else { 
        if (alphaJumpBug > 0.0f) {
            alphaJumpBug -= alpha_decay_rate;
            if (alphaJumpBug < 0.0f) alphaJumpBug = 0.0f;
        }
    }

    if (edgeJump) {
        if (alphaEdgeJump < 1.0f) {
            alphaEdgeJump += alpha_increase_rate;
            if (alphaEdgeJump > 1.0f) alphaEdgeJump = 1.0f;
        }
    }
    else {
        if (alphaEdgeJump > 0.0f) {
            alphaEdgeJump -= alpha_decay_rate;
            if (alphaEdgeJump < 0.0f) alphaEdgeJump = 0.0f;
        }
    }

    if (miniJump) {
        if (alphaMiniJump < 1.0f) {
            alphaMiniJump += alpha_increase_rate;
            if (alphaMiniJump > 1.0f) alphaMiniJump = 1.0f;
        }
    }
    else {
        if (alphaMiniJump > 0.0f) {
            alphaMiniJump -= alpha_decay_rate;
            if (alphaMiniJump < 0.0f) alphaMiniJump = 0.0f;
        }
    }
    static float dtjbtimer = 0.0f;
    static float dtljtimer = 0.0f;
    static float dtmjtimer = 0.0f;
   const float delay = 0.2f;
   if (dtjb) {
       dtjbtimer = ImGui::GetTime(); // Запускаем таймер
       dtjb = false; // Отключаем запуск до следующего изменения
   }
   else {
       float currentTime = ImGui::GetTime(); // Получаем текущее время
       float elapsedTime = currentTime - dtjbtimer; // Вычисляем прошедшее время

       if (elapsedTime >= delay) {
           jbcolor = gui::settings::misc::firstindicatorscolor; // Задаем цвет, если задержка прошла
       }
       else {
           jbcolor = gui::settings::misc::secondindicatorscolor; // Задаем цвет в противном случае
       }
   }

   // Проверка для dtlj
   if (dtlj) {
       dtljtimer = ImGui::GetTime(); // Запускаем таймер
       dtlj = false; // Отключаем запуск до следующего изменения
   }
   else {
       float ljcurrentTime = ImGui::GetTime(); // Получаем текущее время
       float ljelapsedTime = ljcurrentTime - dtljtimer; // Вычисляем прошедшее время

       if (ljelapsedTime >= delay) {
           ljcolor = gui::settings::misc::firstindicatorscolor; // Задаем цвет, если задержка прошла
       }
       else {
           ljcolor = gui::settings::misc::secondindicatorscolor; // Задаем цвет в противном случае
       }
   }

   // Проверка для dtmj
   if (dtmj) {
       dtmjtimer = ImGui::GetTime(); // Запускаем таймер
       dtmj = false; // Отключаем запуск до следующего изменения
   }
   else {
       float mjcurrentTime = ImGui::GetTime(); // Получаем текущее время
       float mjelapsedTime = mjcurrentTime - dtmjtimer; // Вычисляем прошедшее время

       if (mjelapsedTime >= delay) {
           mjcolor = gui::settings::misc::firstindicatorscolor; // Задаем цвет, если задержка прошла
       }
       else {
           mjcolor = gui::settings::misc::secondindicatorscolor; // Задаем цвет в противном случае
       }
   }
    // Отрисовка индикаторов
    if (alphaJumpBug > 0.0f) {
        ImVec2 text_size = gui::settings::fonts::indicatorsfont->CalcTextSizeA(gui::settings::fonts::indicatorsfontsize, FLT_MAX, 0.0f, "jb");
        draw_list->RenderText(gui::settings::fonts::indicatorsfont, gui::settings::fonts::indicatorsfontsize, ImVec2(jumpBugPos.x - text_size.x / 2, jumpBugPos.y), IM_COL32(static_cast<int>(255 * jbcolor.x), static_cast<int>(255 * jbcolor.y), static_cast<int>(255 * jbcolor.z), static_cast<int>(255 * alphaJumpBug)), "jb", gui::settings::fonts::indicatorsfontflag, gui::settings::fonts::indicatorsfontoffset);
        offset_y += 30.0f;
    }
    if (alphaEdgeJump > 0.0f) {
        ImVec2 text_size = gui::settings::fonts::indicatorsfont->CalcTextSizeA(gui::settings::fonts::indicatorsfontsize, FLT_MAX, 0.0f, "lj");
        draw_list->RenderText(gui::settings::fonts::indicatorsfont, gui::settings::fonts::indicatorsfontsize, ImVec2(edgeJumpPos.x - text_size.x / 2, edgeJumpPos.y), IM_COL32(static_cast<int>(255 * ljcolor.x), static_cast<int>(255 * ljcolor.y), static_cast<int>(255 * ljcolor.z), static_cast<int>(255 * alphaEdgeJump)), "lj", gui::settings::fonts::indicatorsfontflag, gui::settings::fonts::indicatorsfontoffset);
        offset_y += 30.0f;
    }
    if (alphaMiniJump > 0.0f) {
        ImVec2 text_size = gui::settings::fonts::indicatorsfont->CalcTextSizeA(gui::settings::fonts::indicatorsfontsize, FLT_MAX, 0.0f, "mj");
        draw_list->RenderText(gui::settings::fonts::indicatorsfont, gui::settings::fonts::indicatorsfontsize, ImVec2(miniJumpPos.x - text_size.x / 2, miniJumpPos.y), IM_COL32(static_cast<int>(255 * mjcolor.x), static_cast<int>(255 * mjcolor.y), static_cast<int>(255 * mjcolor.z), static_cast<int>(255 * alphaMiniJump)), "mj", gui::settings::fonts::indicatorsfontflag, gui::settings::fonts::indicatorsfontoffset);
        offset_y += 30.0f;
    }

    // Обработка ввода
    if (IsKP(gui::settings::binds::jumpbug)) {
        // Misc::JumpBug(cmd); // Команда должна быть вызвана здесь
        jumpBug = true;
    }
    else {
        jumpBug = false;
    }

    if (IsKP(gui::settings::binds::longjump)) {
        // Misc::EdgeJump(cmd); // Команда должна быть вызвана здесь
        edgeJump = true;
    }
    else {
        edgeJump = false;
    }

    if (IsKP(gui::settings::binds::minijump)) {
        // Misc::MiniJump(cmd); // Команда должна быть вызвана здесь
        miniJump = true;
    }
    else {
        miniJump = false;
    }

    ImGui::PopFont();
}

static float alphaAP = 0.0f;
static float alphaFB = 0.0f;
static bool autopeek;
static bool dtap;
ImVec4 apcolor;
ImVec4 bSendPacketcolor;
const char* bSendPacketStatus;

void UpdateAlpha(float& alpha, bool condition, float rate = 0.1f) {
    if (condition) {
        alpha = std::min(alpha + rate, 1.0f);
    }
    else {
        alpha = std::max(alpha - rate, 0.0f);
    }
}

void Misc::RageIndicators(bool& bsendpacket) {
    ImDrawList* draw_list = gui::settings::dlist1;

    static int screenHeight = hooks::screenheight;
    static int screenWidth = hooks::screenwidth;
    ImVec2 center = ImVec2(screenWidth / 2, (screenHeight / 2) + 20);
    float offset_y = 0.0f;

    ImVec2 appos = ImVec2(center.x, center.y + 35);

    // Обновление альфа-анимации
    UpdateAlpha(alphaAP, autopeek);
    UpdateAlpha(alphaFB, IsKT(gui::settings::binds::forcebaim));

    // Цвет для autopeek
    apcolor = gui::settings::aimbot::detectAutoPeek ?
        gui::settings::misc::firstindicatorscolor :
        gui::settings::misc::secondindicatorscolor;

    // Текстовые индикаторы
    const char* text1 = "hotel";
    const char* text2 = "white";
    ImVec2 text_size1 = ImGui::CalcTextSize(text1);
    ImVec2 text_size2 = ImGui::CalcTextSize(text2);
    ImVec2 startPos = ImVec2(center.x - (text_size1.x + text_size2.x) / 2, center.y);

    draw_list->AddText(startPos, IM_COL32(255, 255, 255, 255), text1);
    startPos.x += text_size1.x;
    draw_list->AddText(startPos, IM_COL32(static_cast<int>(255 * gui::settings::misc::checkboxColor.x),
        static_cast<int>(255 * gui::settings::misc::checkboxColor.y),
        static_cast<int>(255 * gui::settings::misc::checkboxColor.z), 255), text2);

    // Autopeek
    if (alphaAP > 0.0f) {
        ImVec2 text_size = ImGui::CalcTextSize("autopeek");
        offset_y += 30.0f;
        draw_list->AddText(ImVec2(appos.x - text_size.x / 2, center.y + offset_y),
            IM_COL32(static_cast<int>(255 * apcolor.x),
                static_cast<int>(255 * apcolor.y),
                static_cast<int>(255 * apcolor.z),
                static_cast<int>(255 * alphaAP)), "autopeek");
    }

    // bSendPacket
    if (gui::settings::misc::fakelags) {
        const char* bSendPacketText = "bSendPacket: ";
        char formattedText[256];
        snprintf(formattedText, sizeof(formattedText), "%s%s", bSendPacketText, bSendPacketStatus);

        ImVec2 bSendPacket_size = ImGui::CalcTextSize(formattedText);
        offset_y += 30.0f;

        bSendPacketcolor = hooks::bSendPacket ?
            gui::settings::misc::firstindicatorscolor :
            gui::settings::misc::secondindicatorscolor;

        draw_list->AddText(ImVec2(center.x - (bSendPacket_size.x / 2), center.y + offset_y),
            IM_COL32(static_cast<int>(255 * bSendPacketcolor.x),
                static_cast<int>(255 * bSendPacketcolor.y),
                static_cast<int>(255 * bSendPacketcolor.z), 255), formattedText);
    }

    // Force Body
    if (gui::settings::aimbot::forcebaim && (alphaFB > 0.0f)) {
        ImVec2 forcebody_size = ImGui::CalcTextSize("force body");
        offset_y += 30.0f;
        draw_list->AddText(ImVec2(center.x - (forcebody_size.x / 2), center.y + offset_y),
            IM_COL32(255, 255, 255, static_cast<int>(255 * alphaFB)), "force body");
    }

    // Индикатор перезарядки
    if (globals::localPlayer->IsAlive()) {
        CBaseCombatWeapon* weapon = globals::localPlayer->GetActiveWeapon();
        float currentTime = globalVars->currentTime;
        float timeToNextAttack = weapon->m_flNextPrimaryAttack - currentTime;
        float maxFillTime = 1.0f;
        float progress = std::clamp(1.0f - (timeToNextAttack / maxFillTime), 0.0f, 1.0f);

        ImVec2 indicatorPos = ImVec2(center.x - 75, center.y + offset_y + 30);
        ImVec2 indicatorSize = ImVec2(150, 4);
        draw_list->AddRectFilled(indicatorPos, ImVec2(indicatorPos.x + indicatorSize.x, indicatorPos.y + indicatorSize.y), IM_COL32(0, 0, 0, 128), 12);
        float fillWidth = indicatorSize.x * progress;
        ImVec2 fillEndPos = ImVec2(indicatorPos.x + fillWidth, indicatorPos.y + indicatorSize.y);
        draw_list->AddRectFilled(indicatorPos, fillEndPos, IM_COL32(static_cast<int>(255 * gui::settings::misc::checkboxColor.x),
            static_cast<int>(255 * gui::settings::misc::checkboxColor.y),
            static_cast<int>(255 * gui::settings::misc::checkboxColor.z), 255), 12);
        offset_y += 30.0f;
    }

    // Обработка кнопки autopeek
    autopeek = IsKP(gui::settings::binds::autopeekbutton);
}

void Misc::MiniJump(UserCmd* cmd)
{

    
    if ((c_engine_prediction::Get().flags & FL_ONGROUND) && !(globals::localPlayer->m_fFlags & FL_ONGROUND))
    {
        cmd->buttons |= IN_JUMP;
        cmd->buttons |= IN_DUCK;
        dtmj = true;

      

    }
  

}



void addSpacesBeforeCapitals(const char* source, char* destination) {
    int j = 0;
    for (int i = 0; source[i] != '\0'; ++i) {
        if (isupper(source[i]) && i != 0) {
            destination[j++] = ' '; // Добавляем пробел
        }
        destination[j++] = source[i]; // Копируем текущий символ
    }
    destination[j] = '\0'; // Завершаем строку нуль-терминатором
}
void Misc::ClanTag()
{
    std::string state;

    static ImVec2 textsize;
    ImDrawList* draw_list = gui::settings::dlist1;
    for (int i = 1; i < 32; ++i)
    {
        CEntity* player = entityList->GetClientEntity(i);
        if (!player)
            continue;
        if (!globals::playerResource)
            return;
        if (!globals::C4 || globals::C4 != (CC4*)entityList->GetEntityFromClass(ClassID::CC4))
            globals::C4 = (CC4*)entityList->GetEntityFromClass(ClassID::CC4);

        if (!globals::plantedC4 || globals::plantedC4 != (CPlantedC4*)entityList->GetEntityFromClass(ClassID::CPlantedC4))
            globals::plantedC4 = (CPlantedC4*)entityList->GetEntityFromClass(ClassID::CPlantedC4);

        player_info_t carrierinfo;
        player_info_t defuserinfo;

        if (!globals::plantedC4)
        {
            if (globals::playerResource->m_iPlayerC4)
            {
                if (player->GetIndex() == globals::playerResource->m_iPlayerC4)
                {
                
                    const char* lastPlaceName = player->m_szLastPlaceName;
                    char modifiedLastPlaceName[100]; // Достаточный размер для новой строки 
                    addSpacesBeforeCapitals(lastPlaceName, modifiedLastPlaceName);
                    int bombCarrierID = player->GetIndex();
                    

                    Vector3 bombAPos = globals::playerResource->m_bombsiteCenterA;
                    Vector3 bombBPos = globals::playerResource->m_bombsiteCenterB;
                    Vector3 bombCarrierPos = player->m_vecOrigin;
                    engine->GetPlayerInfo(bombCarrierID, &carrierinfo);
                    float distToA = (bombCarrierPos - bombAPos).Length2D();
                    float distToB = (bombCarrierPos - bombBPos).Length2D();

                    std::string closestPlant;
                    if (distToA < distToB)
                        closestPlant = "A";
                    else
                        closestPlant = "B";

                    // Формируем текст для отображения
                    state = ("Bomber " + std::string(carrierinfo.name) + " - " + std::string(modifiedLastPlaceName));
                    textsize = ImGui::CalcTextSize(state.c_str());
                    if (globals::C4 && globals::C4->m_bStartedArming)
                    {
                      
                    }
                }
            }
         
            // Рисуем текст state на экране
        }
        else
        {
            if (globals::plantedC4->m_hBombDefuser != 0xffffffff) {


                CEntity* bombDefuser = entityList->GetClientEntityFromHandle(globals::plantedC4->m_hBombDefuser);

                int bombDefuserID = bombDefuser->GetIndex();
                player_info_t defuserinfo;
                if (engine->GetPlayerInfo(bombDefuserID, &defuserinfo))
                {
                    std::string s = std::to_string(static_cast<float>(globals::plantedC4->m_flDefuseCountDown - globalVars->currentTime));
                    state = std::string(defuserinfo.name) + " defusing: " + s;
                }




            }
            else
            {
                std::string s = (std::to_string((float)(globals::plantedC4->m_flC4Blow - globalVars->currentTime)));
                state = s + " untill explosion";
            }



        }
    }
    


        ImVec2 rect_min = ImVec2(15, 100);
        ImVec2 rect_max = ImVec2(rect_min.x + 20 + textsize.x + 20, rect_min.y + 30);

        // Радиус скругления углов
        float rounding = 12.0f;

        ImU32 firstcolor = IM_COL32(15, 15, 15, 150);
        ImU32 thirdcolor = IM_COL32(5, 5, 5, 80);
        draw_list->AddRectFilled(rect_min, rect_max, firstcolor, rounding);
        // Определите ширину аутлайна
        float outline_width = 4.0f;
        ImU32 outline_color = IM_COL32(
            static_cast<int>(gui::settings::misc::checkboxColor.x * 255),
            static_cast<int>(gui::settings::misc::checkboxColor.y * 255),
            static_cast<int>(gui::settings::misc::checkboxColor.z * 255),
            static_cast<int>(gui::settings::misc::checkboxColor.w * 255)
        );

        float thickness = 2.0f;
        // Рисуем аутлайн прямоугольника
        draw_list->AddRect(
            ImVec2(rect_min.x - 2.2, rect_min.y - 2.2), // Левый верхний угол с учетом аутлайна
            ImVec2(rect_max.x + 2.2, rect_max.y + 2.2), // Правый нижний угол с учетом аутлайна
            thirdcolor,
            rounding,
            ImDrawFlags_RoundCornersAll, // Флаги для скругления всех углов
            thickness + 4
        );
        // Рисуем аутлайн прямоугольника с цветом из переменной checkboxColor
        draw_list->AddRect(
            ImVec2(rect_min.x - 0.2, rect_min.y - 0.2), // Левый верхний угол с учетом аутлайна
            ImVec2(rect_max.x + 0.2, rect_max.y + 0.2), // Правый нижний угол с учетом аутлайна
            outline_color,
            rounding,
            ImDrawFlags_RoundCornersAll, // Флаги для скругления всех углов
            thickness
        );




        float centerY = (rect_min.y + rect_max.y) * 0.5f;

        // Вычислите половину высоты текста
        float halfTextHeight = textsize.y * 0.5f;

        // Вычтите половину высоты текста из вертикальной середины
        float textY = centerY - halfTextHeight;
        draw_list->AddText(ImVec2(rect_min.x + 20, textY), IM_COL32(255, 255, 255, 255), state.c_str());

    }
  
    int tickstojb;
    void Misc::JumpBug(UserCmd* cmd)
    {
             bShouldJump = false;
            float stamina = globals::localPlayer->m_flStamina;
            static float btime;
            if (btime < globalVars->currentTime)
                should_jumpbug = false;
            if (stamina == 0.f)
                should_jumpbug = true;

            if (!globals::localPlayer)
                btime = 0.f;
            if (!(c_engine_prediction::Get().flags & FL_ONGROUND) && (globals::localPlayer->m_fFlags & FL_ONGROUND))
            {
                cmd->buttons |= IN_DUCK;
                cmd->buttons &= ~IN_JUMP;
                btime = globalVars->currentTime + .2;
                should_jumpbug = true;

                if (stamina == 0.f)
                    should_jumpbug = false;
                dtjb = true;


            
        }
    }

void Misc::EdgeJump(UserCmd* cmd)
{   
    static int longjump_tick = 0;
    static bool ljbool = false;

    if ((c_engine_prediction::Get().flags & FL_ONGROUND) && !(globals::localPlayer->m_fFlags & FL_ONGROUND))
    {
        cmd->buttons |= IN_JUMP;
        ljbool = true;

        longjump_tick = globalVars->tickCount + 2;

    }
    if (ljbool)
    {
        if (globalVars->tickCount < longjump_tick) {
            dtlj = true;
            cmd->buttons |= IN_DUCK;
            cmd->buttons &= ~IN_FORWARD;
            cmd->buttons &= ~IN_BACK;

        }
        if (globalVars->tickCount > longjump_tick)
            ljbool = false;
    }
  
}








    std::unordered_map<int, std::string> previousLastPlaceNames;

    void Misc::AutoAlign() {
        for (int i = 1; i < 32; ++i) {
            CEntity* player = entityList->GetClientEntity(i);
            if (!player)
                continue;
            if (player == globals::localPlayer)
                continue;
            if (!globals::playerResource)
                return;
            if (!player->IsAlive())
                continue;
            if (gui::settings::misc::target == 0)
            {
                if (player->m_iTeamNum == globals::localPlayer->m_iTeamNum || !player->m_iTeamNum)
                    continue;
            }
            else
            {
                if (player->m_iTeamNum != globals::localPlayer->m_iTeamNum || !player->m_iTeamNum)
                    continue;
            }
            // Получаем текущее значение m_szLastPlaceName игрока
            const char* currentLastPlaceName = player->m_szLastPlaceName;

            // Получаем предыдущее значение m_szLastPlaceName игрока
            std::string& previousLastPlaceName = previousLastPlaceNames[i];

            // Проверяем, изменилось ли значение m_szLastPlaceName игрока
            if (currentLastPlaceName && previousLastPlaceName == currentLastPlaceName)
                continue;  // Значение не изменилось, пропускаем отправку команды

            // Обновляем предыдущее значение m_szLastPlaceName игрока
            previousLastPlaceName = currentLastPlaceName;

            // Получаем информацию о бомбоносце
            player_info_t carrierinfo;
            engine->GetPlayerInfo(player->GetIndex(), &carrierinfo);

            // Формируем текст для отправки в чат
            char modifiedLastPlaceName[100]; // Достаточный размер для новой строки 
            addSpacesBeforeCapitals(currentLastPlaceName, modifiedLastPlaceName);

            std::stringstream ss;
            std::string chat;
            if (gui::settings::misc::chattarget == 0)
            {
                chat = "say \"";
            }
            else
            {
                chat = "say_team \"";
            }
            ss << chat << "[hotel white]  Player " << carrierinfo.name << " - " << modifiedLastPlaceName << "\"";
            std::string command = ss.str();

            // Выполняем команду сформированной строки
            engine->ClientCmd(command.c_str());
        }
    }




void Misc::Watermark()
{
    surface->SetDrawColor(255,255,255,1);
    surface->DrawLine(0, 0, 1, 1);
  
   
}
CEntity* fogController = nullptr;  // Глобальная или статическая переменная для хранения контроллера

void Misc::FindFogControllers() {
    // Если контроллер уже был найден, обновляем его свойства без перебора всех сущностей
    if (fogController) {
        // Включить туман
        fogController->m_fogenable = true;

        // Установить начальную и конечную точки тумана
        fogController->m_fogstart = gui::settings::esp::fogmin;
        fogController->m_fogend = gui::settings::esp::fogmax;

        // Установить максимальную плотность тумана
        float fogDensity = static_cast<float>(gui::settings::esp::fogdensity) / 100.0f;
        fogController->m_fogmaxdensity = fogDensity;

        // Установить цвет тумана
        ImVec4 fogColor = gui::settings::esp::fogcolor;
        fogController->m_fogcolorPrimary = ImColor(fogColor.x, fogColor.y, fogColor.z);
        fogController->m_fogcolorSecondary = ImColor(fogColor.x, fogColor.y, fogColor.z);

        return;  // Останавливаем выполнение функции, так как контроллер уже найден
    }

    // Поиск контроллера тумана среди всех сущностей
    const int maxEntities = entityList->GetHighestEntityIndex();

    for (int i = 0; i < maxEntities; ++i) {
        CEntity* entity = entityList->GetClientEntity(i);

        // Проверяем наличие сущности и её класс на идентификатор тумана
        if (entity && entity->GetClientClass()->classID == 57) {
            fogController = entity;  // Запоминаем контроллер

            // Включить туман
            fogController->m_fogenable = true;

            // Установить начальную и конечную точки тумана
            fogController->m_fogstart = gui::settings::esp::fogmin;
            fogController->m_fogend = gui::settings::esp::fogmax;

            // Установить максимальную плотность тумана
            float fogDensity = static_cast<float>(gui::settings::esp::fogdensity) / 100.0f;
            fogController->m_fogmaxdensity = fogDensity;

            // Установить цвет тумана
            ImVec4 fogColor = gui::settings::esp::fogcolor;
            fogController->m_fogcolorPrimary = ImColor(fogColor.x, fogColor.y, fogColor.z);
            fogController->m_fogcolorSecondary = ImColor(fogColor.x, fogColor.y, fogColor.z);

            break;  // Прерываем цикл, так как контроллер найден
        }
    }
}



#define KEY_W 0x57 // Виртуальный код клавиши W
#define KEY_A 0x41 // Виртуальный код клавиши A
#define KEY_S 0x53 // Виртуальный код клавиши S
#define KEY_D 0x44 // Виртуальный код клавиши D

float normalizeYaw(float yaw) noexcept
{
    if (!std::isfinite(yaw))
        return 0.0f;

    if (yaw >= -180.f && yaw <= 180.f)
        return yaw;

    const float rot = std::round(std::abs(yaw / 360.f));

    yaw = (yaw < 0.f) ? yaw + (360.f * rot) : yaw - (360.f * rot);
    return yaw;
}
static float normalize_float(float angle)
{
    auto revolutions = angle / 360.f;
    if (angle > 180.f || angle < -180.f)
    {
        revolutions = round(abs(revolutions));
        if (angle < 0.f)
            angle = (angle + 360.f * revolutions);
        else
            angle = (angle - 360.f * revolutions);
        return angle;
    }
    return angle;
}
constexpr auto rad2deg(float radians) noexcept { return radians * (180.0f / std::numbers::pi_v<float>); }
float speed, ideal, old_yaw;
float  switch_value = 1.f;
void Misc::Autostrafe(UserCmd* cmd) {
    // Проверяем, что игрок не на земле
    if (globals::localPlayer->m_fFlags & FL_ONGROUND)
        return;

    Vector3 velocity;
    float  delta, abs_delta, velocity_delta, correct;
    
 
    Vector3 m_strafe_angles = hooks::oldangles;

   
    static float cl_sidespeed = 400.f;

    // get networked velocity ( maybe absvelocity better here? ).
    // meh, should be predicted anyway? ill see.
    velocity = globals::localPlayer->m_vecVelocity;

    // get the velocity len2d ( speed ).
    speed = velocity.Length2D();

    // compute the ideal strafe angle for our velocity.
    ideal = (speed > 0.f) ? RAD2DEG(asin(15.f / speed)) : 90.f;

    // some additional sanity.
    std::clamp(ideal, 0.f, 90.f);

    // for changing direction.
    // we want to change strafe direction every call.
    switch_value *= -1.f;

    float wish_dir{ };

    // get our key presses.
    bool holding_w = IsKP(KEY_W);
    bool holding_a = IsKP(KEY_A);
    bool holding_s = IsKP(KEY_S);
    bool holding_d = IsKP(KEY_D);

    // move in the appropriate direction.
    if (holding_w) {
        if (holding_a)
            wish_dir += 45.f;
        else if (holding_d)
            wish_dir -= 45.f;
        else
            wish_dir += 0.f;
    }
    else if (holding_s) {
        if (holding_a)
            wish_dir += 135.f;
        else if (holding_d)
            wish_dir -= 135.f;
        else
            wish_dir += 180.f;
    }
    else if (holding_a)
        wish_dir += 90.f;
    else if (holding_d)
        wish_dir -= 90.f;

    m_strafe_angles.y = normalize_float(m_strafe_angles.y + wish_dir);

    // cancel out any forwardmove values.
    cmd->forwardMove = 0.f;

    // get our viewangle change.
    delta = normalize_float(m_strafe_angles.y - old_yaw);

    // convert to absolute change.
    abs_delta = abs(delta);

    // save old yaw for next call.
    old_yaw = m_strafe_angles.y;

    // set strafe direction based on mouse direction change.
    if (delta > 0.f)
        cmd->sideMove = -cl_sidespeed;
    else if (delta < 0.f)
        cmd->sideMove = cl_sidespeed;

    // we can accelerate more, because we strafed less then needed
    // or we got of track and need to be retracked.
    if (abs_delta <= ideal || abs_delta >= 30.f) {
        // compute angle of the direction we are traveling in.
        Vector3 velocity_angle;
        VectorAngless(velocity, velocity_angle);

        // get the delta between our direction and where we are looking at.
        velocity_delta = normalize_float(m_strafe_angles.y - velocity_angle.y);

        // correct our strafe amongst the path of a circle.
        correct = ideal;

        if (velocity_delta <= correct || speed <= 15.f) {
            // not moving mouse, switch strafe every tick.
            if (-correct <= velocity_delta || speed <= 15.f) {
                m_strafe_angles.y += (ideal * switch_value);
                cmd->sideMove = cl_sidespeed * switch_value;
            }

            else {
                m_strafe_angles.y = velocity_angle.y - correct;
                cmd->sideMove = cl_sidespeed;
            }
        }

        else {
            m_strafe_angles.y = velocity_angle.y + correct;
            cmd->sideMove = -cl_sidespeed;
        }
    }
}

void Misc::SpectatorList() {
    if (!gui::settings::misc::spectatorlist)
        return;
    static int screenHeight = hooks::screenheight;
    static int screenWidth = hooks::screenwidth;
    ImFont* previousFont = ImGui::GetFont();

    constexpr float AnimationSpeed = 500.0f;  // Скорость анимации по X
    constexpr float VerticalAnimationSpeed = 300.0f;  // Скорость анимации по Y
    constexpr float SizeAnimationSpeed = 5.0f;  // Скорость анимации изменения размеров окна
    static std::unordered_map<std::string, float> namePositionsX;  // Для хранения текущих X позиций текстов
    static std::unordered_map<std::string, float> namePositionsY;  // Для хранения текущих Y позиций текстов
    static std::unordered_map<std::string, float> nameAlphas;     // Для хранения альфа-значений текстов
    constexpr float AlphaFadeSpeed = 2.0f;  // Скорость изменения альфа-прозрачности
    std::vector<std::string> spectatorNames;
    ImDrawList* drawList = gui::settings::dlist1;

    std::stringstream ss;
    ss << "Spectator List"; // Заголовок
    std::string headerText = ss.str();

    ImVec2 headerSize = ImGui::CalcTextSize(headerText.c_str());
    static ImVec2 windowPos(20, 100);  // Начальная позиция окна
    static ImVec2 targetWindowSize(300, 200);  // Целевой размер окна (динамический)
    static ImVec2 currentWindowSize = targetWindowSize;  // Текущий размер окна (анимируемый)



    ImGui::PushFont(gui::settings::fonts::spectatorlistfont);
    float deltaTime = ImGui::GetIO().DeltaTime;

    // Заполнение списка зрителей
    if (globals::localPlayer) {
        int localIndex = engine->GetLocalPlayer();
        CEntity* localPlayer = entityList->GetClientEntity(localIndex);

        // Получение зрителей
        for (CEntity* player : hooks::playerList) {
            if (!player || player->IsAlive() || player == globals::localPlayer)
                continue;
            int playerIndex = player->GetIndex();
            if (playerIndex < 1 || playerIndex > engine->GetMaxClients()) {
                continue;  // Пропускаем неправильные индексы
            }

            CEntity* observerTarget = entityList->GetClientEntityFromHandle(player->m_hObserverTarget);
            if (!observerTarget || observerTarget != globals::localPlayer)
                continue;

            player_info_t pInfo;
            engine->GetPlayerInfo(player->GetIndex(), &pInfo);
            spectatorNames.push_back(pInfo.name);
        }

        // Удаление старых имён, которых больше нет в списке зрителей
        for (auto it = namePositionsX.begin(); it != namePositionsX.end();) {
            if (std::find(spectatorNames.begin(), spectatorNames.end(), it->first) == spectatorNames.end()) {
                namePositionsX.erase(it++);  // Удаление из карты X-позиций
            }
            else {
                ++it;
            }
        }

        for (auto it = namePositionsY.begin(); it != namePositionsY.end();) {
            if (std::find(spectatorNames.begin(), spectatorNames.end(), it->first) == spectatorNames.end()) {
                namePositionsY.erase(it++);  // Удаление из карты Y-позиций
            }
            else {
                ++it;
            }
        }

        for (auto it = nameAlphas.begin(); it != nameAlphas.end();) {
            if (std::find(spectatorNames.begin(), spectatorNames.end(), it->first) == spectatorNames.end()) {
                nameAlphas.erase(it++);  // Удаление из карты альфа-значений
            }
            else {
                ++it;
            }
        }

        // Расчёт целевых размеров окна на основе текста
        float totalHeight = headerSize.y + 15;  // Высота заголовка
        float maxWidth = headerSize.x;

        for (const auto& name : spectatorNames) {
            ImVec2 specSize = gui::settings::fonts::spectatorlistfont->CalcTextSizeA(gui::settings::fonts::spectatorlistfontsize, FLT_MAX, 0.0f, name.c_str());
            totalHeight += specSize.y;
            maxWidth = std::max(maxWidth, specSize.x);

            // Если нового имени ещё нет в карте позиций, добавляем его с начальных позиций
            if (namePositionsX.find(name) == namePositionsX.end()) {
                namePositionsX[name] = windowPos.x + currentWindowSize.x; // Справа за окном
                namePositionsY[name] = windowPos.y; // Начальная позиция по Y — сверху
                nameAlphas[name] = 0.0f;  // Начальная прозрачность
            }
        }

        // Обновляем целевой размер окна
        targetWindowSize.x = std::max(maxWidth + 40.0f, 300.0f);  // Обход конфликта с макросом
        targetWindowSize.y = totalHeight + 10;  // Высота окна с учетом всех строк

        // Плавное изменение текущих размеров окна к целевым
        currentWindowSize.x += (targetWindowSize.x - currentWindowSize.x) * SizeAnimationSpeed * deltaTime;
        currentWindowSize.y += (targetWindowSize.y - currentWindowSize.y) * SizeAnimationSpeed * deltaTime;

        ImVec2 rectMin(windowPos);
        ImVec2 rectMax(windowPos.x + currentWindowSize.x, windowPos.y + currentWindowSize.y);

        // Добавление подсветки
        ImColor backlightColor(gui::settings::misc::checkboxColor.x, gui::settings::misc::checkboxColor.y, gui::settings::misc::checkboxColor.z, gui::settings::misc::checkboxColor.w);
        float radius = 12.0f; // Радиус размытия
        float alpha;
        int steps = 10; // Количество шагов для градиента

        for (int i = 0; i < steps; ++i) {
            alpha = (1.0f / 2) * ((float)(steps - i * 2) / steps); // Уменьшение прозрачности с увеличением расстояния
            ImColor color = ImColor(backlightColor.Value.x, backlightColor.Value.y, backlightColor.Value.z, alpha);

            // Смещение для слоев подсветки
            float offset = radius * (float)i / steps;
            ImVec2 innerPos = ImVec2((windowPos.x - offset), (windowPos.y - offset));
            ImVec2 outerPos = ImVec2((windowPos.x + currentWindowSize.x + offset), (windowPos.y + currentWindowSize.y + offset));

            // Рисуем слой подсветки
            drawList->AddRectFilled(innerPos, outerPos, color, radius);
        }

        // Градиентный фон

        drawList->AddRectFilled(rectMin, rectMax, ImColor(0, 0, 0, 255), 12.0f);

        // Отрисовка заголовка с обводкой
        ImVec2 textPos(windowPos.x + 10, windowPos.y + 5);

        drawList->RenderText(gui::settings::fonts::spectatorlistfont, gui::settings::fonts::spectatorlistfontsize, textPos, IM_COL32(255, 255, 255, 255), headerText.c_str(), gui::settings::fonts::spectatorlistfontflag, gui::settings::fonts::spectatorlistfontoffset);


        // Отрисовка списка зрителей
        float basePosY = windowPos.y + headerSize.y + 10; // Начальная Y позиция для первого имени
        for (const auto& name : spectatorNames) {
            ImVec2 specSize = gui::settings::fonts::spectatorlistfont->CalcTextSizeA(gui::settings::fonts::spectatorlistfontsize, FLT_MAX, 0.0f, name.c_str());

            // Линейная интерполяция для перемещения имени влево и вниз
            float& currentX = namePositionsX[name];  // Получаем ссылку на текущую позицию X
            currentX -= AnimationSpeed * deltaTime; // Анимация влево
            currentX = std::max(currentX, windowPos.x + 10); // Ограничиваем движение по X

            float& currentY = namePositionsY[name];  // Получаем ссылку на текущую позицию Y
            currentY += VerticalAnimationSpeed * deltaTime; // Анимация вниз
            currentY = std::min(currentY, basePosY);  // Ограничиваем движение по Y

            // Анимация альфа-прозрачности (увеличиваем до 255)
            float& alpha = nameAlphas[name];
            alpha += AlphaFadeSpeed * deltaTime * 255.0f;
            alpha = std::min(alpha, 255.0f);  // Ограничиваем значение до 255

            // Цвет с анимированным альфа-значением и тенью
            ImU32 textColor = IM_COL32(255, 255, 255, static_cast<int>(alpha));

            drawList->RenderText(gui::settings::fonts::spectatorlistfont, gui::settings::fonts::spectatorlistfontsize, ImVec2(currentX, currentY), textColor, name.c_str(), gui::settings::fonts::spectatorlistfontflag, gui::settings::fonts::spectatorlistfontoffset);
            basePosY += specSize.y + gui::settings::fonts::spectatorlistfontsize + 2;

        }
    }
    ImGui::PopFont();
}

#include <cmath> 


void Misc::MovementCorrection(const Vector3& oang, UserCmd* cmd)
{
    // Получаем текущее направление движения
    Vector3 vMove(cmd->forwardMove, cmd->sideMove, 0.0f);
    float flSpeed = vMove.Length2D();  // Длина движения в плоскости X-Y
    if (flSpeed == 0.f)
        return;

    // Преобразуем вектор движения в углы
    Vector3 qMove;
    VectorAngless(vMove, qMove);

    // Нормализуем углы видения
    float viewYaw = std::fmod(cmd->viewAngles.y + 180.f, 360.f) - 180.f;
    float viewPitch = std::fmod(cmd->viewAngles.x + 180.f, 360.f) - 180.f;

    // Разница между углами текущего вида и исходного (oang)
    float deltaYaw = DEG2RAD(viewYaw - oang.y + qMove.y);

    // Если игрок смотрит выше или ниже, переворачиваем движение
    if (viewPitch > 90.0f || viewPitch < -90.0f) {
        cmd->forwardMove = -std::cos(deltaYaw) * flSpeed;
    }
    else {
        cmd->forwardMove = std::cos(deltaYaw) * flSpeed;
    }

    cmd->sideMove = std::sin(deltaYaw) * flSpeed;
}

#define IN_ATTACK (1 << 0)  // Обычно это первый бит

void Misc::Watermarkk()
{
 static std::string usertext = " | dev";
    static ImVec2 usernamesize;
    usernamesize = ImGui::CalcTextSize(usertext.c_str());
     bool isUsertextSet = false; // Флаг для отслеживания установки usertext

    if (!isUsertextSet )
    {
        player_info_t info;
        engine->GetPlayerInfo(globals::localPlayer->GetIndex(), &info);
        bool isAttacking = (hooks::currentcmd->buttons & IN_ATTACK) != 0;

        // Конвертируем результат в строку
        std::string usertext;
        if (isAttacking) {
            usertext = "IN_ATTACK: pressed";
        }
        else {
            usertext = "IN_ATTACK: not pressed";
        }
     //   usertext = " | " + std::string(info.name);
        usernamesize = ImGui::CalcTextSize(usertext.c_str());
       
    }

    std::string watertext = "hotel white$";

    auto draw_listback  = gui::settings::dlist1;
    auto watermarksize = ImGui::CalcTextSize(watertext.c_str());

    ImVec2 rect_min = ImVec2(15, 15);
    ImVec2 rect_max = ImVec2(rect_min.x + 20 + watermarksize.x + usernamesize.x + 20, rect_min.y + 30);

    // Радиус скругления углов
    float rounding = 20.0f;

    ImU32 firstcolor = IM_COL32(15, 15, 15, 255);
    ImU32 thirdcolor = IM_COL32(5, 5, 5, 255);
    draw_listback->AddRectFilled(rect_min, rect_max, firstcolor, rounding);
    // Определите ширину аутлайна
    float outline_width = 4.0f;
    ImU32 outline_color = IM_COL32(
        static_cast<int>(gui::settings::misc::checkboxColor.x * 255),
        static_cast<int>(gui::settings::misc::checkboxColor.y * 255),
        static_cast<int>(gui::settings::misc::checkboxColor.z * 255),
        static_cast<int>(gui::settings::misc::checkboxColor.w * 255)
    );

    float thickness = 2.0f;
    // Рисуем аутлайн прямоугольника
    draw_listback->AddRect(
        ImVec2(rect_min.x - 2.2, rect_min.y - 2.2), // Левый верхний угол с учетом аутлайна
        ImVec2(rect_max.x + 2.2, rect_max.y + 2.2), // Правый нижний угол с учетом аутлайна
        thirdcolor,
        rounding,
        ImDrawFlags_RoundCornersAll, // Флаги для скругления всех углов
        thickness + 4
    );
    // Рисуем аутлайн прямоугольника с цветом из переменной checkboxColor
    ImVec2 line_start = ImVec2(rect_min.x - 0.2, rect_max.y); // Начало линии (левый край)
    ImVec2 line_end = ImVec2(rect_max.x + 0.2, rect_max.y);   // Конец линии (правый край)

    // Рисуем нижнюю линию с заданным цветом и толщиной
    draw_listback->AddLine(
        line_start,
        line_end,
        outline_color,
        thickness
    );



    
    float centerY = (rect_min.y + rect_max.y) * 0.5f;

    // Вычислите половину высоты текста
    float halfTextHeight = watermarksize.y * 0.5f;

    // Вычтите половину высоты текста из вертикальной середины
    float textY = centerY - halfTextHeight;

    draw_listback->AddText(ImVec2(rect_min.x + 20, textY), IM_COL32(255, 255, 255, 255), watertext.c_str());

    // Рисуем usertext с учетом смещения на ширину watertext
    draw_listback->AddText(ImVec2(rect_min.x + 20 + watermarksize.x, textY), IM_COL32(255, 255, 255, 255), usertext.c_str());
    
  
}

void Misc::BombInfo()
{
    static int screenHeight = hooks::screenheight;
    static int screenWidth = hooks::screenwidth;
    if (!globals::C4 || globals::C4 != (CC4*)entityList->GetEntityFromClass(ClassID::CC4))
        globals::C4 = (CC4*)entityList->GetEntityFromClass(ClassID::CC4);

    if (!globals::plantedC4 || globals::plantedC4 != (CPlantedC4*)entityList->GetEntityFromClass(ClassID::CPlantedC4))
        globals::plantedC4 = (CPlantedC4*)entityList->GetEntityFromClass(ClassID::CPlantedC4);

    if (!globals::plantedC4 || !globals::plantedC4->m_bBombTicking)
        return;

    auto draw_listback = gui::settings::dlist1;

    ImVec2 windowSize = ImGui::GetIO().DisplaySize;

    float timeRemaining = globals::plantedC4->m_flC4Blow - globalVars->currentTime;
    std::string bombtimer = std::to_string(static_cast<int>(timeRemaining)) + "s";

    ImVec2 timersize = ImGui::CalcTextSize(bombtimer.c_str());

 

    std::string explosionText = "] explosion: ";
    ImVec2 explosionSize = ImGui::CalcTextSize(explosionText.c_str());

    float totalTextWidth = explosionSize.x + timersize.x;

    float rectWidth = totalTextWidth + 10;

    ImVec2 rect_min = ImVec2((screenWidth / 2) - (rectWidth / 2) - 5, 200);
    ImVec2 rect_max = ImVec2(rect_min.x + rectWidth + 10, 230);

    float rounding = 20; // Получаем радиус скругления углов из стиля ImGui

    ImU32 firstcolor = IM_COL32(15, 15, 15, 255);
    draw_listback->AddRectFilled(rect_min, rect_max, firstcolor, rounding);

    ImU32 outline_color = IM_COL32(
        static_cast<int>(gui::settings::misc::checkboxColor.x * 255),
        static_cast<int>(gui::settings::misc::checkboxColor.y * 255),
        static_cast<int>(gui::settings::misc::checkboxColor.z * 255),
        static_cast<int>(gui::settings::misc::checkboxColor.w * 255)
    );
    ImU32 thirdcolor = IM_COL32(5, 5, 5, 255);
    float thickness = 2.0f;
   

  
    draw_listback->AddRect(
        ImVec2(rect_min.x - 2.2, rect_min.y - 2.2),
        ImVec2(rect_max.x + 2.2, rect_max.y + 2.2),
        thirdcolor,
        rounding,
        ImDrawFlags_RoundCornersAll,
        thickness + 2
    );
    draw_listback->AddRect(
        ImVec2(rect_min.x - 0.2, rect_min.y - 0.2),
        ImVec2(rect_max.x + 0.2, rect_max.y + 0.2),
        outline_color,
        rounding,
        ImDrawFlags_RoundCornersAll,
        thickness
    );

    ImVec2 explosionTextPos = ImVec2(rect_min.x + 5 + 5, rect_min.y + (rect_max.y - rect_min.y) / 2 - explosionSize.y / 2);
    draw_listback->AddText(explosionTextPos, IM_COL32(255, 255, 255, 255), explosionText.c_str());

    ImVec2 bombtimerPos = ImVec2(explosionTextPos.x + explosionSize.x, explosionTextPos.y);
    draw_listback->AddText(bombtimerPos, IM_COL32(255, 255, 255, 255), bombtimer.c_str());
}


ImVec4 interpolateColor(float value, float max_value, ImVec4 color1, ImVec4 color2) {
    float ratio = value / max_value;

    float r = color1.x + ratio * (color2.x - color1.x);
    float g = color1.y + ratio * (color2.y - color1.y);
    float b = color1.z + ratio * (color2.z - color1.z);
    float a = color1.w + ratio * (color2.w - color1.w);

    return ImVec4(r, g, b, a);
}
using KvAllocFn = void* (__cdecl*)(int);
using KvInitNameFn = void* (__thiscall*)(void*, const char*);
using KvSetStringFn = void* (__thiscall*)(void*, const char*, const char*);


void Misc::SetClanTag(const char* tag)
{
    static auto enginedll = (DWORD)GetModuleHandle("engine");
    static auto kv_alloc = (void* (_cdecl*)(int))(enginedll + 0x2A1400);
    static auto kv_init_name = (void* (__thiscall*)(void*, const char*))(enginedll + 0x2A13A0);
    static auto kv_setstring = (void* (__thiscall*)(void*, const char*, const char*))(enginedll + 0x2A4630);

    auto v9 = kv_alloc(32);
    if (v9)
    {
        auto v10 = kv_init_name(v9, "ClanTagChanged");
        if (v10)
        {
            kv_setstring(v10, "tag", tag);
            engine->ServerCmdKeyValues(v10);
        }
    }
}
int iLastTime;
void Misc::ct() {
    static float flNextTimeUpdate = 0;
    static int iLastTime = 0;
    //SetClanTag("hotel white$");
    float flCurTime = globals::localPlayer->m_nTickBase * globalVars->intervalPerTick;

    if (int(globalVars->currentTime * 2) != iLastTime) {
        if (flNextTimeUpdate <= flCurTime || flNextTimeUpdate - flCurTime > 1.f) {
            switch (int(globalVars->currentTime * 1.2) % 13) { //pomenyal vrode okэй
            case 0: SetClanTag("h"); break;
            case 1: SetClanTag("ho"); break;
            case 2: SetClanTag("hot"); break;
            case 3: SetClanTag("hote"); break;
            case 4: SetClanTag("hotel"); break;
            case 5: SetClanTag("hotel "); break;
            case 6: SetClanTag("hotel w"); break;
            case 7: SetClanTag("hotel wh"); break;
            case 8: SetClanTag("hotel whi"); break;
            case 9: SetClanTag("hotel whit"); break;
            case 10: SetClanTag("hotel white"); break;
            case 11: SetClanTag("hotel white$"); break;
            case 12: SetClanTag("hotel white"); break;
            case 13: SetClanTag("hotel whit"); break;
            case 14: SetClanTag("hotel whi"); break;
            case 15: SetClanTag("hotel wh"); break;
            case 16: SetClanTag("hotel w"); break;
            case 17: SetClanTag("hotel "); break;
            case 18: SetClanTag("hotel"); break;
            case 19: SetClanTag("hote"); break;
            case 20: SetClanTag("hot"); break;
            case 21: SetClanTag("ho"); break;
            case 22: SetClanTag("h"); break;

            }
            flNextTimeUpdate = flCurTime + 1.0f; // koro4e obnova pod last
        }
        iLastTime = int(globalVars->currentTime * 2);
    }
}
int maxcolor;
int staminamaxcolor;
inline int custom_min(int a, int b) {
    return (a < b) ? a : b;
}

// Функция для линейной интерполяции с сглаживанием
float smoothLerp(float current, float target, float smoothing) {
    return current + (target - current) * smoothing;
}

ImVec4 interpolateColor(int value, int max_value, ImVec4 color1, ImVec4 color2, float smoothing = 0.05f) {
    static float t = 0.0f; // Добавляем статическое значение t для плавного изменения
    float target_t = custom_min(static_cast<float>(value) / static_cast<float>(max_value), 1.0f);

    // Плавно изменяем значение t с помощью функции smoothLerp
    t = smoothLerp(t, target_t, smoothing);

    return ImVec4(
        color1.x + t * (color2.x - color1.x),
        color1.y + t * (color2.y - color1.y),
        color1.z + t * (color2.z - color1.z),
        color1.w + t * (color2.w - color1.w)
    );
}
ImVec4 interpolateColor1(int value, int max_value, ImVec4 color1, ImVec4 color2, float smoothing = 0.05f) {
    static float t = 0.0f; // Добавляем статическое значение t для плавного изменения
    float target_t = custom_min(static_cast<float>(value) / static_cast<float>(max_value), 1.0f);

    // Плавно изменяем значение t с помощью функции smoothLerp
    t = smoothLerp(t, target_t, smoothing);

    return ImVec4(
        color1.x + t * (color2.x - color1.x),
        color1.y + t * (color2.y - color1.y),
        color1.z + t * (color2.z - color1.z),
        color1.w + t * (color2.w - color1.w)
    );
}



void Misc::Velocity()
{
    if (!globalVars) {
        // Инициализация объекта globalVars, если он не был инициализирован ранее
        globalVars = new IGlobalVars();
    }
    static int velofontsize = 0;
    static bool render = true;
   

    ImFont* previousFont = ImGui::GetFont();
   
        ImGui::PushFont(gui::settings::fonts::velocityfont);
       
    long currentTime = static_cast<long>(globalVars->realTime * 1000);
    static int screenHeight = hooks::screenheight;
    static int screenWidth = hooks::screenwidth;
    if (globals::localPlayer && render)
    {
         ImVec4 color1 = gui::settings::misc::velocolor;
         ImVec4 color2 = gui::settings::misc::velocolor2;
         ImVec4 stcolor1 = gui::settings::misc::staminacolor;
         ImVec4 stcolor2 = gui::settings::misc::staminacolor2;
        ImDrawList* draw_list = gui::settings::dlist1;
        float max_value = 300.0f;

        static int lastUpdateTime = currentTime;
        const int updateInterval = 10;
        static int kamiposY = 0;
        static int kamialpha = 255;
        int prePosY = (gui::settings::misc::velocityposition);

        if (gui::settings::misc::velocity)
        {
            Vector3 velocity = globals::localPlayer->m_vecVelocity;
            float speed = velocity.Length2D();
            int roundspeed = std::round(speed);
            static int lastVelocity = roundspeed;
            static bool showLastVelocity = false;

            if (!(globals::localPlayer->m_fFlags & FL_ONGROUND) && !showLastVelocity) {
                showLastVelocity = true;
                lastVelocity = roundspeed;
            }
            else if ((globals::localPlayer->m_fFlags & FL_ONGROUND) && showLastVelocity) {
                if (gui::settings::misc::pretype == 2) {
                    kamiposY = prePosY;
                    kamialpha = 255;
                }
                showLastVelocity = false;
            }

            int alpha = custom_min(roundspeed, 255);

            if (gui::settings::misc::pretype == 0) {
                std::string speedString = std::to_string(roundspeed);
                maxcolor = custom_min(roundspeed, gui::settings::misc::maxcolor);
                ImVec4 interpolatedColor = interpolateColor(maxcolor, gui::settings::misc::maxcolor, color1, color2);
                ImU32 resultColor = ImColor(interpolatedColor.x, interpolatedColor.y, interpolatedColor.z, alpha / 255.0f);
                ImVec2 textSize = gui::settings::fonts::velocityfont->CalcTextSizeA(gui::settings::fonts::velocityfontsize, FLT_MAX, 0.0f, speedString.c_str());
                ImVec2 pos((screenWidth - textSize.x) / 2, gui::settings::misc::velocityposition);
                draw_list->RenderText(gui::settings::fonts::velocityfont, gui::settings::fonts::velocityfontsize, pos, resultColor, speedString.c_str(), gui::settings::fonts::velocityfontflag, gui::settings::fonts::velocityfontoffset);
            }
            else if (gui::settings::misc::pretype == 1) {
                std::string speedString = std::to_string(roundspeed);
                ImVec2 textSize = ImGui::CalcTextSize(speedString.c_str());
                ImVec2 pos((screenWidth - textSize.x) / 2, gui::settings::misc::velocityposition);

                if (showLastVelocity) {
                    std::string preSpeedString = "(" + std::to_string(lastVelocity) + ")";
                    ImVec2 preTextSize = ImGui::CalcTextSize(preSpeedString.c_str());
                    ImVec2 prePos((pos.x + 2) / 2, gui::settings::misc::velocityposition);
                    ImU32 preColor = ImColor(gui::settings::misc::precolor.x, gui::settings::misc::precolor.y, gui::settings::misc::precolor.z, alpha / 255.0f);
                    draw_list->AddText(prePos, preColor, preSpeedString.c_str());
                }

                ImU32 resultColor = ImColor(gui::settings::misc::velocolor.x, gui::settings::misc::velocolor.y, gui::settings::misc::velocolor.z, alpha / 255.0f);
                draw_list->AddText(pos, resultColor, speedString.c_str());
            }
            else if (gui::settings::misc::pretype == 2) {
                std::string speedString = std::to_string(roundspeed);
                ImVec2 textSize = ImGui::CalcTextSize(speedString.c_str());
                ImVec2 pos((screenWidth - textSize.x) / 2, gui::settings::misc::velocityposition);

                if (showLastVelocity) {
                    std::string preSpeedString = std::to_string(lastVelocity);
                    ImVec2 preTextSize = ImGui::CalcTextSize(preSpeedString.c_str());
                    ImVec2 prePos((screenWidth - preTextSize.x) / 2, kamiposY);
                    ImU32 preColor = ImColor(gui::settings::misc::precolor.x, gui::settings::misc::precolor.y, gui::settings::misc::precolor.z, kamialpha / 255.0f);

                    if (globals::localPlayer->m_fFlags & FL_ONGROUND) {
                        kamialpha = 255;
                        kamiposY = prePosY;
                    }
                    else if (currentTime - lastUpdateTime >= updateInterval) {
                        kamiposY -= 1;
                        kamialpha -= 3;
                        lastUpdateTime = currentTime;
                    }
                    DrawDropShadowText(gui::settings::fonts::velocityfont, gui::settings::misc::velosize, prePos, preColor, preSpeedString.c_str());

                }
                ImU32 resultColor = ImColor(gui::settings::misc::velocolor.x, gui::settings::misc::velocolor.y, gui::settings::misc::velocolor.z, alpha / 255.0f);
             DrawDropShadowText(gui::settings::fonts::velocityfont, gui::settings::misc::velosize, pos, resultColor, speedString.c_str());
            }
        }

        if (gui::settings::misc::stamina) {
            float stamina = globals::localPlayer->m_flStamina;
            int roundstamina = std::round(stamina);
            std::string staminaString = std::to_string(stamina);
            int staminaalpha = custom_min(roundstamina * 13, 255);
            int maxnums = (stamina >= 10) ? 5 : 3;
            std::string trimmedStaminaString = staminaString.substr(0, maxnums);

            ImVec2 sttextSize = gui::settings::fonts::staminafont->CalcTextSizeA(gui::settings::fonts::staminafontsize, FLT_MAX, 0.0f, trimmedStaminaString.c_str());
            ImVec2 staminapos((screenWidth - sttextSize.x) / 2, gui::settings::misc::staminaposition);
            staminamaxcolor = custom_min(roundstamina, gui::settings::misc::staminamaxcolor);
            ImVec4 staminainterpolatedColor = interpolateColor1(staminamaxcolor, gui::settings::misc::staminamaxcolor, stcolor1, stcolor2);
            ImU32 staminaresultColor = ImColor(staminainterpolatedColor.x, staminainterpolatedColor.y, staminainterpolatedColor.z, staminaalpha / 255.0f);
            
            draw_list->RenderText(gui::settings::fonts::staminafont, gui::settings::fonts::staminafontsize, ImVec2(staminapos.x, staminapos.y), staminaresultColor, trimmedStaminaString.c_str(), gui::settings::fonts::staminafontflag, gui::settings::fonts::staminafontoffset);
       
        }
    }
    ImGui::PopFont();
}
