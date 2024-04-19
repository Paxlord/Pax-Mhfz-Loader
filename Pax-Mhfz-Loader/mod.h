#pragma once
#include <Windows.h>

class Mod {
public:
	const char* name = "";
	const char* display_name = "";

	virtual void Attach() = 0;
	virtual void Detach() = 0;
	virtual void DrawUI() {};
	virtual void UpdateQuest() {};
	virtual void UpdateLobby() {};
};