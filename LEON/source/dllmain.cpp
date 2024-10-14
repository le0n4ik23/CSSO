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
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

void Cleanup(HMODULE hModule)
{
    hooks::RemoveHooks();
    gui::Destroy();

#ifdef _DEBUG
    FreeConsole();
#endif

    FreeLibraryAndExitThread(hModule, 0);
}
DWORD WINAPI HackThread(HMODULE hModule)
{
    // Console output in debug mode
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "HackThread initialized.\n";
#endif
    SetupInterfaces();
    SetupNetvars();
    int g_SetupError = 0;
    if (g_SetupError != 0)
    {
        std::cerr << "Failed to setup interfaces or netvars.\n";
        Cleanup(hModule);
        return 1;
    }

    gui::Setup();
    hooks::SetupHooks();

    // Addresses and pattern scans
    uintptr_t engineDll = (uintptr_t)GetModuleHandle("engine.dll");
    if (!engineDll)
    {
        std::cerr << "Failed to get engine.dll base.\n";
        Cleanup(hModule);
        return 1;
    }

    uintptr_t offset = 0x744C58;  // Constant offset
    uintptr_t finalAddressToWrite = engineDll + offset;

    uint8_t* functionAddress = util::PatternScan("engine.dll", "C6 45 ? ? 8B 01 8B 40");
    uint8_t* clantagAddress = util::PatternScan("engine.dll", "55 8B EC 56 8B F1 6A ? FF 75 ? C7 06 ? ? ? ? C7 46 ? ? ? ? ?");

    if (!functionAddress || !clantagAddress)
    {
        std::cerr << "Pattern scan failed.\n";
        Cleanup(hModule);
        return 1;
    }

    int* valuePtr = reinterpret_cast<int*>(finalAddressToWrite);
    if (!valuePtr)
    {
        std::cerr << "Invalid address to write value.\n";
        Cleanup(hModule);
        return 1;
    }

    // Main loop
    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(functionAddress), sizeof(char), PAGE_EXECUTE_READWRITE, &oldProtect);

    int8_t var21Offset = -0x21;
    uint8_t* ebp = functionAddress + 0x3;

    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }

        // Avoid unnecessary writes
        if (*valuePtr == 0)
        {
            *valuePtr = 1;
        }

        // Update bSendPacket based on hooks
        char bSendPacket = hooks::bSendPacket ? 1 : 0;

        // Only write if there is a change
        if (*reinterpret_cast<char*>(ebp) != bSendPacket)
        {
            *reinterpret_cast<char*>(ebp) = bSendPacket;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Reduced CPU usage
    }

    VirtualProtect(reinterpret_cast<LPVOID>(functionAddress), sizeof(char), oldProtect, &oldProtect);
    Cleanup(hModule);
    return 0;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {

        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
  
    return TRUE;
}

