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

int knIsLittleEndian(lua_State *script) {
	/**
	 * (boolean) platformIsLittleEndian = knIsLittleEndian()
	 */
	
	union { int8_t a; int16_t b; } u;
	u.b = 1;
	lua_pushboolean(script, u.a);
	return 1;
}

int knEnablePack(lua_State *script) {
	knRegisterFunc(script, knPack);
	knRegisterFunc(script, knUnpack);
	// knRegisterFunc(script, knIsLittleEndian);
	
	return 0;
}
