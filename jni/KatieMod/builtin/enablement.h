enum {
	KN_LOG_BIT = (1 << 0),
	KN_PATCHING_BIT = (1 << 1),
	KN_HTTP_BIT = (1 << 2),
	KN_SYSTEM_BIT = (1 << 3),
	KN_STRING_BIT = (1 << 4),
	KN_REGISTRY_BIT = (1 << 5),
	KN_DATABASE_BIT = (1 << 6),
	KN_PROPERTIES_BIT = (1 << 7),
	KN_FILE_BIT = (1 << 8),
	KN_GAMECTL_BIT = (1 << 9),
	KN_OVERLAY_BIT = (1 << 10),
	KN_SHADERS_BIT = (1 << 11),
	KN_INPUT_BIT = (1 << 12),
	KN_DRAW_BIT = (1 << 13),
	KN_PACK_BIT = (1 << 14),
	KN_ISC_BIT = (1 << 15),
	KN_UDP_BIT = (1 << 16),
};

#define KNSHIM_ENABLE() \
	int knEnableLog(lua_State *script);\
	if ((gDisabledModules & KN_LOG_BIT) == 0) { knEnableLog(script); }\
	int knEnablePatching(lua_State *script);\
	if ((gDisabledModules & KN_PATCHING_BIT) == 0) { knEnablePatching(script); }\
	int knEnableHttp(lua_State *script);\
	if ((gDisabledModules & KN_HTTP_BIT) == 0) { knEnableHttp(script); }\
	int knEnableSystem(lua_State *script);\
	if ((gDisabledModules & KN_SYSTEM_BIT) == 0) { knEnableSystem(script); }\
	int knEnableString(lua_State *script);\
	if ((gDisabledModules & KN_STRING_BIT) == 0) { knEnableString(script); }\
	int knEnableRegistry(lua_State *script);\
	if ((gDisabledModules & KN_REGISTRY_BIT) == 0) { knEnableRegistry(script); }\
	int knEnableDatabase(lua_State *script);\
	if ((gDisabledModules & KN_DATABASE_BIT) == 0) { knEnableDatabase(script); }\
	int knEnableProperties(lua_State *script);\
	if ((gDisabledModules & KN_PROPERTIES_BIT) == 0) { knEnableProperties(script); }\
	int knEnableFile(lua_State *script);\
	if ((gDisabledModules & KN_FILE_BIT) == 0) { knEnableFile(script); }\
	int knEnableGamectl(lua_State *script);\
	if ((gDisabledModules & KN_GAMECTL_BIT) == 0) { knEnableGamectl(script); }\
	int knEnableOverlay(lua_State *script);\
	if ((gDisabledModules & KN_OVERLAY_BIT) == 0) { knEnableOverlay(script); }\
	int knEnableShaders(lua_State *script);\
	if ((gDisabledModules & KN_SHADERS_BIT) == 0) { knEnableShaders(script); }\
	int knEnableInput(lua_State *script);\
	if ((gDisabledModules & KN_INPUT_BIT) == 0) { knEnableInput(script); }\
	int knEnableDraw(lua_State *script);\
	if ((gDisabledModules & KN_DRAW_BIT) == 0) { knEnableDraw(script); }\
	int knEnablePack(lua_State *script);\
	if ((gDisabledModules & KN_PACK_BIT) == 0) { knEnablePack(script); }\
	int knEnableIsc(lua_State *script);\
	if ((gDisabledModules & KN_ISC_BIT) == 0) { knEnableIsc(script); }\
	int knEnableUdp(lua_State *script);\
	if ((gDisabledModules & KN_UDP_BIT) == 0) { knEnableUdp(script); }\


#define KNSHIM_PUSH_ENABLE_ENUM() \
	knLuaPushEnum(script, KN_LOG_BIT);\
	knLuaPushEnum(script, KN_PATCHING_BIT);\
	knLuaPushEnum(script, KN_HTTP_BIT);\
	knLuaPushEnum(script, KN_SYSTEM_BIT);\
	knLuaPushEnum(script, KN_STRING_BIT);\
	knLuaPushEnum(script, KN_REGISTRY_BIT);\
	knLuaPushEnum(script, KN_DATABASE_BIT);\
	knLuaPushEnum(script, KN_PROPERTIES_BIT);\
	knLuaPushEnum(script, KN_FILE_BIT);\
	knLuaPushEnum(script, KN_GAMECTL_BIT);\
	knLuaPushEnum(script, KN_OVERLAY_BIT);\
	knLuaPushEnum(script, KN_SHADERS_BIT);\
	knLuaPushEnum(script, KN_INPUT_BIT);\
	knLuaPushEnum(script, KN_DRAW_BIT);\
	knLuaPushEnum(script, KN_PACK_BIT);\
	knLuaPushEnum(script, KN_ISC_BIT);\
	knLuaPushEnum(script, KN_UDP_BIT);\
