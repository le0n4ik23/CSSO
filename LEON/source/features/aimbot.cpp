#include "aimbot.h"
#include "../interfaces/interfaces.h"
#include "../util/usercmd.h"
#include "../globals.h"
#include "../hooks.h"
#include "../util/enums.h"
#include "../gui.h"
#include "../interfaces/IClientEntityList.h"
#undef max
#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>
#define mask_shot (0x1 | 0x4000 | 0x2000000 | 0x2 | 0x4000000 | 0x40000000)

static bool aa = false;
bool aimbotting = false;

Vector3 VelocityExtrapolate(CEntity* player, const Vector3& aimPos) {
    return aimPos + (player->m_vecVelocity * globalVars->intervalPerTick);
}

float deg2rad(float deg) {
    return deg * (3.14159265358979323846f / 180.0f);
}

void angle_vectors(const Vector3& angles, Vector3& forward) {
    float sp = sin(deg2rad(angles.x));
    float cp = cos(deg2rad(angles.x));
    float sy = sin(deg2rad(angles.y));
    float cy = cos(deg2rad(angles.y));

    forward.x = cp * cy;
    forward.y = cp * sy;
    forward.z = -sp;
}



void Aimbot::Triggerbot(UserCmd* cmd) noexcept {
    Vector3 eyePos = globals::localPlayer->GetEyePos();
    Vector3 aimPunch = globals::localPlayer->m_aimPunchAngle;
    CBaseCombatWeapon* weapon = globals::localPlayer->GetActiveWeapon();
    const Vector3 dst = eyePos + (Vector3{ cmd->viewAngles + (aimPunch * 2) }.ToVector()) * 8000.f;

    trace_t trace;
    CTraceFilter filter(globals::localPlayer);
    Ray_t ray;
    ray.Init(eyePos, dst);
    engineTrace->TraceRay(ray, 0x4600400B, &filter, &trace);

    if (!trace.entity || !trace.entity->IsPlayer() || trace.entity->IsDormant() || !trace.entity->IsAlive() || trace.entity->m_bImmunity)
        return;

    if (trace.entity->m_iTeamNum == globals::localPlayer->m_iTeamNum || trace.entity->m_iTeamNum == 0)
        return;

    Vector3 vel = globals::localPlayer->m_vecVelocity;
    
        Aimbot::AutoFire(cmd, weapon);
    
}
float Aimbot::GetMaxFov(CBaseCombatWeapon* weapon) {
    if (weapon->IsPistol()) {
        return gui::settings::aimbot::pistolfov * 2;
    }
    if (weapon->IsRifle()) {
        return gui::settings::aimbot::riflefov * 2;
    }
    if (weapon->IsSniper()) {
        return gui::settings::aimbot::sniperfov * 2;
    }
    return 0.0f; // Значение по умолчанию, если оружие не соответствует ни одному из типов
}
float Aimbot::GetSmoothFactor(CBaseCombatWeapon* weapon)  {
    if (weapon->IsPistol()) {
        return gui::settings::aimbot::pistolsmooth / 100.f;
    }
    if (weapon->IsRifle()) {
        return gui::settings::aimbot::riflesmooth / 100.f;
    }
    if (weapon->IsSniper()) {
        return gui::settings::aimbot::snipersmooth / 100.f;
    }
    return 1.0f; // Значение по умолчанию
}
float Aimbot::GetWeaponRcs(CBaseCombatWeapon* weapon) {
    if (weapon->IsPistol()) {
        return gui::settings::aimbot::pistolrcs;
    }
    if (weapon->IsRifle()) {
        return gui::settings::aimbot::riflercs;
    }
    if (weapon->IsSniper()) {
        return gui::settings::aimbot::sniperrcs;
    }
    return 0.0f; // Значение по умолчанию, если оружие не соответствует ни одному из типов
}
std::map<int, int> Aimbot::GetHitboxMapping(CBaseCombatWeapon* weapon) {
    std::map<int, int> hitboxMapping;

    if (weapon->IsPistol()) {
        // Сопоставление для пистолетов
        for (int hitbox : gui::settings::aimbot::pistolhitboxes) {
            switch (hitbox) {
            case 0: hitboxMapping[hitbox] = 8; break;   // Head
            case 1: hitboxMapping[hitbox] = 6; break;   // Body
            case 2: hitboxMapping[hitbox] = 39; break;  // Right Hand
            case 3: hitboxMapping[hitbox] = 13; break;  // Left Hand
            case 4: hitboxMapping[hitbox] = 73; break;  // Right Leg
            case 5: hitboxMapping[hitbox] = 66; break;  // Left Leg
            case 6: hitboxMapping[hitbox] = 74; break;  // Right Foot
            case 7: hitboxMapping[hitbox] = 67; break;  // Left Foot
            default: continue;  // Skip invalid indices
            }
        }
    }
    else if (weapon->IsRifle()) {
        // Сопоставление для винтовок
        for (int hitbox : gui::settings::aimbot::riflehitboxes) {
            switch (hitbox) {
            case 0: hitboxMapping[hitbox] = 8; break;   // Head
            case 1: hitboxMapping[hitbox] = 6; break;   // Body
            case 2: hitboxMapping[hitbox] = 39; break;  // Right Hand
            case 3: hitboxMapping[hitbox] = 13; break;  // Left Hand
            case 4: hitboxMapping[hitbox] = 73; break;  // Right Leg
            case 5: hitboxMapping[hitbox] = 66; break;  // Left Leg
            case 6: hitboxMapping[hitbox] = 74; break;  // Right Foot
            case 7: hitboxMapping[hitbox] = 67; break;  // Left Foot
            default: continue;  // Skip invalid indices
            }
        }
    }
    else if (weapon->IsSniper()) {
        // Сопоставление для снайперов
        for (int hitbox : gui::settings::aimbot::sniperhitboxes) {
            switch (hitbox) {
            case 0: hitboxMapping[hitbox] = 8; break;   // Head
            case 1: hitboxMapping[hitbox] = 6; break;   // Body
            case 2: hitboxMapping[hitbox] = 39; break;  // Right Hand
            case 3: hitboxMapping[hitbox] = 13; break;  // Left Hand
            case 4: hitboxMapping[hitbox] = 73; break;  // Right Leg
            case 5: hitboxMapping[hitbox] = 66; break;  // Left Leg
            case 6: hitboxMapping[hitbox] = 74; break;  // Right Foot
            case 7: hitboxMapping[hitbox] = 67; break;  // Left Foot
            default: continue;  // Skip invalid indices
            }
        }
    }

    return hitboxMapping;
}


