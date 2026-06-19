/**
 * Memory patching utilities
 */

#include "lua_utils.h"
#include "../util.h"

int knPatch(lua_State *script) {
	/**
	 * (bool) success = knPatch((int) vaddr, (string) data)
	 */
	
	if (lua_gettop(script) < 2) {
		return luaL_error(script, "Not enough args");
	}
	
	size_t vaddr = lua_tointeger(script, 1);
	
	size_t size;
	const char *data = lua_tolstring(script, 2, &size);
	
	if (!data) {
		return luaL_error(script, "Data is not a string or couldn't be converted to one; use knPack for number values");
	}
	
	YipBuffer orig;
	
	if (YipPatchv2(vaddr, (YipBuffer) {size, (uint8_t *)data}, &orig)) {
		lua_pushlstring(script, (const char*)orig.data, orig.size);
		YipDestroyBuffer(orig);
	}
	else {
		return luaL_error(script, "Patching failed!");
	}
	
	return 1;
}

int knEnablePatching(lua_State *script) {
	knRegisterFunc(script, knPatch);
	
	return 0;
}
