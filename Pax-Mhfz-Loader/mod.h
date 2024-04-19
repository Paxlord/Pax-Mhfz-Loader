#pragma once
#include <Windows.h>
#include <string>
#include "imgui/imgui.h"

class Mod {
public:
	std::string name;
	std::string display_name;

	Mod(std::string name, std::string display_name) {
		this->name = name;
		this->display_name = display_name;
	}

	virtual void InitImGUIContext(ImGuiContext* ctx) = 0;
	virtual void Attach() = 0;
	virtual void Detach() = 0;
	virtual void DrawUI() {};
	virtual void UpdateQuest() {};
	virtual void UpdateLobby() {};
};