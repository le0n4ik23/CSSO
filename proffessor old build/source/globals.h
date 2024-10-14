#pragma once
#include "../source/interfaces/interfaces.h"
#include <map>
class CEntity;
class CC4;
class CPlantedC4;
class CCSPlayerResource;


#define FL_ONGROUND (1<<0) //0001
namespace globals
{
    inline CEntity* localPlayer;
    inline CC4* C4;
    inline CPlantedC4* plantedC4;
    inline CCSPlayerResource* playerResource;
    inline CEntity* g_LocalPlayer;
    inline float currentTime = 0.0f;
    inline float realTime1 = 0.0f;
    inline float frameTime = 0.0f;
    
    inline float intervalPerTick = 0.0f;
    inline bool jumpbugActive = false; // Add the jumpbugActive variable
}

