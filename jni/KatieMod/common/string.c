#include <stdlib.h>
#include <string.h>

#include "lua_utils.h"
#include "../util.h"

int knRemoveSuffix(lua_State *script) {
	if (lua_gettop(script) != 2) {
		return luaL_error(script, "Wrong number of arguments");
	}
	
	size_t str_size;
	const char *str = lua_tolstring(script, 1, &str_size);
	
	size_t suf_size;
	const char *suf = lua_tolstring(script, 2, &suf_size);
	
	if (!str || !suf) {
		return luaL_error(script, "Invalid arguments");
	}
	
	if (str_size >= suf_size && !memcmp(&str[str_size - suf_size], suf, suf_size)) {
		lua_pushlstring(script, str, str_size - suf_size);
	}
	else {
		lua_pop(script, 1);
	}
	
	return 1;
}

int knRemovePrefix(lua_State *script) {
	if (lua_gettop(script) != 2) {
		return luaL_error(script, "Wrong number of arguments");
	}
	
	size_t str_size;
	const char *str = lua_tolstring(script, 1, &str_size);
	
	size_t pre_size;
	const char *pre = lua_tolstring(script, 2, &pre_size);
	
	if (!str || !pre) {
		return luaL_error(script, "Invalid arguments");
	}
	
	if (str_size >= pre_size && !memcmp(str, pre, pre_size)) {
		lua_pushlstring(script, str + pre_size, str_size - pre_size);
	}
	else {
		lua_pop(script, 1);
	}
	
	return 1;
}

static inline bool starts_with(const char *s, size_t sl, const char *w, size_t wl) {
	if (wl > sl) {
		return false;
	}
	
	return !memcmp(s, w, wl);
}

int knSplit(lua_State *script) {
	/**
	 * knSplit(string: string, separator: string, [removeEmpty: boolean], [maxSplit: integer]): table[string]
	 * 
	 * Split string by a given separator
	 */
	
	int top = lua_gettop(script);
	
	if ((top < 2) || (top > 4)) {
		return luaL_error(script, "Wrong number of arguments (%d)", top);
	}
	
	size_t str_size;
	const char *str = lua_tolstring(script, 1, &str_size);
	
	size_t sep_size;
	const char *sep = lua_tolstring(script, 2, &sep_size);
	
	const bool remove_empty = lua_toboolean(script, 3);
	
	size_t max_split = lua_tointeger(script, 4);
	
	if (!str || !sep) {
		return luaL_error(script, "Invalid arguments");
	}
	
	lua_newtable(script);
	
	size_t nsplit = 0;
	size_t last_sep_end = 0;
	
	// handle splits
	for (size_t i = 0; i < str_size;) {
		if (starts_with(str + i, str_size - i, sep, sep_size)) {
			const char *piece = str + last_sep_end;
			const size_t piece_len = i - last_sep_end;
			
			if (!remove_empty || piece_len) {
				lua_pushlstring(script, piece, piece_len);
				lua_rawseti(script, -2, nsplit + 1);
				nsplit++;
			}
			
			last_sep_end = i + sep_size;
			i = last_sep_end;
			
			if (max_split && nsplit == max_split) {
				break;
			}
		}
		else {
			i++;
		}
	}
	
	// handle final piece
	{
		const char *piece = str + last_sep_end;
		const size_t piece_len = str_size - last_sep_end;
		
		if (!remove_empty || piece_len) {
			lua_pushlstring(script, piece, piece_len);
			lua_rawseti(script, -2, nsplit + 1);
		}
	}
	
	return 1;
}

int knEnableString(lua_State *script) {
	knRegisterFunc(script, knRemoveSuffix);
	knRegisterFunc(script, knRemovePrefix);
	knRegisterFunc(script, knSplit);
	
	return 0;
}
