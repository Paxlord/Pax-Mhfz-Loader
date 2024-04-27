#pragma once
#include <Windows.h>
#include <string>
#include "imgui/imgui.h"

typedef void* (*createImageData)(std::string);

class Mod {
protected:
	createImageData CreateImage;

public:
	std::string name;
	std::string display_name;
	std::string version;
	bool hge_only;
	int required_version;

	Mod(std::string name, std::string display_name, std::string version, int required_version, bool hge_only) {
		this->name = name;
		this->display_name = display_name;
		this->required_version = required_version;
		this->version = version;
		this->hge_only = hge_only;
	}

	void SetCreateImageData(createImageData func) {
		this->CreateImage = func;
	}

	virtual void InitImGUIContext(ImGuiContext* ctx) = 0;
	//Will run once at game start, where you initialize things like hooks
	virtual void OnAttach() = 0;
	//Will run once on detach, used to clean up memory if needed
	virtual void OnDetach() = 0;
	//Will run every frame in the main update function in quests
	virtual void OnUpdateQuest() {};
	//WIll run every frame in the main update function in the lobby
	virtual void OnUpdateLobby() {};
	//Will run once the global ImGui Context has been initialized
	virtual void OnImGUIInit() = 0;
	
	//Linked to the main injected imgui context, can draw whatever here
	virtual void DrawUI() {};

};

