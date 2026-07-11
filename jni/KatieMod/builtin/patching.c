/**
 * Memory patching utilities
 */

#include "lua_utils.h"
#include "../util.h"

int knPatch(lua_State *script) {
	/**
	 * (bool) success = knPatch(addressSpec, (string) data)
	 */
	
	if (lua_gettop(script) < 2) {
		return luaL_error(script, "Not enough args");
	}
	
	void *addr = NULL;
	
	switch (lua_type(script, 1)) {
		case LUA_TSTRING: {
			addr = YipLookupSymbol(lua_tostring(script, 1));
			break;
		}
		case LUA_TNUMBER: {
			Leaf *leaf = YipGetLeafInstance();
			addr = LeafGetRealAddr(leaf, lua_tointeger(script, 1));
			break;
		}
		case LUA_TLIGHTUSERDATA: {
			addr = lua_touserdata(script, 1);
			break;
		}
		default: {
			return luaL_error(script, "Invalid address specification");
		}
	}
	
	if (!addr) {
		return luaL_error(script, "Tried to patch a NULL address! (This is probably because a symbol lookup failed or the virtual address doesn't exist in the game binary)");
	}
	
	if (lua_type(script, 2) != LUA_TSTRING) {
		return luaL_error(script, "Data is not a string: use knPack() to convert numbers first");
	}
	
	size_t size;
	const char *data = lua_tolstring(script, 2, &size);
	
	// Push original data
	lua_pushlstring(script, addr, size);
	
	// Copy new data
	memcpy(addr, data, size);
	
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
	
	void *addr = NULL;
	
	switch (lua_type(script, 1)) {
		case LUA_TSTRING: {
			addr = YipLookupSymbol(lua_tostring(script, 1));
			break;
		}
		case LUA_TNUMBER: {
			Leaf *leaf = YipGetLeafInstance();
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

int knAddress(lua_State *script) {
	/**
	 * knAddress(base, [offset...]): lightuserdata
	 * 
	 * Return a lightuserdata for a (mostly) arbitrary address. The base
	 * address may be either a number (interpreted as an offset from the start
	 * of the binary) or a string (interpreted as a symbol name). This is the
	 * initial address that any offsets will work with.
	 * 
	 * For each offset (an integer), the current base address is dereferenced,
	 * then the offset is added in the order the offsets were passed to the
	 * function. So, for example, this:
	 * 
	 *     knAddress("gGame", 0x60, 0x8bc)
	 * 
	 * Is the same as writing:
	 * 
	 *     (void *)(*((void **)(*(void **)YipLookupSymbol("gGame")) + 0x60) + 0x8bc)
	 *     // Can imagine as: gGame->0x60->0x8bc
	 */
	
	void *base = NULL;
	
	// Interpret base address
	switch (lua_type(script, 1)) {
		case LUA_TSTRING: {
			base = YipLookupSymbol(lua_tostring(script, 1));
			break;
		}
		case LUA_TNUMBER: {
			Leaf *leaf = YipGetLeafInstance();
			base = LeafGetRealAddr(leaf, lua_tointeger(script, 1));
			break;
		}
		case LUA_TLIGHTUSERDATA: {
			base = lua_touserdata(script, 1);
			break;
		}
		default: {
			return luaL_error(script, "Invalid type of base address");
		}
	}
	
	if (!base) {
		return luaL_error(script, "Base address is NULL! For base symbols, check your spelling. For offsets into a binary, check that the virtual address exists within the binary itself.");
	}
	
	for (size_t i = 2; i <= lua_gettop(script); i++) {
		base = *((void **)base) + lua_tointeger(script, i);
	}
	
	lua_pushlightuserdata(script, base);
	return 1;
}

int knEnablePatching(lua_State *script) {
	knRegisterFunc(script, knPatch);
	knRegisterFunc(script, knPeek);
	knRegisterFunc(script, knAddress);
	
	return 0;
}
