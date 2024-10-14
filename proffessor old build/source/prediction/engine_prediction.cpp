#include "engine_prediction.hpp"
#include "../util/util.h"
#include "../classes/CEntity.h"
#include "../util/vector.h"

player_prediction* prediction1 = nullptr;
int* prediction_random_seed = nullptr;
player_move_helper* move_helper = nullptr;
player_move_data* move_data = nullptr;
virtual_game_movement* game_movement = nullptr;

void prediction::start(UserCmd* cmd) {
    if (!globals::localPlayer)
        return;

    origin = globals::localPlayer->m_vecOrigin;
    velocity = globals::localPlayer->m_vecVelocity;
    flags = globals::localPlayer->m_fFlags;

    if (!prediction_random_seed)
        prediction_random_seed = *reinterpret_cast<int**>(util::PatternScan("client.dll", "0F B7 81 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 C3") + 2);

    *prediction_random_seed = cmd->randomSeed;

    old_cur_time = globalVars->currentTime;
    old_frame_time = globalVars->frameTime;

    globals::currentTime = globals::localPlayer->m_nTickBase * globalVars->intervalPerTick;
    globals::frameTime = globalVars->intervalPerTick;

    memset(&data, 0, sizeof(data));
    move_helper->set_host(globals::localPlayer);
    prediction1->setup_move(globals::localPlayer, cmd, move_helper, &data);
    game_movement->process_movement(globals::localPlayer, &data);
    prediction1->finish_move(globals::localPlayer, cmd, &data);
}

void prediction::end() {
    if (!globals::localPlayer)
        return;

    move_helper->set_host(nullptr);

    *prediction_random_seed = -1;

    globalVars->currentTime = old_cur_time;
    globalVars->frameTime = old_frame_time;
}

void prediction::restore(int predicted_frame)
{
    static auto restore = reinterpret_cast<DWORD(__stdcall*)(int, int)>(util::PatternScan(("client.dll"), ("0F B7 81 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 C3")));
    restore(0, predicted_frame);
}
