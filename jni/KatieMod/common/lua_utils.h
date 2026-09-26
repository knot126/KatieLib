#ifndef _LUA_UTILS_H_
#define _LUA_UTILS_H_

#include <stdarg.h>

#include "../smash_hit/smashhit.h"

#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"

QiVec2 knLuaToVec2(lua_State *L, int index);
QiVec3 knLuaToVec3(lua_State *L, int index);
QiColor knLuaToColor(lua_State *L, int index);
void knLuaPushVec2(lua_State *L, QiVec2 v);
void knLuaCopyIndex(lua_State *L, int l, lua_State *M);
void knLuaCallVoid(lua_State *L, const char *func, ...);
int knLuaCallBool(lua_State *L, const char *func, ...);
int knPopBool(lua_State *L);

enum {
	KAT_END = 0,
	KAT_BOOL = 1,
	KAT_INT = 2,
	KAT_NUM = 3,
	KAT_STR = 4,
	KAT_PTR = 5,
	KAT_LONG = 6,
};

#endif // _LUA_UTILS_H_
