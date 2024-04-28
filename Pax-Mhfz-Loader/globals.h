#pragma once

inline int mhfdll_addy = 0;
inline bool NO_LGE = false;
inline const int VERSION = 3;

inline int OffsetByDll(int addy) {
	return addy + mhfdll_addy;
}