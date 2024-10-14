#pragma once
#include <cstdint>

class IGlobalVars
{
public:
    float realTime = 0.0f;
    std::int32_t frameCount = 0;
    float absFrameTime = 0.0f;
    float currentTime = 0.0f;
    float frameTime = 0.0f;
    std::int32_t maxClients = 0;
    std::int32_t tickCount = 0;
    float intervalPerTick = 0.0f;
    float interpolationAmount = 0.0f;
    std::int32_t frameSimulationTicks = 0;
    std::int32_t networkProtocol = 0;
    void* saveData = nullptr;
    bool client = false;
    bool remoteClient = false;
    std::int32_t timestampNetworkingBase = 0;
    std::int32_t timestampRandomizeWindow = 0;

    IGlobalVars() = default;

};