Vector3 bestAngle1;
static CEntity* player1;
bool toggle = false;
int bestHitboxIndex;
void Aimbot::Aim(UserCmd* cmd, float maxFov, float smoothFactor, const std::map<int, int>& hitboxMapping) noexcept {
    aimbotting = false;
    Vector3 bestAngle{};
    float bestFov = std::numeric_limits<float>::max();
    Vector3 aimPunch;
    Vector3 enemyAngle;
    CBaseCombatWeapon* weapon = globals::localPlayer->GetActiveWeapon();
    Vector3 velocity = globals::localPlayer->m_vecVelocity;

    velocity.Normalize();
    float speed = velocity.Length2D();

    CEntity* bestPlayer = nullptr;
    aimPunch = globals::localPlayer->m_aimPunchAngle;
    float weaponRcs = GetWeaponRcs(weapon);
    Vector3 eyePos = globals::localPlayer->GetEyePos();

    for (CEntity* player : hooks::playerList) {
        if (!player || !player->IsAlive() || player->IsDormant() || player->m_bImmunity)
            continue;
        if (player->m_iTeamNum == globals::localPlayer->m_iTeamNum && !gui::settings::aimbot::targetteammates)
            continue;

        Vector3 hitboxPos;
        if (IsKT(gui::settings::binds::forcebaim) && gui::settings::aimbot::forcebaim) {
             hitboxPos = player->GetBonePos(6);
           
        }
        else {
            for (const auto& [hitbox, boneIndex] : hitboxMapping) {
                 hitboxPos = player->GetBonePos(boneIndex);
             
            }
        }

        Vector3 bodyAngle = (hitboxPos - eyePos).ToAngle() - cmd->viewAngles;
        enemyAngle = bodyAngle - (aimPunch * (1.2f + (weaponRcs / 100.f)));  // Enhanced compensation

        const float fov = std::hypot(enemyAngle.x, enemyAngle.y);
        if (fov < bestFov && fov <= maxFov) {
            bestFov = fov;
            bestAngle = enemyAngle;
            bestPlayer = player;
        }
    }

    if (bestPlayer) {
        player1 = bestPlayer;

        // Check visibility for specific hitboxes
        if (bestPlayer->IsVisible(6) || bestPlayer->IsVisible(8) || bestPlayer->IsVisible(13) ||
            bestPlayer->IsVisible(39) || bestPlayer->IsVisible(66) || bestPlayer->IsVisible(67) ||
            bestPlayer->IsVisible(73) || bestPlayer->IsVisible(74)) {

            if ((globals::localPlayer->GetActiveWeapon()->m_flNextPrimaryAttack - 0.50 <= globalVars->currentTime)) {
                AutoStop(cmd, weapon);
            }

            // Check if smoothFactor is 0 for instant aiming
            if (smoothFactor > 0) {
                float dynamicSmooth = std::max(smoothFactor * (bestFov / maxFov), 0.1f); // Dynamic smoothing factor
                bestAngle = bestAngle * (1.0f - dynamicSmooth);
            }

            if (weapon->m_flNextPrimaryAttack - 0.1 <= globalVars->currentTime) {
                if (gui::settings::aimbot::autofire) {
                    cmd->viewAngles = cmd->viewAngles + bestAngle;
                    aimbotting = true;
                    if ((speed <= weapon->GetIdealAccurateSpeed())) {
                        AutoFire(cmd, weapon);
                    }
                        
                    
                }
                else if (IsKP(gui::settings::binds::aimKey)) {
                    cmd->viewAngles = cmd->viewAngles + bestAngle;
                    aimbotting = true;
                    Triggerbot(cmd);
                }
            }
        }
    }
}

