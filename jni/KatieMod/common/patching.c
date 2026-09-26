/**
 * Memory patching utilities
 */

#include "lua_utils.h"
#include "../util.h"

static void *kt_checkaddr(lua_State *script, int idx) {
	void *addr = NULL;
	
	switch (lua_type(script, idx)) {
		case LUA_TSTRING: {
			addr = YipLookupSymbol(lua_tostring(script, idx));
			break;
		}
		case LUA_TNUMBER: {
			Leaf *leaf = YipGetLeafInstance();
			addr = LeafGetRealAddr(leaf, lua_tointeger(script, idx));
			break;
		}
		case LUA_TLIGHTUSERDATA: {
			addr = lua_touserdata(script, idx);
			break;
		}
		default: {
			luaL_error(script, "Invalid type of address specification");
			return NULL;
		}
	}
	
	if (!addr) {
		luaL_error(script, "The address specification resulted in an invalid pointer.");
		return NULL;
	}
	
	return addr;
}

int knPatch(lua_State *script) {
	/**
	 * (string) originalData = knPatch(addressSpec, (string) data)
	 */
	
	if (lua_gettop(script) < 2) {
		return luaL_error(script, "Not enough args");
	}
	
	void *addr = kt_checkaddr(script, 1);
	
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
	
	void *addr = kt_checkaddr(script, 1);
	
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
	
	void *base = kt_checkaddr(script, 1);
	
	for (size_t i = 2; i <= lua_gettop(script); i++) {
		base = *((void **)base) + lua_tointeger(script, i);
	}
	
	lua_pushlightuserdata(script, base);
	return 1;
}

#include "patch_insert.c"

int knInsertCode(lua_State *script) {
	/**
	 * knInsertCode(location: address, code: string): string
	 * 
	 */
	
	uint32_t *pc = kt_checkaddr(script, 1);
	
	size_t code_size;
	const void *code = lua_tolstring(script, 2, &code_size);
	
	uint32_t *orig = kt_insert_code(pc, code, code_size);
	
	lua_pushlstring(script, (void *)orig, 4);
	return 1;
}

int knEnablePatching(lua_State *script) {
	knRegisterFunc(script, knPatch);
	knRegisterFunc(script, knPeek);
	knRegisterFunc(script, knAddress);
	knRegisterFunc(script, knInsertCode);
	
	return 0;
}
