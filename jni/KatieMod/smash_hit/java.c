/**
 * Invoke java commands from lua
 */

#include "../common/lua_utils.h"
#include "smashhit.h"
#include "../util.h"

#define MakeQiString(CSTR) (QiString) { \
	.data = (char *) CSTR, \
	.allocated_size = strlen(CSTR), \
	.length = strlen(CSTR), \
}

int knJavaCommand(lua_State *script) {
	KNLoadFunc(QiString, _Z11javaCommandRK8QiString, (QiString *command));
	
	const char *cmd = lua_tostring(script, 1);
	cmd = cmd ? cmd : "";
	QiString qCmd = MakeQiString(cmd);
	QiString result = _Z11javaCommandRK8QiString(&qCmd);
	lua_pushstring(script, result.data ? result.data : result.cached);
	return 1;
}

int knEnableJava(lua_State *script) {
	knRegisterFunc(script, knJavaCommand);
	
	return 0;
}
