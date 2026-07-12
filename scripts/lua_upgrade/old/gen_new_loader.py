#!/usr/bin/env python3

import re
import os
from sys import argv
from pathlib import Path

RE_LUA_FUNC = re.compile(r"\((luaL?_[_A-Za-z0-9]+)\)")

LOADER_EXCLUDES = {
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

syms = []

for filename in sorted(os.listdir(argv[1])):
	with open(f"{argv[1]}/{filename}", "r") as f:
		with open(f"{filename[:-2]}-{argv[1]}.h", "w") as h:
			# Loader shit
			macro_continued = False
			
			for line in f:
				if ("#" not in line) and ("typedef" not in line) and not macro_continued:
					m = RE_LUA_FUNC.search(line)
					
					if m and m[1] not in LOADER_EXCLUDES:
						syms.append(m[1])
						
						fptr = line.replace(f"({m[1]})", f"(*{m[1]})")
						
						if fptr == line:
							fptr = line.replace(m[1], f"(*{m[1]})")
						
						h.write(fptr)
					else:
						h.write(line)
				else:
					h.write(line)

RE_LUA_ANY = re.compile(r"[^a-zA-Z0-9_]((?:lua|LUA)[a-zA-Z0-9_]+)[^a-zA-Z0-9_]")
RENAME_EXCLUDES = {
	"lua_State",
}

# Go back and append _new to nearly everything (with few exceptions)
for filename in sorted(os.listdir(argv[1])):
	p = Path(f"{filename[:-2]}-{argv[1]}.h")
	header = p.read_text()
	
	syms = set()
	
	for m in RE_LUA_ANY.finditer(header):
		syms.add(m[1])
	
	for sym in (sorted(syms)):
		if sym not in RENAME_EXCLUDES:
			header = header.replace(sym, sym + ("_NEW" if sym[0] == "L" else "_new"))
	
	p.write_text(header)