void Aimbot::AutoFire(UserCmd* cmd, CBaseCombatWeapon* weapon) noexcept {
    bool pistol = weapon->IsPistol();
    bool sniper = weapon->IsSniper();

    // Если это пистолет
    if (pistol) {
        // Пистолет стреляет только один раз на нажатие кнопки
        static bool lastShot = false; // Следим за предыдущим выстрелом

        // Если мы не стреляли на предыдущем кадре
        if (!lastShot) {
            cmd->buttons |= IN_ATTACK;  // Стреляем
            lastShot = true;  // Помечаем, что мы выстрелили
        }
        else {
            // Если уже стреляли в предыдущем кадре, отпускаем кнопку атаки
            cmd->buttons &= ~IN_ATTACK;
            lastShot = false;
        }
        return;
    }

    // Для снайперских винтовок
    if (sniper) {
        if (!globals::localPlayer->m_bIsScoped) {
            cmd->buttons |= IN_ZOOM; // Если не прицелились, прицеливаемся
        }
        cmd->buttons |= IN_ATTACK; // Стреляем
        return;
    }

    // Обычное оружие стреляет автоматически
    cmd->buttons |= IN_ATTACK;
}

static Vector3 quickpeekstartpos;
float distance(const Vector3& pos1, const Vector3& pos2) {
    return std::sqrt(
        std::pow(pos1.x - pos2.x, 2) +
        std::pow(pos1.y - pos2.y, 2) +
        std::pow(pos1.z - pos2.z, 2)
    );
}
void gotoStart(UserCmd* cmd) {
    auto localPlayer = globals::localPlayer;
    if (!localPlayer || !localPlayer->IsAlive()) return;
    Vector3 playerLoc = localPlayer->m_vecOrigin;

    float yaw = cmd->viewAngles.y;
    Vector3 VecForward = playerLoc - quickpeekstartpos;

    Vector3 translatedVelocity = Vector3{
        (float)(VecForward.x * cos(yaw / 180 * (float)M_PI) + VecForward.y * sin(yaw / 180 * (float)M_PI)),
        (float)(VecForward.y * cos(yaw / 180 * (float)M_PI) - VecForward.x * sin(yaw / 180 * (float)M_PI)),
        VecForward.z
    };
    if (distance(playerLoc, quickpeekstartpos) >= 10.0f)
    {
        gui::settings::aimbot::indicatorAutoPeek = true;
        cmd->forwardMove = -translatedVelocity.x * 20.f;
        cmd->sideMove = translatedVelocity.y * 20.f;
        
    }
    else
    {
        gui::settings::aimbot::indicatorAutoPeek = false;
        gui::settings::aimbot::detectAutoPeek = false;
    }
}
void Aimbot::AutoPeek(UserCmd* cmd) noexcept {
    auto localPlayer = globals::localPlayer;
    if (!localPlayer || !localPlayer->IsAlive()) return;
    if (IsKP(gui::settings::binds::autopeekbutton)) {
        if (quickpeekstartpos.x == 0 || quickpeekstartpos.y == 0 || quickpeekstartpos.z == 0) {
            quickpeekstartpos = localPlayer->m_vecOrigin;
        }
        else {
            if (cmd->buttons & IN_ATTACK) {
                gui::settings::aimbot::detectAutoPeek = true;
               

            }
            if (gui::settings::aimbot::detectAutoPeek)
            {
               
                gotoStart(cmd);
            }
        }
    }
    else {
        gui::settings::aimbot::detectAutoPeek = false;
        quickpeekstartpos = Vector3{ 0, 0, 0 };
    }
}

