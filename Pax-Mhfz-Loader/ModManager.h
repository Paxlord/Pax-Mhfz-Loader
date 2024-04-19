#pragma once
#include <Windows.h>
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>
#include <MinHook.h>

#include "imgui/imgui.h"
#include "globals.h"
#include "mod.h"

typedef Mod* ( *t_createMod)();
typedef void (*t_setDllAddy)(int);

class ModManager
{
private:
	std::vector<Mod*> mod_list;
	static ModManager* instance;
	ModManager();
public:
	static ModManager* GetInstance();
	std::vector<Mod*> GetModList();
	void AttachAll();
	void DetachAll();
	void InitializeImGUICtx(ImGuiContext* ctx);
	void DrawModMenu();
	void HookUpdates();
};

