#include "ModManager.h"
#include "color.hpp"

namespace fs = std::filesystem;

typedef int(__thiscall* game_core)();
game_core gc;
DWORD game_core_addy = 0x89BE10;

int hk_game_core() {
	
	for (const auto& mod : ModManager::GetInstance()->GetModList()) {
		mod->UpdateQuest();
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

ModManager::ModManager() {

	std::string mod_folder = "./mods";
	for (const auto& entry : fs::directory_iterator(mod_folder)) {
		std::string file_extension = entry.path().extension().generic_string();
		if (file_extension != ".dll") continue;

		std::string absolute_path = fs::absolute(entry.path()).generic_string();

		HMODULE dll_handle = LoadLibrary(absolute_path.c_str());
		if (!dll_handle) {
			std::cout << "Error while loading dll : '" << absolute_path << "' Skipping..." << std::endl;
			continue;
		}
		
		t_createMod createMod = (t_createMod)(GetProcAddress(dll_handle, "createMod"));
		t_setDllAddy setDllAddy = (t_setDllAddy)(GetProcAddress(dll_handle, "setDllAddress"));

		if (!createMod || !setDllAddy) {
			std::cout << "GetProcAddress Failed " << GetLastError() << std::endl;
			continue;
		}

		setDllAddy(mhfdll_addy);
		Mod* mod = createMod();
		if (!mod) {
			std::cout << "Error while creating mod instance " << GetLastError() << std::endl;
			continue;
		}

		std::cout << "Found mod : " << dye::aqua(mod->name) << " at path : " << dye::green(absolute_path) << std::endl;
		if (mod->required_version > VERSION) {
			std::cout << "Missmatched version between loader (" << VERSION << ") and " << mod->name << " required version (" << mod->required_version << ") Please update the loader to run this mod." << std::endl;
			continue;
		}
		mod_list.push_back(mod);
	}

}

void ModManager::AttachAll() {
	for (const auto& mod : mod_list) {
		mod->Attach();
	}
	std::cout << "Attached " << mod_list.size() << " mods successfully." << std::endl;
}

void ModManager::DetachAll() {

}

void ModManager::DrawModMenu() {
	ImGui::Begin("Mod Menu");
	for (const auto& mod: mod_list) {
		if (ImGui::CollapsingHeader(mod->display_name.c_str())) {
			mod->DrawUI();
		}
	}
	ImGui::End();
}

void ModManager::InitializeImGUICtx(ImGuiContext* ctx) {
	for (const auto& mod : mod_list) {
		mod->InitImGUIContext(ctx);
	}
}

void ModManager::HookUpdates() {
	MH_CreateHook((LPVOID)OffsetByDll(game_core_addy), (LPVOID)hk_game_core, (LPVOID*)&gc);
	MH_EnableHook((LPVOID)OffsetByDll(game_core_addy));
}

