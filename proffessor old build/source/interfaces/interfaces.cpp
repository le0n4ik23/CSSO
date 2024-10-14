#include "interfaces.h"

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
    if (client)
        std::cout << "found client Interface " << client << std::endl;

    if (entityList)
        std::cout << "found entityList Interface " << entityList << std::endl;

    if (debugOverlay)
        std::cout << "found debugOverlay Interface " << debugOverlay << std::endl;

    if (vguiPanel)
        std::cout << "found vguiPanel Interface " << vguiPanel << std::endl;

    if (surface)
        std::cout << "found surface Interface " << surface << std::endl;

    if (engine)
        std::cout << "found engine Interface " << engine << std::endl;

    if (engineTrace)
        std::cout << "found trace Interface " << engineTrace << std::endl;

    if (modelRender)
        std::cout << "found modelRender Interface " << modelRender << std::endl;

    if (studioRender)
        std::cout << "found studioRender Interface " << studioRender << std::endl;

    if (matSystem)
        std::cout << "found matSystem Interface " << matSystem << std::endl;

    if (modelInfo)
        std::cout << "found modelInfo Interface " << modelInfo << std::endl;

    if (engineClientReplay)
        std::cout << "found engineClientReplay Interface " << engineClientReplay << std::endl;

    if (renderView)
        std::cout << "found renderView Interface " << renderView << std::endl;
 
   
#endif

}

bool ITraceFilter::ShouldHitEntity(CEntity* pEntity, int contentsMask)
{
    return !pEntity || (pEntity->GetClientClass() && pEntity->GetClientClass()->classID == 31);
}
