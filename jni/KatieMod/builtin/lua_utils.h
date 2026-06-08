#ifndef _LUA_UTILS_H_
#define _LUA_UTILS_H_

#include "smashhit.h"

#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"

static inline QiVec2 knLuaToVec2(lua_State *L, int index) {
	QiVec2 v;
	
	if (lua_istable(L, index)) {
		lua_pushinteger(L, 1); lua_gettable(L, index); v.x = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_pushinteger(L, 2); lua_gettable(L, index); v.y = lua_tonumber(L, -1); lua_pop(L, 1);
	}
	else {
		v = (QiVec2) {0.0, 0.0};
	}
	
	return v;
}

static inline QiVec3 knLuaToVec3(lua_State *L, int index) {
	QiVec3 v;
	
	if (lua_istable(L, index)) {
		lua_pushinteger(L, 1); lua_gettable(L, index); v.x = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_pushinteger(L, 2); lua_gettable(L, index); v.y = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_pushinteger(L, 3); lua_gettable(L, index); v.z = lua_tonumber(L, -1); lua_pop(L, 1);
	}
	else {
		v = (QiVec3) {0.0, 0.0, 0.0};
	}
	
	return v;
}

static inline QiColor knLuaToColor(lua_State *L, int index) {
	QiColor v;
	
	if (lua_istable(L, index)) {
		lua_pushinteger(L, 1); lua_gettable(L, index); v.r = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_pushinteger(L, 2); lua_gettable(L, index); v.g = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_pushinteger(L, 3); lua_gettable(L, index); v.b = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_pushinteger(L, 4); lua_gettable(L, index); v.a = lua_tonumber(L, -1); lua_pop(L, 1);
	}
	else {
		v = (QiColor) {0.0, 0.0, 0.0, 1.0};
	}
	
	return v;
}

static inline void knLuaPushVec2(lua_State *L, QiVec2 v) {
	lua_newtable(L);
	lua_pushinteger(L, 1); lua_pushnumber(L, v.x); lua_settable(L, -3);
	lua_pushinteger(L, 2); lua_pushnumber(L, v.y); lua_settable(L, -3);
}

static inline void knLuaCopyIndex(lua_State *L, int l, lua_State *M) {
	const int type = lua_type(L, l);
	
	lua_checkstack(M, 1);
	
	switch (type) {
		case LUA_TNIL:
		case LUA_TFUNCTION:
		case LUA_TUSERDATA:
		case LUA_TTHREAD:
			lua_pushnil(M);
			break;
		case LUA_TNUMBER:
			lua_pushnumber(M, lua_tonumber(L, l));
			break;
		case LUA_TBOOLEAN:
			lua_pushboolean(M, lua_toboolean(L, l));
			break;
		case LUA_TSTRING: {
			size_t size;
			const char *data = lua_tolstring(L, l, &size);
			lua_pushlstring(M, data, size);
			break;
		}
		case LUA_TTABLE:
			lua_newtable(M);
			lua_pushnil(L);
			while (lua_next(L, l)) {
				knLuaCopyIndex(L, -2, M); // key
				knLuaCopyIndex(L, -1, M); // value
				lua_settable(M, -3);
				lua_pop(L, 1);
			}
			break;
		case LUA_TLIGHTUSERDATA:
			lua_pushlightuserdata(M, lua_touserdata(L, l));
			break;
	}
}

#endif // _LUA_UTILS_H_
