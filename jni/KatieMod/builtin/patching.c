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
	
	if (lua_type(script, 2) != LUA_TSTRING) {
		return luaL_error(script, "Data is not a string: use knPack() to convert numbers first");
	}
	
	size_t size;
	const char *data = lua_tolstring(script, 2, &size);
	
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

int knPeek(lua_State *script) {
	/**
	 * Return `size` bytes at the address of the given symbol, virtual
	 * address, or raw address.
	 */
	
	if (lua_gettop(script) < 2) {
		return luaL_error(script, "Not enough args");
	}
	
	Leaf *leaf = YipGetLeafInstance();
	void *addr = NULL;
	
	switch (lua_type(script, 1)) {
		case LUA_TSTRING: {
			addr = YipLookupSymbol(lua_tostring(script, 1));
			break;
		}
		case LUA_TNUMBER: {
			addr = LeafGetRealAddr(leaf, lua_tointeger(script, 1));
			break;
		}
		case LUA_TLIGHTUSERDATA: {
			addr = lua_touserdata(script, 1);
			break;
		}
		default: {
			return luaL_error(script, "First argument must be a string (symbol), number (virtual address), or light userdata (raw address)");
		}
	}
	
	size_t size = lua_tointeger(script, 2);
	
	lua_pushlstring(script, addr, size);
	return 1;
}

int knEnablePatching(lua_State *script) {
	knRegisterFunc(script, knPatch);
	knRegisterFunc(script, knPeek);
	
	return 0;
}
