#include "engine_prediction.h"
#include "interfaces.h"
#include "../classes/game_movement.h"
#include "../classes/client_prediction.h"
c_engine_prediction::c_engine_prediction()
    : old_current_time(0.0f),
    old_frametime(0.0f),
    move_helper(nullptr),
    flags(0)
{
}

void c_engine_prediction::start(UserCmd* cmd, CEntity* localPlayer) 
{
    old_current_time = globalVars->currentTime;
    old_frametime = globalVars->frameTime;

  
    flags = localPlayer->m_fFlags;
  

   globalVars->currentTime = localPlayer->m_nTickBase *globalVars->intervalPerTick;
   globalVars->frameTime = globalVars->intervalPerTick;

    *(DWORD*)((uint32_t)localPlayer + 12080) = (DWORD)cmd;
    *(int*)(0x10686DF0) = cmd->randomSeed;
    *(CEntity**)(0x106D3C2C) = localPlayer;

   


    memset(&move_data, 0, sizeof(move_data));

    game_movement->start_track_prediction_errors(localPlayer);


    client_prediction->setup_move(localPlayer, cmd, move_helper, &move_data);
    game_movement->process_movement(localPlayer, &move_data);
   
  
    client_prediction->finish_move(localPlayer, cmd, &move_data);
}

void c_engine_prediction::end(UserCmd* cmd, CEntity* localPlayer)
{
   
    game_movement->finish_track_prediction_errors(localPlayer);

    *(DWORD*)((uint32_t)localPlayer + 12080) = NULL;
    *(int*)(0x10686DF0) = -1;
    *(CEntity**)(0x106D3C2C) = NULL;


   globalVars->currentTime = old_current_time;
   globalVars->frameTime = old_frametime;
}