void Aimbot::AutoStop(UserCmd* cmd, CBaseCombatWeapon* weapon) noexcept {
    // Проверяем, включена ли опция автологики
    if (!gui::settings::aimbot::adaptiveautostop)
        return;

    // Получаем максимальную скорость для точной стрельбы
    float maxSpeed = weapon->GetIdealAccurateSpeed() - 10;

    // Получаем текущую скорость игрока
    Vector3 velocity = globals::localPlayer->m_vecVelocity;

    // Проверка скорости перед использованием
    float speed = velocity.Length2D();
    if (speed <= 0.1f) // Уже стоим, не нужно останавливаться
        return;

    // Нормализация скорости
    velocity.Normalize();

    // Рассчитываем угол текущего движения
    Vector3 veloangle = velocity.ToAngle();

    // Рассчитываем угол между направлением камеры и движением
    Vector3 direction;
    direction.y = cmd->viewAngles.y - veloangle.y;

    // Перевод угла в вектор движения
    Vector3 forward;
    angle_vectors(direction, forward);

    // Рассчитываем новый вектор движения с учетом уменьшенной скорости
    Vector3 adjustedVelocity = forward * -speed;

    // Проверка на готовность оружия к следующему выстрелу
    if (weapon->m_flNextPrimaryAttack - 0.15f <= globalVars->currentTime) {
        if (speed > maxSpeed) {
            // Останавливаем игрока, уменьшив движения вперед и вбок
            cmd->forwardMove = adjustedVelocity.x * 2;
            cmd->sideMove = adjustedVelocity.y * 2;
        }
    }
}



int Aimbot::GetBestBone(CEntity* ent) {
    return 0;
}
static bool leftToggle = false;
static bool backToggle = false;
static bool rightToggle = false;
void CalcAngle(const Vector3& src, const Vector3& dst, Vector3& angles) {
    Vector3 delta = dst - src; // Вычисляем разницу между позициями
    float hypotenuse = std::sqrt(delta.x * delta.x + delta.y * delta.y); // Вычисляем гипотенузу

    // Углы
    angles.x = std::atan2(delta.z, hypotenuse) * (180.0f / M_PI); // Угол по оси X
    angles.y = std::atan2(delta.y, delta.x) * (180.0f / M_PI); // Угол по оси Y
    angles.z = 0.0f; // Угол по оси Z (обычно не используется для аимбота)

    // Нормализация углов
    if (angles.y > 180.0f) angles.y -= 360.0f;
    if (angles.y < -180.0f) angles.y += 360.0f;
    if (angles.x > 89.0f) angles.x = 89.0f;
    if (angles.x < -89.0f) angles.x = -89.0f;
}

