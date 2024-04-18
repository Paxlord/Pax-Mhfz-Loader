// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "imgui_injection.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "libMinHook.x86.lib")

HINSTANCE dll_handle;
DWORD* mhfdll_addy = nullptr;

DWORD __stdcall EjectThread(LPVOID lpParameter) {
    Sleep(100);
    FreeLibraryAndExitThread(dll_handle, 0);
    return 0;
}

DWORD WINAPI Loader(HMODULE base) {

    IMGuiInjection::getWindowHandle();

    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);

    IMGuiInjection::hookEndScene();

    do {
        mhfdll_addy = (DWORD*)GetModuleHandleA("mhfo-hd.dll");
        Sleep(50);
    } while (!mhfdll_addy);

    std::cout << "DLLMAIN : mhfo-hd.dll addy found : " << mhfdll_addy << std::endl;

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

