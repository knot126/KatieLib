/**
 * Inter-script communication
 */

#include <yiploader/yiploader.h>
#include "../util.h"
#include "lua_utils.h"
#include "smashhit.h"

// const char * const KN_MENU = "com.mediocre.smashhit.menu";
// const char * const KN_MOVIE = "com.mediocre.smashhit.movie";
// const char * const KN_HUD = "com.mediocre.smashhit.hud";

enum {
	KN_MENU = 1,
	KN_MOVIE = 2,
	KN_HUD = 3,
};

int knCall(lua_State *L) {
	/**
	 * [result1, [result2, [...]]] = knCall(script, function, [...])
	 * 
	 * Call a function in the given script and return the results.
	 * 
	 * :param script: may be one of the constants KN_MENU, KN_MOVIE, KN_HUD or
	 *                a light userdata pointing to a lua_State obtained with
	 *                knOwnHandle().
	 */
	
	const int nargs = lua_gettop(L) - 2;
	const char * const function_name = lua_tostring(L, 2);
	
	if (!function_name) {
		return luaL_error(L, "function name must be a string");
	}
	
	lua_State *M = NULL;
	
	switch (lua_type(L, 1)) {
		case LUA_TNUMBER: {
			const int script_id = lua_tointeger(L, 1);
			
			if (script_id < 1 || script_id > 3) {
				return luaL_error(L, "scene index %d does not refer to a valid scene", script_id);
			}
			
			const Scene * const scene = (&gGame->menuScene)[script_id - 1];
			
			if (!scene) {
				return luaL_error(L, "scene index %d is not loaded", script_id);
			}
			
			M = *scene->script.state;
			break;
		}
		case LUA_TLIGHTUSERDATA: {
			M = lua_touserdata(L, 1);
			break;
		}
		default: {
			return luaL_error(L, "script must be a light userdata pointing to a script or a scene enum (one of KN_MENU, KN_MOVIE, KN_HUD)");
			break;
		}
	}
	
	// We'll be using this to find how many return values there are and restore
	// the old stack top
	const int old_top = lua_gettop(M);
	
	// get function to call
	lua_getglobal(M, function_name);
	
	if (!lua_isfunction(M, -1)) {
		lua_pop(M, 1);
		return luaL_error(L, "inter-script call to script <%p> failed because %s is not a function", M, function_name);
	}
	
	// Copy call args
	for (int i = 0; i < nargs; i++) {
		knLuaCopyIndex(L, 3 + i, M);
	}
	
	// Call it
	if (lua_pcall(M, nargs, LUA_MULTRET, 0)) {
		knLuaCopyIndex(M, -1, L);
		lua_settop(M, old_top);
		return lua_error(L);
	}
	else {
		const int nrets = lua_gettop(M) - old_top;
		
		for (int i = 0; i < nrets; i++) {
			knLuaCopyIndex(M, old_top + 1 + i, L);
		}
		
		lua_settop(M, old_top);
		
		return nrets;
	}
}

int knOwnHandle(lua_State *L) {
	lua_pushlightuserdata(L, L);
	return 1;
}

int knEnableIsc(lua_State *L) {
	knRegisterFunc(L, knCall);
	knRegisterFunc(L, knOwnHandle);
	
	knLuaPushEnum(L, KN_MENU);
	knLuaPushEnum(L, KN_MOVIE);
	knLuaPushEnum(L, KN_HUD);
	
	return 0;
}
