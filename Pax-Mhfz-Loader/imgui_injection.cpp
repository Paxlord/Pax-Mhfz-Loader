#include "imgui_injection.h"
#include <fstream>
#include <ini.h>
#include "color.hpp"

typedef HRESULT(__stdcall* EndScene)(IDirect3DDevice9* pDevice);
typedef HRESULT(APIENTRY* Reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
EndScene pEndScene;
Reset oReset;

WNDPROC oWndProc;
HWND window;

LPD3DXFONT pFont;
LPDIRECT3DDEVICE9 game_pDevice;

bool init = false;
bool showMenu = true;
bool initImgui = false;

BOOL CALLBACK EnumWindowCB(HWND hwnd, LPARAM lParam)
{
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (GetCurrentProcessId() != lpdwProcessId) {
		return TRUE;
	}
	window = hwnd;
	return FALSE;
}

HWND GetProcessWindow() {
    window = NULL;
    EnumWindows(EnumWindowCB, NULL);
    return window;
}

void InitImGUI(LPDIRECT3DDEVICE9 lpDevice) {
    ImGui::CreateContext();

    ModManager::GetInstance()->InitializeImGUI(ImGui::GetCurrentContext());
    game_pDevice = lpDevice;
    ModManager::GetInstance()->OnInitImGUIAll();

    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(lpDevice);
    io.ImeWindowHandle = window;

    mINI::INIFile imgui("./paxloader.ini");
    mINI::INIStructure ini_imgui;

    imgui.read(ini_imgui);

    auto dont_show_val = ini_imgui.get("Mod Menu Config").get("dont_show");
    if(!dont_show_val.empty())
        showMenu = !(std::stoi(dont_show_val) != 0);
}


HRESULT __stdcall hkEndScene(IDirect3DDevice9* pDevice) {
    
    if (!init) {
        InitImGUI(pDevice);
        init = true;
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    if (GetAsyncKeyState(VK_F12) & 1)
        showMenu = !showMenu;

    ImGui::GetIO().MouseDrawCursor = showMenu;
    if (showMenu == true && mhfdll_addy) {
        ModManager::GetInstance()->DrawModMenu();
    }

    if (mhfdll_addy) {
        ModManager::GetInstance()->DrawOverlay(showMenu);
    }
    
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    
    return pEndScene(pDevice);
}

HRESULT APIENTRY hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = oReset(pDevice, pPresentationParameters);
    ImGui_ImplDX9_CreateDeviceObjects();
    return hr;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (showMenu && ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
        return true;
    }
    return CallWindowProc(oWndProc, hwnd, msg, wParam, lParam);
}

void IMGuiInjection::hookEndScene() {
    std::cout << dye::yellow("[MODLOADER] ") << "Starting DX9 Hooking process..." << std::endl;
    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (pD3D == NULL) {
        std::cout << dye::yellow("[MODLOADER] ") << "Failed to create D3D9" << std::endl;
        return;
    }

    D3DPRESENT_PARAMETERS d3dparams = { 0 };
    d3dparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dparams.hDeviceWindow = window;
    d3dparams.Windowed = true;

    IDirect3DDevice9* pDevice = NULL;

    HRESULT result = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dparams.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dparams, &pDevice);
    if (FAILED(result) || pDevice == NULL) {
        std::cout << dye::yellow("[MODLOADER] ") << "Failed to Create Device" << std::endl;
        pD3D->Release();
        return;
    }

    void** vTable = *reinterpret_cast<void***>(pDevice);

    if (MH_Initialize() != MH_OK) {
        std::cout << dye::yellow("[MODLOADER] ") << "Failed to initialize MinHook" << std::endl;
        return;
    }

    if (MH_CreateHook(vTable[42], &hkEndScene, reinterpret_cast<void**>(&pEndScene)) != MH_OK) {
        std::cout << dye::yellow("[MODLOADER] ") << "Failed to create hook" << std::endl;
        return;
    }

    if (MH_EnableHook(vTable[42]) != MH_OK) {
        std::cout << dye::yellow("[MODLOADER] ") << "Failed to enable hook" << std::endl;
        return;
    }
    std::cout << dye::yellow("[MODLOADER] ") << "Hooked EndScene..." << std::endl;

    MH_CreateHook(vTable[16], &hkReset, reinterpret_cast<void**>(&oReset));
    MH_EnableHook(vTable[16]);

    std::cout << dye::yellow("[MODLOADER] ") << "Hooked Reset..." << std::endl;

    std::cout << dye::yellow("[MODLOADER] ") << "Imgui Injection complete!" << std::endl;
    oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);

    pDevice->Release();
    pD3D->Release();
}

HWND IMGuiInjection::getWindowHandle() {
    do {
        window = GetProcessWindow();
    } while (window == NULL);

    return window;
}

void* IMGuiInjection::CreateDx9Tex(std::string tex_path) {
    PDIRECT3DTEXTURE9 pTexture = NULL;
    std::cout << dye::yellow("[MODLOADER] ") << "Creating dx9 texture pointer from file : " << tex_path << std::endl;
    HRESULT hr = D3DXCreateTextureFromFileA(game_pDevice, tex_path.c_str(), &pTexture);
    if (FAILED(hr)) {
        std::cout << dye::yellow("[MODLOADER] ") << "Failed to create texture with code " << std::hex << hr << std::dec << std::endl;
        return NULL;
    }
    return reinterpret_cast<void*>(pTexture);
}