float NormalizeYaw(float yaw)
{
    // Приведение угла к диапазону [-180, 180]
    while (yaw > 180.0f) yaw -= 360.0f;
    while (yaw < -180.0f) yaw += 360.0f;
    return yaw;
}
float randomJitter;
void Aimbot::AA(UserCmd* cmd, bool& bSendPacket) noexcept {
    if (!globals::localPlayer || !globals::localPlayer->IsAlive())
        return;

    static Vector3 LastRealAngle(0, 0, 0);
    static Vector3 baseAngle(0, 0, 0);
   
    // Активируем анти-аим только если мы не аимботим
    if (!aimbotting) {
        aa = true;
    }
    else {
        aa = false;
    }

    // Логика переключения нажатия
    if (IsKT(gui::settings::binds::leftmanual)) {  // Влево (Z)
        leftToggle = !leftToggle;
        rightToggle = false;
    }
   
    else if (IsKT(gui::settings::binds::rightmanual)) {  // Вправо (C)
        rightToggle = !rightToggle;
        leftToggle = false;
       
    }
    else if (IsKT(gui::settings::binds::backmanual)) {  // Вправо (C)
        rightToggle = false;
        leftToggle = false;

    }
  
    if (aa) {
        if (leftToggle) {
            baseAngle.y = hooks::realangles.y - 90.f;
        }
        else if (rightToggle) {
            baseAngle.y = hooks::realangles.y + 90.f;
        }
        else {
            if (gui::settings::aimbot::attargets) {
                CEntity* closestPlayer = nullptr;
                float closestDistance = std::numeric_limits<float>::max(); // Инициализируем максимальным значением

                // Проходим по списку игроков
                for (CEntity* player : hooks::playerList) {
                    if (player != nullptr && player->IsAlive()) { // Проверяем, что игрок жив
                        float distance = (globals::localPlayer->m_vecOrigin - player->m_vecOrigin).Length2D();
                        if (distance < closestDistance) { // MAX_DISTANCE - максимальное расстояние
                            closestDistance = distance;
                            closestPlayer = player;
                        }
                    }
                }

                // Если нашли ближайшего игрока, вычисляем угол
                if (closestPlayer != nullptr) {
                    Vector3 at;
                    CalcAngle(globals::localPlayer->m_vecOrigin, closestPlayer->m_vecOrigin, at);
                    baseAngle.y = at.y + 180.f;
                    if (baseAngle.y > 180.f) {
                        baseAngle.y -= 360.f; // Нормализация углов
                    }
                }
                else {
                    baseAngle.y = hooks::realangles.y + 179.f;
                }
            }
            else {
                baseAngle.y = hooks::realangles.y + 179.f;
            }
        }

        if (gui::settings::aimbot::jitter) {
            randomJitter = static_cast<float>(rand() % 41 - 20);
        }

        cmd->viewAngles.y = baseAngle.y + randomJitter;
        cmd->viewAngles.x = 89.f;
    }

   

  
}

