#include "ModManager.h"
#include "color.hpp"

namespace fs = std::filesystem;

ModManager* ModManager::instance = nullptr;

ModManager* ModManager::get_instance() {
	if (!instance) {
		instance = new ModManager();
	}
	return instance;
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

		std::cout << "Loaded mod : " << dye::aqua(mod->name) << " at path : " << dye::green(absolute_path) << std::endl;
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
		mod->DrawUI();
	}
	ImGui::End();
}