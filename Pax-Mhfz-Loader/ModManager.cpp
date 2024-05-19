#include "ModManager.h"
#include "color.hpp"
#include "imgui_injection.h";
#include <ini.h>

#include <fstream>
#include <typeinfo>
#include <unordered_set>
#include <map>
	
namespace fs = std::filesystem;
bool dont_show = false;

typedef int(__thiscall* game_core)();
game_core gc;
DWORD game_core_addy = 0x89BE10;

int hk_game_core() {
	
	for (const auto& mod : ModManager::GetInstance()->GetModList()) {
		mod->OnUpdateQuest();
	}

	return gc();
}

ModManager* ModManager::instance = nullptr;

ModManager* ModManager::GetInstance() {
	if (!instance) {
		instance = new ModManager();
	}
	return instance;
}

std::vector<Mod*> ModManager::GetModList() {
	return mod_list;
}

void LoadINIConfig() {
	std::cout << dye::yellow("[MODLOADER]") << "Loading INI config file..." << std::endl;
	mINI::INIFile imgui("./paxloader.ini");
	mINI::INIStructure ini_imgui;

	imgui.read(ini_imgui);

	auto dont_show_val = ini_imgui.get("Mod Menu Config").get("dont_show");
	if (!dont_show_val.empty())
		dont_show = std::stoi(dont_show_val) != 0;
	std::cout << dye::yellow("[MODLOADER]") << "Done with INI config file..." << std::endl;
}


ModManager::ModManager() {

	LoadConfig();
	LoadINIConfig();

	std::string mod_folder = "./mods";

	if (!fs::exists(fs::status(mod_folder))) return;
	std::cout << dye::yellow("[MODLOADER]") << "Found mod folder. Parsing..." << std::endl;
	for (const auto& entry : fs::directory_iterator(mod_folder)) {
		std::string file_extension = entry.path().extension().generic_string();
		if (file_extension != ".dll") continue;

		std::string absolute_path = fs::absolute(entry.path()).generic_string();

		HMODULE dll_handle = LoadLibrary(absolute_path.c_str());
		if (!dll_handle) {
			std::cout << dye::yellow("[MODLOADER]") << "Error while loading dll : '" << absolute_path << "' Skipping..." << std::endl;
			continue;
		}
		
		t_createMod createMod = (t_createMod)(GetProcAddress(dll_handle, "createMod"));
		t_setDllAddy setDllAddy = (t_setDllAddy)(GetProcAddress(dll_handle, "setDllAddress"));

		if (!createMod || !setDllAddy) {
			std::cout << dye::yellow("[MODLOADER]") << "GetProcAddress Failed " << GetLastError() << std::endl;
			continue;
		}

		setDllAddy(mhfdll_addy);
		Mod* mod = createMod();
		if (!mod) {
			std::cout << dye::yellow("[MODLOADER]") << "Error while creating mod instance " << GetLastError() << std::endl;
			continue;
		}

		std::cout << dye::yellow("[MODLOADER]") << "Found mod : " << dye::aqua(mod->name) << " at path : " << dye::green(absolute_path) << std::endl;
		if(CheckModValidity(mod))
			mod_list.push_back(mod);
	}

	CheckRequired();

}

void ModManager::LoadConfig() {
	std::cout << dye::yellow("[MODLOADER]") << "Loading json config file..." << std::endl;

	std::ifstream f("config.json");
	if (f.fail()) {
		std::cout << dye::yellow("[MODLOADER]") << "Did not find any json file, skipping..." << std::endl;
		return;
	}

	json config_json = json::parse(f);
	if(config_json.contains("no_lge"))
		NO_LGE = config_json["no_lge"].template get<bool>();

	ParseConfigArray(config_json, "required", required);
	ParseConfigArray(config_json, "allowed", allowed);

	std::cout << dye::yellow("[MODLOADER]") << "Required items:" << std::endl;
	for (const auto& req : required) {
		std::cout << req->name << ": " << req->version << ", ";
		std::cout << std::endl;
	}

	std::cout << dye::yellow("[MODLOADER]") << "Allowed items:" << std::endl;
	for (const auto& allow : allowed) {
		std::cout << allow->name << ": " << allow->version << ", ";
		std::cout << std::endl;
	}

	f.close();
	std::cout << dye::yellow("[MODLOADER]") << "Done with json config file..." << std::endl;
}

void ModManager::AttachAll() {
	for (const auto& mod : mod_list) {
		mod->OnAttach();
	}
	std::cout << "Attached " << mod_list.size() << " mods successfully." << std::endl;

}

void ModManager::DetachAll() {

}

void ModManager::OnInitImGUIAll() {
	for (const auto& mod : mod_list) {
		mod->OnImGUIInit();
	}
}

