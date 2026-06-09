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
	
	lua_pushboolean(script, data && YipPatch(vaddr, (YipBuffer) {size, data}));
	return 1;
}

int knEnablePatching(lua_State *script) {
	knRegisterFunc(script, knPatch);
	
	return 0;
}
