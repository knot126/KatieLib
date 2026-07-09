#include <stdlib.h>

#include "lua_utils.h"
#include "../util.h"

#define PACK_TYPE(STRNAME, TYPE, TYPECLASS) else if (!strcmp(typename, STRNAME)) { \
		TYPE n = lua_to ## TYPECLASS(script, 2); \
		lua_pushlstring(script, (const char *) &n, sizeof n); \
	}

int knPack(lua_State *script) {
	/**
	 * (string) data = knPack((string) type, (number|integer) value)
	 * 
	 * Convert a integer or floating point value to bytes in native endian form
	 */
	
	int atype = lua_type(script, 1);
	
	if (atype == LUA_TSTRING) {
		const char *typename = lua_tostring(script, 1);
		
		if (false) {}
		PACK_TYPE("float", float, number)
		PACK_TYPE("double", double, number)
		PACK_TYPE("char", int8_t, integer)
		PACK_TYPE("bool", int8_t, integer)
		PACK_TYPE("short", int16_t, integer)
		PACK_TYPE("int", int32_t, integer)
		PACK_TYPE("long", int64_t, integer)
		PACK_TYPE("pointer", void *, userdata)
		else {
			luaL_error(script, "Invalid pack type string: '%s'", typename);
		}
	}
	else if (atype == LUA_TNIL || atype == LUA_TNONE) {
		luaL_error(script, "Cannot pack nil (or none) value; you probably forgot to pass any arguments");
	}
	else {
		luaL_error(script, "Pack type expects a string, not a value");
	}
	
	return 1;
}

#define UNPACK_TYPE(NAME, TYPE, TYPECLASS) if (!strcmp(target_type, NAME)) { \
		if (in_size == sizeof(TYPE)) { \
			lua_push ## TYPECLASS(script, ( *(TYPE *)in_data )); \
		} \
		else { \
			return luaL_error(script, "Cannot convert %d byte buffer to %d byte " NAME, in_size, sizeof(TYPE)); \
		} \
	}

int knUnpack(lua_State *script) {
	/**
	 * (integer|number) value = knUnpack((string) type, (string) data)
	 */
	
	const char * const target_type = lua_tostring(script, 1);
	size_t in_size;
	const char * const in_data = lua_tolstring(script, 2, &in_size);
	
	if (!target_type) {
		return luaL_error(script, "Target type not provided");
	}
	
	if (!in_data) {
		return luaL_error(script, "Input data not provided");
	}
	
	if (false) {}
	UNPACK_TYPE("float", float, number)
	UNPACK_TYPE("double", double, number)
	UNPACK_TYPE("char", int8_t, integer)
	UNPACK_TYPE("bool", int8_t, integer)
	UNPACK_TYPE("short", int16_t, integer)
	UNPACK_TYPE("int", int32_t, integer)
	UNPACK_TYPE("long", int64_t, integer)
	UNPACK_TYPE("pointer", void *, lightuserdata)
	else {
		luaL_error(script, "'%s' is not a supported unpack data type", target_type);
	}
	
	return 1;
}

#define IS_HEXDIGIT(CH) ( ((CH) >= '0' && (CH) <= '9') || ((CH) >= 'a' && (CH) <= 'f') || ((CH) >= 'A' && (CH) <= 'F') )

static inline uint8_t hex2nibble(char ch) {
	if (ch >= '0' && ch <= '9') return ch - '0';
	if (ch >= 'a' && ch <= 'f') return ch - 'a' + 0xa;
	if (ch >= 'A' && ch <= 'F') return ch - 'A' + 0xA;
	return 0;
}

int knHexToBin(lua_State *script) {
	/**
	 * (boolean) data = knHexToBin(hexdata)
	 * 
	 * Convert hex data (potentially with spaces or other useless chars) to
	 * binary data
	 */
	
	const char * const hex = lua_tostring(script, 1);
	
	size_t hexdigits = 0;
	
	// Estimate size of final data
	for (size_t i = 0; i < strlen(hex); i++) {
		if (IS_HEXDIGIT(hex[i])) {
			hexdigits += 1;
		}
	}
	
	// That is not a very nice size
	if (hexdigits & 1) {
		return luaL_error(script, "Number of hex digits must be even");
	}
	
	hexdigits >>= 1;
	
	// Allocate temp buffer
	uint8_t *data = malloc(hexdigits);
	
	if (!data) {
		return luaL_error(script, "Failed to allocate buffer for hex to binary conversion");
	}
	
	// Copy data
	size_t data_loc = 0;
	
	for (size_t i = 0; i < strlen(hex); i++) {
		if (IS_HEXDIGIT(hex[i])) {
			if (data_loc & 1) {
				data[data_loc >> 1] <<= 4;
				data[data_loc >> 1] |= hex2nibble(hex[i]);
			}
			else {
				data[data_loc >> 1] = hex2nibble(hex[i]);
			}
			
			data_loc += 1;
		}
	}
	
	// Push it
	lua_pushlstring(script, (void *) data, hexdigits);
	free(data);
	return 1;
}

static inline char nibble2hex(uint8_t nib) {
	if (nib <= 9) return '0' + nib;
	return 'a' + nib - 10;
}

int knBinToHex(lua_State *script) {
	size_t size;
	const uint8_t *data = (void *) lua_tolstring(script, 1, &size);
	
	if (!data) {
		return luaL_error(script, "Data is NULL");
	}
	
	// Create temp buffer
	char *hex = malloc(size << 1);
	
	if (!hex) {
		return luaL_error(script, "Allocation error");
	}
	
	for (size_t i = 0; i < size; i++) {
		hex[(i << 1)] = nibble2hex(data[i] >> 4);
		hex[(i << 1) | 1] = nibble2hex(data[i] & 0xf);
	}
	
	// Yay result
	lua_pushlstring(script, hex, size << 1);
	free(hex);
	return 1;
}

int knEnablePack(lua_State *script) {
	knRegisterFunc(script, knPack);
	knRegisterFunc(script, knUnpack);
	
	knRegisterFunc(script, knHexToBin);
	knRegisterFunc(script, knBinToHex);
	
	return 0;
}
