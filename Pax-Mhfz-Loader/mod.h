#pragma once
#include <Windows.h>
#include <string>
#include "imgui/imgui.h"

class Mod {
public:
	std::string name;
	std::string display_name;
	int required_version;

	Mod(std::string name, std::string display_name, int required_version) {
		this->name = name;
		this->display_name = display_name;
		this->required_version = required_version;
	}

	virtual void InitImGUIContext(ImGuiContext* ctx) = 0;
	//Will run once at game start, where you initialize things like hooks
	virtual void Attach() = 0;
	//Will run once on detach, used to clean up memory if needed
	virtual void Detach() = 0;
	//Linked to the main injected imgui context, can draw whatever here
	virtual void DrawUI() {};
	//Will run every frame in the main update function in quests
	virtual void UpdateQuest() {};
	//WIll run every frame in the main update function in the lobby
	virtual void UpdateLobby() {};
};