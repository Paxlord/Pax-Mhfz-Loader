#pragma once
#include "color.hpp"

#define DEBUG_LOG(msg) (std::cout << dye::yellow("[MODLOADER] ") << msg << std::endl)

inline int mhfdll_addy = 0;
inline bool NO_LGE = false;
inline const int VERSION = 3;

inline int OffsetByDll(int addy) {
	return addy + mhfdll_addy;
}