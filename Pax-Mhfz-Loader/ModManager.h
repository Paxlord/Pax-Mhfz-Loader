#pragma once
#include <Windows.h>
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>

#include "mod.h"

typedef Mod* ( *t_createMod)();
typedef void(*t_logthing)();

class ModManager
{
private:
	std::vector<Mod*> mod_list;
	static ModManager* instance;
	ModManager();
public:
	static ModManager* get_instance();
	void AttachAll();
	void DetachAll();
};

