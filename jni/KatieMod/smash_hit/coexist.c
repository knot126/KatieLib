/**
 * Enable built-in functions like init, draw, drawWorld, frame, and handleCommand
 * to be tables of functions instead of just single functions. This is intended
 * to allow libraries to add code on to these functions without worrying about
 * replacing them properly.
 */

#include "../util.h"
#include "../common/lua_utils.h"
#include <yiploader/yiploader.h>
#include "smashhit.h"

#define L (this->scriptInternal->state)
#define S(qs) ((qs)->data ? (qs)->data : (qs)->cached)

bool QiScript_call_hook(QiScript *this, QiString *functionName) {
	lua_getglobal(L, S(functionName));
	
	switch (lua_type(L, -1)) {
		case LUA_TFUNCTION: {
			bool result = false;
			
			if (lua_pcall(L, 0, 0, 0)) {
				LogE("Error in %s: %s", S(functionName), lua_tostring(L, -1));
				lua_pop(L, 1);
				result = false;
			}
			else {
				result = true;
			}
			
			return result;
		}
		case LUA_TTABLE: {
			lua_pushnil(L);
			
			while (lua_next(L, -2) != 0) {
				if (lua_pcall(L, 0, 0, 0)) {
					LogE("Error in %s (multicall): %s", S(functionName), lua_tostring(L, -1));
					lua_pop(L, 1);
				}
			}
			
			lua_pop(L, 1);
			
			return true;
		}
	}
	
	lua_pop(L, 1);
	return false;
}

bool QiScript_call_str_hook(QiScript *this, QiString *functionName, QiString *arg) {
	lua_getglobal(L, S(functionName));
	
	switch (lua_type(L, -1)) {
		case LUA_TFUNCTION: {
			bool result = false;
			
			lua_pushstring(L, S(arg));
			
			if (lua_pcall(L, 1, 0, 0)) {
				LogE("Error in %s: %s", S(functionName), lua_tostring(L, -1));
				lua_pop(L, 1);
				result = false;
			}
			else {
				result = true;
			}
			
			return result;
		}
		case LUA_TTABLE: {
			lua_pushnil(L);
			
			while (lua_next(L, -2) != 0) {
				lua_pushstring(L, S(arg));
				
				if (lua_pcall(L, 1, 0, 0)) {
					LogE("Error in %s (multicall): %s", S(functionName), lua_tostring(L, -1));
					lua_pop(L, 1);
				}
			}
			
			lua_pop(L, 1);
			
			return true;
		}
	}
	
	lua_pop(L, 1);
	return false;
}

static inline bool isJustAFunctionCall(const char * const s) {
	/**
	 * Return true if this matches the regex equivlent "[a-z]*\(\)"
	 */
	
	const int len = strlen(s);
	
	if (len < 2) {
		return false;
	}
	else if (strcmp(s + len - 2, "()")) {
		for (int i = 0; i < len - 2; i++) {
			if (!(s[i] >= 'a' && s[i] <= 'z')) {
				return false;
			}
		}
		
		return true;
	}
	else {
		return false;
	}
}

bool QiScript_execute_hook(QiScript *this, QiString *code) {
	/**
	 * Unforunately the game sometimes does things like
	 * 
	 *     script->execute("init()")
	 * 
	 * instead of the more resonable
	 * 
	 *     script->call("init")
	 * 
	 * so we need to account for this edge case.
	 */
	
	const char * const codeStr = S(code);
	const int codeStrLen = strlen(codeStr);
	
	if (isJustAFunctionCall(S(code))) {
		char buf[codeStrLen - 1];
		strncpy(buf, codeStr, codeStrLen - 2);
		QiString s = {.data = buf};
		return QiScript_call_hook(this, &s);
	}
	else {
		if (luaL_loadstring(L, S(code))) {
			return false;
		}
		
		if (lua_pcall(L, 0, 0, 0)) {
			lua_pop(L, 1);
			return false;
		}
		else {
			return true;
		}
	}
}

bool QiScript_hasFunction_hook(QiScript *this, QiString *functionName) {
	/**
	 * Since there can now be tables of functions, count them as functions too.
	 */
	
	lua_getglobal(L, S(functionName));
	int type = lua_type(L, -1);
	lua_pop(L, 1);
	return (type == LUA_TFUNCTION) || (type == LUA_TTABLE);
}

#undef S
#undef L

const char *KNInitCoexistance(void) {
	YipHookFunction("_ZN8QiScript4callERK8QiString", QiScript_call_hook, true);
	YipHookFunction("_ZN8QiScript4callERK8QiStringS2_", QiScript_call_str_hook, true);
	YipHookFunction("_ZN8QiScript7executeERK8QiString", QiScript_execute_hook, true);
	YipHookFunction("_ZN8QiScript11hasFunctionERK8QiString", QiScript_hasFunction_hook, true);
	return NULL;
}
