#include "lua_utils.h"

#include "../util.h"
#include "smashhit.h"

#define TICK_TABLE_NAME "tick_callbacks"

void (*Script_tick)(Script *this, bool shouldCallFrame);

void Script_tick_hook(Script *this, bool shouldCallFrame) {
	lua_State *L = *this->script->state;
	
	lua_getglobal(L, TICK_TABLE_NAME);
	
	// Call tick callbacks, if available
	if (lua_type(L, -1) == LUA_TTABLE) {
		lua_pushnil(L);
		
		while (lua_next(L, -2)) {
			if (lua_isfunction(L, -1)) {
				if (lua_pcall(L, 0, 0, 0)) {
					const char *error_msg = lua_tostring(L, -1);
					LogW("Error while calling script tick callback: %s", error_msg);
					lua_pop(L, 1);
				}
			}
			
			lua_pop(L, 1);
		}
	}
	
	lua_pop(L, 1);
	
	Script_tick(this, shouldCallFrame);
}

const char *KNInitTick(void) {
	Script_tick = YipHookFunction("_ZN6Script4tickEb", Script_tick_hook, false);
	return NULL;
}

int knEnableTick(lua_State *script) {
	lua_newtable(script);
	lua_setglobal(script, TICK_TABLE_NAME);
	return 0;
}
