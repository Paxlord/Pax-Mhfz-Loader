#include <Windows.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <TlHelp32.h>
#include <tchar.h>

#include "imgui_injection.h"
#include "ModManager.h"
#include "globals.h"
#include "color.hpp"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "libMinHook.x86.lib")

HINSTANCE dll_handle;
HANDLE loader_thread;

//See Thread Walking page of the MDN Docs
std::vector<HANDLE> ListProcessThreads(DWORD dwOwnerPID) {

    HANDLE hthreadsnapshot = INVALID_HANDLE_VALUE;
    THREADENTRY32 tentry;
    std::vector<HANDLE> thread_list;

    hthreadsnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hthreadsnapshot == INVALID_HANDLE_VALUE)
        return thread_list;
    
    tentry.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(hthreadsnapshot, &tentry)) {
        CloseHandle(hthreadsnapshot);
        return thread_list;
    }

    do {
        if (tentry.th32OwnerProcessID == dwOwnerPID)
        {
            HANDLE thread_handle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, tentry.th32ThreadID);
            if (thread_handle && tentry.th32ThreadID != GetThreadId(GetCurrentThread()))
                thread_list.push_back(thread_handle);
        }
    } while (Thread32Next(hthreadsnapshot, &tentry));

    CloseHandle(hthreadsnapshot);
    return thread_list;

}

DWORD __stdcall EjectThread(LPVOID lpParameter) {
    Sleep(100);
    if (NO_LGE)
        TerminateProcess(GetCurrentProcess(), 0);
    FreeLibraryAndExitThread(dll_handle, 0);
    return 0;
}

bool SetMhfDllAddy() {
    //Getting the main dll adress
    DWORD lge_addy;

    do {
        mhfdll_addy = (DWORD)GetModuleHandleA("mhfo-hd.dll");
        lge_addy = (DWORD)GetModuleHandleA("mhfo.dll");
        if (lge_addy) {
            return false;
        }
        Sleep(50);
    } while (!mhfdll_addy);

    return true;
}

void ModManagerInit() {
    ModManager::GetInstance()->AttachAll();
}

void SuspendThreads(std::vector<HANDLE> list) {
    for (const auto handle : list) {
        SuspendThread(handle);
    }
}

void ResumeThreads(std::vector<HANDLE> list) {
    for (const auto handle : list) {
        ResumeThread(handle);
    }
}

DWORD WINAPI Loader(HMODULE base) {

    //Version.dll injects on launcher startup, so we're setting the thread in an infinite loop until the actual game is launched
    SetMhfDllAddy();

    //Get the main mhfz window handle before anything else
    HWND main_window = IMGuiInjection::getWindowHandle();

    //Creating debug console
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);

    std::vector<HANDLE> list = ListProcessThreads(GetCurrentProcessId());

    if (SetMhfDllAddy()) {
        std::cout << dye::yellow("[MODLOADER] ") << "mhfo-hd.dll address found: 0x" << std::hex << mhfdll_addy << std::dec << std::endl;

        //First suspension to initialize the loader itself
        std::cout << dye::yellow("[MODLOADER] ") << "Loading mod files to loader..." << std::endl;
        ModManager::GetInstance();
        std::cout << dye::yellow("[MODLOADER] ") << "Done with mod loading..." << std::endl;

        //Infinite loop until the game manager is initialized
        do {
            Sleep(50);
        } while (*(DWORD*)(mhfdll_addy + 0xE7FFF3C) == 0);

        //Second suspension after the game initialized to run mods attach function
        SuspendThreads(list);
        std::cout << dye::yellow("[MODLOADER] ") << "Attaching all mods..." << std::endl;
        ModManagerInit();
        ResumeThreads(list);

        //Once we've loaded every mod, we hook dx9 and display the mods UI
        std::cout << dye::yellow("[MODLOADER] ") << "Beginning of ImGui Setup..." << std::endl;
        IMGuiInjection::hookEndScene();

        ModManager::GetInstance()->HookUpdates();

        //Infinite loop to keep this thread up
        while (true) {
            Sleep(50);
        }
    }
    else {
        
        if (!NO_LGE) {
            MessageBoxA(NULL, "Low-Grade Edition isn't supported by this mod loader, please launch the game using High-Grade Edition if you want mods to be loaded.", "LGE Detected", MB_ICONWARNING | MB_OK);
        }
        else {
            MessageBoxA(NULL, "This server decided to not support Low-Grade Edition, please launch the game in High-Grade Edition to play.", "LGE Detected", MB_ICONERROR | MB_OK);
        }

    }

    Sleep(1000);
    if (fp != nullptr) {
        fclose(fp);
    }
    FreeConsole();
    CreateThread(0, 0, EjectThread, 0, 0, 0);
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
        dll_handle = hModule;
        DisableThreadLibraryCalls(hModule);
        loader_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loader, NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

