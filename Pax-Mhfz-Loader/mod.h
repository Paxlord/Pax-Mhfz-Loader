#pragma once
#include <Windows.h>

class Mod {
public:
	virtual void ApplyHooks(DWORD dll_addy) = 0;
	virtual void RemoveHook() = 0;
	virtual void DrawUI() {

	}
};