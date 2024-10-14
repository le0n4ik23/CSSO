#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include "pch.h"
#include "includes.h"
#include "../source/classes/CEntity.h"
#include "../source/interfaces/interfaces.h"
#include "bitset"
#include "globals.h"
#include "../source/features/misc.h"
#include "hooks.h"
#include "../source/gui.h"


DWORD WINAPI HackThread(HMODULE hModule)
{
    //console
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "pp\n";
#endif
    

    SetupInterfaces();
    SetupNetvars();
    gui::Setup();
    hooks::SetupHooks();

    //mod base
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle("hl2.exe");
    uintptr_t clientDll = (uintptr_t)GetModuleHandle("client.dll");
    
        

    bool showHealth = false;
    //loop
    while (true)
    {

        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }
        Sleep(5);
    }

    hooks::RemoveHooks();
    gui::Destroy();
    //clean and eject
#ifdef _DEBUG
    fclose(f);
    FreeConsole();
#endif
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));

    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



