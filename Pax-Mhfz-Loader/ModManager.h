#pragma once
#include <Windows.h>
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>
#include <MinHook.h>
#include <json.hpp>

#include "imgui/imgui.h"
#include "globals.h"
#include "../paxloader-basemod/BaseMod/mod.h"

using json = nlohmann::json;

typedef Mod* ( *t_createMod)();
typedef void (*t_setDllAddy)(int);

typedef struct {
	std::string name;
	std::string version;
} Mod_Config_List;

class ModManager
{
private:
	std::vector<Mod*> mod_list;
	std::vector<Mod_Config_List*> required, allowed;
	static ModManager* instance;
	ModManager();
public:
	static ModManager* GetInstance();
	std::vector<Mod*> GetModList();
	void AttachAll();
	void DetachAll();
	void OnInitImGUIAll();
	void InitializeImGUI(ImGuiContext* ctx);
	void DrawModMenu();
	void HookUpdates();

	void LoadConfig();
	void ParseConfigArray(json config_json, std::string array_key, std::vector<Mod_Config_List*> &dest_array);
	bool CheckModValidity(Mod* mod);
	void CheckRequired();
};

