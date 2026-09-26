/**
 * Load, clear and trigger quick saves from lua
 */

#include "../common/lua_utils.h"
#include "smashhit.h"
#include "../util.h"

void (*Player_quickSave)(Player *this);

int knQuickSave(lua_State *script) {
	if (!Player_quickSave) {
		Player_quickSave = YipLookupSymbol("_ZN6Player9quickSaveEv");
	}
	
	Player_quickSave(gGame->player);
	
	return 0;
}

bool (*Player_quickLoad)(Player *this);

int knQuickLoad(lua_State *script) {
	if (!Player_quickLoad) {
		Player_quickLoad = YipLookupSymbol("_ZN6Player9quickLoadEv");
	}
	
	lua_pushboolean(script, Player_quickLoad(gGame->player));
	
	return 1;
}

void (*Player_clearQuickSave)(Player *this);

int knQuickClear(lua_State *script) {
	if (!Player_clearQuickSave) {
		Player_clearQuickSave = YipLookupSymbol("_ZN6Player14clearQuickSaveEv");
	}
	
	Player_clearQuickSave(gGame->player);
	
	return 0;
}

int knEnableQuicksave(lua_State *script) {
	knRegisterFunc(script, knQuickSave);
	knRegisterFunc(script, knQuickLoad);
	knRegisterFunc(script, knQuickClear);
	
	return 0;
}
