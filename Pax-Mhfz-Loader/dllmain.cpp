// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "imgui_injection.h"
#include "ModManager.h"
#include "globals.h"
#include "color.hpp"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "libMinHook.x86.lib")

HINSTANCE dll_handle;
int mhfdll_addy = 0;

DWORD __stdcall EjectThread(LPVOID lpParameter) {
    Sleep(100);
    FreeLibraryAndExitThread(dll_handle, 0);
    return 0;
}

void SetMhfDllAddy() {
    //Getting the main dll adress
    do {
        mhfdll_addy = (DWORD)GetModuleHandleA("mhfo-hd.dll");
        Sleep(50);
    } while (!mhfdll_addy);
}

DWORD WINAPI Loader(HMODULE base) {

    //Get the main mhfz window handle before anything else
    IMGuiInjection::getWindowHandle();

    //Creating debug console
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);

    SetMhfDllAddy();
    
    std::cout << "mhfo-hd.dll addy found : " << dye::purple(mhfdll_addy) << std::endl;

    ModManager::get_instance()->AttachAll();
    IMGuiInjection::hookEndScene();
    ModManager::get_instance()->HookUpdates();

    while (true) {
        Sleep(50);
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
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loader, NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

