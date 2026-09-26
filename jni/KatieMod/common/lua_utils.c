#include "lua_utils.h"
#include "../log.h"

QiVec2 knLuaToVec2(lua_State *L, int index) {
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

QiVec3 knLuaToVec3(lua_State *L, int index) {
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

QiColor knLuaToColor(lua_State *L, int index) {
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

void knLuaPushVec2(lua_State *L, QiVec2 v) {
	lua_newtable(L);
	lua_pushinteger(L, 1); lua_pushnumber(L, v.x); lua_settable(L, -3);
	lua_pushinteger(L, 2); lua_pushnumber(L, v.y); lua_settable(L, -3);
}

void knLuaCopyIndex(lua_State *L, int l, lua_State *M) {
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

/*
int knLuaCallByName(lua_State *L, const char *function_name) {
	lua_getglobal(L, function_name);
	
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 1);
		return 0;
	}
	
	if (lua_pcall(L, 0, 0, 0) == 0) {
		return 1;
	}
	else {
		LogE("Error in %s: %s", function_name, lua_tostring(L, -1));
		lua_pop(L, 1);
		return 0;
	}
}

int knLuaCallByNameBool(lua_State *L, const char *function_name) {
	lua_getglobal(L, function_name);
	
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 1);
		return 0;
	}
	
	if (lua_pcall(L, 0, 1, 0) == 0) {
		int value = lua_toboolean(L, -1);
		lua_pop(L, 1);
		return value;
	}
	else {
		LogE("Error in %s: %s", function_name, lua_tostring(L, -1));
		lua_pop(L, 1);
		return 0;
	}
}*/

static int knLuaCallV(lua_State *L, const char *func, int retvals, va_list v) {
	lua_getglobal(L, func);
	
	int is_func = lua_isfunction(L, -1);
	int arg_count = 0;
	
	while (1) {
		int type = va_arg(v, int);
		
		if (type == KAT_BOOL) {
			lua_pushboolean(L, va_arg(v, int));
		}
		else if (type == KAT_INT) {
			lua_pushinteger(L, va_arg(v, int));
		}
		else if (type == KAT_LONG) {
			lua_pushinteger(L, va_arg(v, long long));
		}
		else if (type == KAT_NUM) {
			lua_pushnumber(L, va_arg(v, double));
		}
		else if (type == KAT_STR) {
			lua_pushstring(L, va_arg(v, char *));
		}
		else if (type == KAT_PTR) {
			lua_pushlightuserdata(L, va_arg(v, void *));
		}
		else {
			break;
		}
		
		arg_count++;
	}
	
	if (!is_func) {
		lua_pop(L, 1 + arg_count);
		return -1;
	}
	
	return lua_pcall(L, arg_count, retvals, 0);
}

void knLuaCallVoid(lua_State *L, const char *func, ...) {
	va_list args;
	va_start(args, func);
	int result = knLuaCallV(L, func, 0, args);
	va_end(args);
	
	if (result == -1) {
		return;
	}
	else if (result) {
		LogE("Error in %s: %s", func, lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

int knLuaCallBool(lua_State *L, const char *func, ...) {
	va_list args;
	va_start(args, func);
	int result = knLuaCallV(L, func, 1, args);
	va_end(args);
	
	if (result == -1) {
		return 0;
	}
	else if (result) {
		LogE("Error in %s: %s", func, lua_tostring(L, -1));
		lua_pop(L, 1);
		return 0;
	}
	else {
		result = lua_toboolean(L, -1);
		lua_pop(L, 1);
		return result;
	}
}

int knPopBool(lua_State *L) {
	int b = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return b;
}