void ModManager::DrawModMenu() {
	ImGui::Begin("Mod Menu");
	ImGui::Text("Press F12 to hide/show the menu!");

	if (ImGui::Checkbox("Hide on startup", &dont_show)) {
		mINI::INIFile imgui("./paxloader.ini");
		mINI::INIStructure ini_imgui;
		imgui.read(ini_imgui);
		ini_imgui["Mod Menu Config"]["dont_show"] = dont_show ? "1" : "0";
		imgui.write(ini_imgui, true);
	}

	if (ImGui::CollapsingHeader("Mod List")) {
		ImGui::Text("%d Active mods", mod_list.size());
		for (const auto& mod : mod_list) {
			ImGui::Text("%s - v.%s", mod->name.c_str(), mod->version.c_str());
		}
	}

	for (const auto& mod: mod_list) {
		mod->DrawModMenu();
	}
	ImGui::End();
}

void ModManager::DrawOverlay(bool show_menu) {
	for (const auto& mod : mod_list) {
		mod->DrawUI(show_menu);
	}
}

void ModManager::InitializeImGUI(ImGuiContext* ctx) {
	for (const auto& mod : mod_list) {
		mod->InitImGUIContext(ctx);
		mod->SetCreateImageData(IMGuiInjection::CreateDx9Tex);
	}
}

void ModManager::HookUpdates() {
	MH_CreateHook((LPVOID)OffsetByDll(game_core_addy), (LPVOID)hk_game_core, (LPVOID*)&gc);
	MH_EnableHook((LPVOID)OffsetByDll(game_core_addy));
}

void ModManager::ParseConfigArray(json config_json, std::string array_key, std::vector<Mod_Config_List*> &dest_array) {
	if (config_json.contains(array_key) && config_json[array_key].is_array()) {
		for (const auto& item : config_json[array_key]) {
			Mod_Config_List* mod_conf = new Mod_Config_List();
			mod_conf->name = item["name"].template get<std::string>();
			mod_conf->version = item["version"].template get<std::string>();
			dest_array.push_back(mod_conf);
		}
	}
}

bool ModManager::CheckModValidity(Mod* mod) {

	//Check if mod name already exists in list
	for (const auto& loaded_mod : mod_list) {
		if (loaded_mod->name == mod->name) {
			std::cout << "Mod " << mod->name << "has already been loaded. " << std::endl;
			return false;
		}
	}

	//Match mod loader version
	if (mod->required_version > VERSION) {
		std::cout << "Missmatched version between loader (" << VERSION << ") and " << mod->name << " required version (" << mod->required_version << ") Please update the loader to run this mod." << std::endl;
		return false;
	}

	//Check for allowed
	if (allowed.size() > 0) {
		bool is_in = false;
		for (const auto &mod_entry : allowed) {
			if (mod_entry->name == mod->name) {
				is_in = true;

				if (mod_entry->version != mod->version && mod_entry->version != "any") {
					std::cout << "Missmatched mod version in allowed list for mod " << mod->name << " only version " << mod_entry->version << " is allowed. But found " << mod->version << std::endl;
					return false;
				}

				break;
			}
		}

		if (!is_in) {
			std::cout << "Allowed mod list set but mod " << mod->name << " isn't in it." << std::endl;
			return false;
		}
	}

	if (mod->hge_only && !NO_LGE) {
		std::cout << "Mod " << mod->name << " has the hge_only flag enabled but this loader isn't set as no_lge." << std::endl;
		return false;
	}


	return true;
}

void ModManager::CheckRequired() {
	
	if (required.size() <= 0) return;
	if (mod_list.size() < required.size()) {
		std::string str = "Required list has more mods than loaded mod list. Terminating...";
		MessageBoxA(NULL, str.c_str(), "Required list error", MB_ICONWARNING | MB_OK);
		Sleep(100);
		TerminateProcess(GetCurrentProcess(), 0);
	}
	//Generate a set of all loaded mod names
	std::unordered_set<std::string> set;
	std::map<std::string, std::string> mod_list_version_lookup;

	for (const auto& mod : mod_list) {
		set.insert(mod->name);
		mod_list_version_lookup.insert({ mod->name, mod->version });
	}

	for (const auto& required_mod : required) {

		if (set.find(required_mod->name) == set.end()) {
			std::string str = "Required mod : " + required_mod->name + " isn't loaded. Terminating...";
			MessageBoxA(NULL, str.c_str(), "Required list error", MB_ICONWARNING | MB_OK);
			Sleep(100);
			TerminateProcess(GetCurrentProcess(), 0);
		}

		if (required_mod->version != "any" && mod_list_version_lookup[required_mod->name] != required_mod->version) {
			std::string str = "Version mismatch for required mod : " + required_mod->name + " version " + required_mod->version + " needed.";
			MessageBoxA(NULL, str.c_str(), "Required list error", MB_ICONWARNING | MB_OK);
			Sleep(100);
			TerminateProcess(GetCurrentProcess(), 0);
		}

	}

}

