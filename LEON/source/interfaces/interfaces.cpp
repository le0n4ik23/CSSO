#include "interfaces.h"
#include "../classes/game_movement.h"
#include "../classes/client_prediction.h"
template <typename T>
T* GetInterface(const char* name, const char* library)
{
    const auto handle = GetModuleHandle(library);

    if (!handle)
        return nullptr;

    const auto functionAddress = GetProcAddress(handle, "CreateInterface");

    if (!functionAddress)
        return nullptr;

    using Fn = T * (*)(const char*, int*);
    const auto CreateInterface = reinterpret_cast<Fn>(functionAddress);

    return CreateInterface(name, nullptr);
}

void SetupInterfaces()
{
    client = GetInterface<IBaseClientDll>("VClient017", "client.dll");


    entityList = GetInterface<IClientEntityList>("VClientEntityList003", "client.dll");


    debugOverlay = GetInterface<IVDebugOverlay>("VDebugOverlay004", "engine.dll");


    vguiPanel = GetInterface<IPanel>("VGUI_Panel009", "vgui2.dll");


    surface = GetInterface<vgui::ISurface>("VGUI_Surface030", "vguimatsurface.dll");


    engine = GetInterface<IVEngineClient>("VEngineClient013", "engine.dll");

    game_movement = GetInterface<i_game_movement>("GameMovement001", "client.dll");
    client_prediction = GetInterface<v_client_prediction>("VClientPrediction001", "client.dll");
  


    engineTrace = GetInterface<IEngineTrace>("EngineTraceClient003", "engine.dll");


    modelRender = GetInterface<IVModelRender>("VEngineModel016", "engine.dll");


    studioRender = GetInterface<IStudioRender>("VStudioRender025", "studiorender.dll");


    matSystem = GetInterface<IMaterialSystem>("VMaterialSystem081", "MaterialSystem.dll");


    modelInfo = GetInterface<IVModelInfo>("VModelInfoClient006", "engine.dll");

  


    //if (!globalVars)
    //    globalVars = (IGlobalVars*)((uintptr_t)(GetModuleHandle("engine.dll")) + 0xF7AFB10);

    //if (globalVars)
    //    std::cout << globalVars->currentTime << std::endl;
    
    //
    engineClientReplay = GetInterface<IEngineClientReplay>("EngineClientReplay001", "engine.dll");


    renderView = GetInterface<IVRenderView>("VEngineRenderView014", "engine.dll");


    if (engineClientReplay)
        globalVars = engineClientReplay->GetGlobalVars();

    g_ClientMode = **reinterpret_cast<IBaseClientDll***>((*reinterpret_cast<unsigned int**>(client))[10] + 5);

#ifdef _DEBUG
   
        std::cout << "found client Interface " << client << std::endl;

   
        std::cout << "found entityList Interface " << entityList << std::endl;

        std::cout << "found debugOverlay Interface " << debugOverlay << std::endl;

  
        std::cout << "found vguiPanel Interface " << vguiPanel << std::endl;


        std::cout << "found surface Interface " << surface << std::endl;

  
        std::cout << "found engine Interface " << engine << std::endl;

 
        std::cout << "found trace Interface " << engineTrace << std::endl;

   
        std::cout << "found modelRender Interface " << modelRender << std::endl;

    
        std::cout << "found studioRender Interface " << studioRender << std::endl;

    
        std::cout << "found matSystem Interface " << matSystem << std::endl;

        std::cout << "found modelInfo Interface " << modelInfo << std::endl;

  
        std::cout << "found engineClientReplay Interface " << engineClientReplay << std::endl;

   
        std::cout << "found renderView Interface " << renderView << std::endl;
 
   
#endif

}