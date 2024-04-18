#pragma once
#include <Windows.h>
#include <vector>
#include <iostream>

#include "mod.h"

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
	void makeModList();

};

