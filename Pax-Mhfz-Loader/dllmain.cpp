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
int mhfdll_addy = 0;

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

void ModManagerInit() {
    ModManager::GetInstance()->AttachAll();
    IMGuiInjection::hookEndScene();
    ModManager::GetInstance()->HookUpdates();
}

DWORD WINAPI Loader(HMODULE base) {

    //Get the main mhfz window handle before anything else
    HWND main_window = IMGuiInjection::getWindowHandle();

    //Creating debug console
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);

    std::vector<HANDLE> list = ListProcessThreads(GetCurrentProcessId());

    SetMhfDllAddy();
    std::cout << "mhfo-hd.dll address found: 0x" << std::hex << dye::purple(mhfdll_addy) << std::dec << std::endl;

    for (const auto handle : list) {
        SuspendThread(handle);
    }

    ModManagerInit();

    for (const auto handle : list) {
        ResumeThread(handle);
    }

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
        loader_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loader, NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

