#!/usr/bin/env python3
import os
import shutil
import sys
from datetime import datetime
from pathlib import Path

if "--help" in sys.argv:
	print(f"""{sys.argv[0]} [OPTIONS] -- build KatieMod

Options:
    --no-regen-header   Do not regenerate headers
    --package           Package the final libs into a zip.
    --upgrade           Automatically upgrade apk open in apk editor studio
""")
	sys.exit()

game = "smashhit" #if "--game" not in sys.argv else sys.argv[sys.argv.index("--game")+1]

if "--no-regen-header" not in sys.argv:
	if "--package" in sys.argv:
		version = Path("RELEASE").read_text().strip()
		new_data = f"#define SHIM_VERSION \"{version}\"\n"
		Path("jni/KatieMod/version.h").write_text(new_data)
	
	with open("jni/KatieMod/builtin/modules.txt", "r") as f:
		enum = ""
		enables = ""
		pushenum = ""
		i = 0
		
		for line in f.readlines():
			name = line.strip().split()[0]
			
			if game in line:
				enum += f"\tKN_{name.upper()}_BIT = (1 << {i}),\n"
				enables += f"\tint knEnable{name}(lua_State *script);\\\n"
				enables += f"\tif ((gDisabledModules & KN_{name.upper()}_BIT) == 0) {{ knEnable{name}(script); }}\\\n"
				pushenum += f"\tknLuaPushEnum(script, KN_{name.upper()}_BIT);\\\n"
				i += 1
		
		Path("jni/KatieMod/builtin/enablement.h").write_text(f"""enum {{
{enum}}};

#define KNSHIM_ENABLE() \\
{enables}

#define KNSHIM_PUSH_ENABLE_ENUM() \\
{pushenum}""")

status = os.system(f"ndk-build")

if not status:
	# for arch in {"armeabi-v7a", "arm64-v8a"}:
	# 	try:
	# 		os.remove(f"libs/{arch}/libYipLoader.so")
	# 	except:
	# 		pass
	
	if "--upgrade" in sys.argv:
		apks = os.listdir("/tmp/apk-editor-studio/apk")
		
		if len(apks) > 0:
			apk_path = f"/tmp/apk-editor-studio/apk/{apks[0]}"
			print(f"Upgrade apk at {apk_path}")
			shutil.copytree("./libs", f"{apk_path}/lib", dirs_exist_ok=True)
		else:
			print(f"No APKs to upgrade")
	
	if "--package" in sys.argv:
		version = Path("RELEASE").read_text().strip()
		print(f"Package release as version {version}...")
		shutil.make_archive(f"katiemod-r{version}-{game}-libs", "zip", "./libs")
