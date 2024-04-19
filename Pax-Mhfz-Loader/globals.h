#pragma once

extern int mhfdll_addy;

inline int OffsetByDll(int addy) {
	return addy + mhfdll_addy;
}