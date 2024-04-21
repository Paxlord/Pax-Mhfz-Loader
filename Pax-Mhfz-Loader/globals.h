#pragma once

extern int mhfdll_addy;
extern const int VERSION;

inline int OffsetByDll(int addy) {
	return addy + mhfdll_addy;
}