void Aimbot::ManualIndicator() {
    static int screenHeight = hooks::screenheight;
    static int screenWidth = hooks::screenwidth;
    ImVec2 screenCenter = ImVec2(screenWidth / 2.0f, screenHeight / 2.0f);
    ImDrawList* draw_list = gui::settings::dlist1;

    float triangleHeight = 25.0f;
    float triangleWidth = triangleHeight * 1.15f;
    float offset = 30.0f;
    float jitterHeight = 25.0f; // Высота прямоугольника меньше высоты треугольника
    float jitterWidth = 5.0f; // Ширина прямоугольника

    ImU32 shadowColor = IM_COL32(0, 0, 0, 150);
    float outlineThickness = 2.0f;
    ImVec2 leftp1 = ImVec2(screenCenter.x - triangleWidth - offset, screenCenter.y);
    ImVec2 leftp2 = ImVec2(screenCenter.x - offset, screenCenter.y - triangleHeight / 2.0f);
    ImVec2 leftp3 = ImVec2(screenCenter.x - offset, screenCenter.y + triangleHeight / 2.0f);
    ImVec2 rightp1 = ImVec2(screenCenter.x + triangleWidth + offset, screenCenter.y);
    ImVec2 rightp2 = ImVec2(screenCenter.x + offset, screenCenter.y - triangleHeight / 2.0f);
    ImVec2 rightp3 = ImVec2(screenCenter.x + offset, screenCenter.y + triangleHeight / 2.0f);
    draw_list->AddTriangleFilled(leftp1, leftp2, leftp3, shadowColor);
    draw_list->AddTriangleFilled(rightp1, rightp2, rightp3, shadowColor);
    // Левый треугольник
    if (leftToggle) {
        draw_list->AddTriangleFilled(leftp1, leftp2, leftp3, IM_COL32(
            static_cast<ImU32>(gui::settings::misc::checkboxColor.x * 255),
            static_cast<ImU32>(gui::settings::misc::checkboxColor.y * 255),
            static_cast<ImU32>(gui::settings::misc::checkboxColor.z * 255),
            170
        ));
    }
    if (rightToggle) {
       
       
        draw_list->AddTriangleFilled(rightp1, rightp2, rightp3, IM_COL32(
            static_cast<ImU32>(gui::settings::misc::checkboxColor.x * 255),
            static_cast<ImU32>(gui::settings::misc::checkboxColor.y * 255),
            static_cast<ImU32>(gui::settings::misc::checkboxColor.z * 255),
            170
        ));
    }

    // Проверка состояния джиттера
    if (gui::settings::aimbot::jitter) {
        ImVec2 leftrectStart = ImVec2(screenCenter.x - offset  - jitterWidth + 5.0f, screenCenter.y - jitterHeight / 2.0f);
        ImVec2 leftrectEnd = ImVec2(screenCenter.x - offset + 5.0f, screenCenter.y + jitterHeight / 2.0f);
        ImVec2 rightrectStart = ImVec2(screenCenter.x + offset - 5.0f, screenCenter.y - jitterHeight / 2.0f);
        ImVec2 rightrectEnd = ImVec2(screenCenter.x + offset + jitterWidth - 5.0f, screenCenter.y + jitterHeight / 2.0f);
        draw_list->AddRectFilled(leftrectStart, leftrectEnd, IM_COL32(10, 10, 10, 80));
        draw_list->AddRectFilled(rightrectStart, rightrectEnd, IM_COL32(10, 10, 10, 80));
        if (randomJitter < 0.0f) {draw_list->AddRectFilled(leftrectStart, leftrectEnd, IM_COL32(255, 255, 255, 255));}
        if (randomJitter > 0.0f) {draw_list->AddRectFilled(rightrectStart, rightrectEnd, IM_COL32(255, 255, 255, 255));}
    }
}

static int ticks = 0;
int ticksMax = 16;
int value = 10;
bool adaptive = true;
void Aimbot::FakeLag(UserCmd* cmd, bool& bsendpacket)
{
    if (!gui::settings::misc::fakelags)
        return;


    if (!globals::localPlayer || !globals::localPlayer->IsAlive())
        return;


    Vector3 velocity = globals::localPlayer->m_vecVelocity;
    float speed = velocity.Length2D();
    if (cmd->buttons & IN_ATTACK)
    {
        bsendpacket = true;
        return;
    }

    if (ticks >= ticksMax)
    {
        bsendpacket = true;
        ticks = 0;
    }
    else
    {
        if (gui::settings::misc::adaptive)
        {
            int packetsToChoke;
            if (speed > 0.f)
            {
                packetsToChoke = (int)((64.f / globalVars->intervalPerTick) / speed + 1);
                if (packetsToChoke >= 15)
                    packetsToChoke = 14;
                if (packetsToChoke < value)
                    packetsToChoke = value;
            }
            else
                packetsToChoke = 0;

            bsendpacket = ticks < 16 - packetsToChoke;
        }
        else
            bsendpacket = ticks < 16 - gui::settings::misc::fakelagticks;
    }

    ticks++;
}
// Функция для проверки видимости врага
bool Aimbot::CheckEnemyVisibility()
{
    if (player1 && bestHitboxIndex != -1) {
        // Проверяем видимость лучшего игрока
        return player1->IsVisible(bestHitboxIndex); // Используйте индекс лучшей кости
    }
    return false; // Враг не видимый
}
