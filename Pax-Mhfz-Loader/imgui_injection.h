#pragma once
#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <MinHook.h>
#include <iostream>

#include "globals.h"
#include "ModManager.h"

namespace IMGuiInjection{
	void hookEndScene();
	HWND getWindowHandle();
	void* CreateDx9Tex(std::string);
}