#!/usr/bin/env python3

import re
import os
from sys import argv

relua = re.compile(r"lua[_A-Za-z0-9]+")

excludes = {
	"luaL_Reg",
	"lua_CFunction",
	"lua_Number",
	"lua_Integer",
	"lua_State",
	"luaL_Buffer",
	"lua_Hook",
	"lua_Debug",
	"lualib",
}

compat_shims = {
	"lua_getfield",
	"lua_load",
	"lua_resume",
}

replacements = {
	"lua_getfenv",
	"lua_setfenv",
	"lua_rawlen",
	"lua_cpcall",
}

def gen_hook(symbol):
	print(f"""	sym = dlsym(lualib, "{symbol}");
	
	if (sym) {{
		if (!YipHookFunction("{symbol}", sym, true)) {{
			LogW("lua upgrade: failed to hook {symbol}");
		}}
	}}
	else {{
		LogW("lua upgrade: could not find symbol %s, not hooking", "{symbol}");
	}}
	""")

def gen_hook_shimmed(symbol):
	print(f"""	{symbol}_new = dlsym(lualib, "{symbol}");
	
	if ({symbol}_new) {{
		if (!YipHookFunction("{symbol}", {symbol}_shim, true)) {{
			LogW("lua upgrade: failed to hook %s (shim)", "{symbol}");
		}}
	}}
	else {{
		LogW("lua upgrade: could not find symbol {symbol}, not hooking");
	}}
	""")

def gen_hook_replaced(symbol):
	print(f"""if (!YipHookFunction("{symbol}", {symbol}_shim, true)) {{
		LogW("lua upgrade: failed to hook %s (replacement)", "{symbol}");
	}}
	""")

print("""static void lua_upgrade_hook_symbols(void) {
	void *sym;
	""")

for filename in sorted(os.listdir(argv[1])):
	with open(f"{argv[1]}/{filename}", "r") as f:
		for line in f:
			if ("#" not in line) and ("typedef" not in line):
				m = relua.search(line)
				
				if m and m[0] not in excludes:
					# print(m[0])
					gen_hook(m[0])

print("""}""")
