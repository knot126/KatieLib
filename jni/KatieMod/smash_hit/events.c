#include "../util.h"
#include "../common/lua_utils.h"
#include <yiploader/yiploader.h>
#include <stdbool.h>

lua_State *getActiveScript(Game *this) {
	if (this->hudScene && this->hudScene->script.scriptInternal->state) {
		return this->hudScene->script.scriptInternal->state;
	}
	else if (this->menuScene && this->menuScene->script.scriptInternal->state) {
		return this->menuScene->script.scriptInternal->state;
	}
	else {
		return NULL;
	}
}

void callEventFunction(Game *this, const char *function_name) {
	#define CALL(ERRFUNC) if (lua_pcall(script, 0, 0, 0)) { \
		const char *err = lua_tostring(script, -1); \
		ERRFUNC; \
		lua_pop(script, 1); \
	}
	
	lua_State *script = getActiveScript(this);
	
	if (script) {
		lua_getglobal(script, function_name);
		
		switch (lua_type(script, -1)) {
			case LUA_TFUNCTION: {
				CALL(LogE("Error in %s: %s", function_name, err));
				break;
			}
			case LUA_TTABLE: {
				int n = 0;
				lua_pushnil(script); // push first key
				
				while (lua_next(script, -2) != 0) {
					n++;
					CALL(LogE("Error in %s[%d]: %s", function_name, n, err));
				}
				
				lua_pop(script, 1); // pop table
				
				break;
			}
			default: {
				lua_pop(script, 1);
			}
		}
	}
	
	#undef CALL
}

bool callEventFunctionBool(Game *this, const char *function_name) {
	lua_State *script = getActiveScript(this);
	
	if (script) {
		return knLuaCallBool(script, function_name, KAT_END);
	}
	else {
		return false;
	}
}

void (*Game_frame)(Game *this);

void Game_frame_hook(Game *this) {
	callEventFunction(this, "onFrameStart");
	Game_frame(this);
	callEventFunction(this, "onFrameEnd");
}

void (*Level_handleInput)(Level *this, QiInput *inputSource);

void Level_handleInput_hook(Level *this, QiInput *inputSource) {
	bool override = callEventFunctionBool(gGame, "onHandleInput");
	
	if (!override) {
		Level_handleInput(this, inputSource);
	}
}

void (*Level_hitSomething)(Level *this, int playerIndex);

void Level_hitSomething_hook(Level *this, int playerIndex) {
	lua_State *L = getActiveScript(gGame);
	int result = 0;
	
	if (L) {
		result = knLuaCallBool(L, "onHitSomething", KAT_INT, playerIndex, KAT_END);
	}
	
	if (!result) {
		Level_hitSomething(this, playerIndex);
	}
}

void (*Level_streakAbort)(Level *this, int playerIndex);

void Level_streakAbort_hook(Level *this, int playerIndex) {
	lua_State *L = getActiveScript(gGame);
	int result = 0;
	
	if (L) {
		result = knLuaCallBool(L, "onStreakAbort", KAT_INT, playerIndex, KAT_END);
	}
	
	if (!result) {
		Level_streakAbort(this, playerIndex);
	}
}

void (*Level_streakInc)(Level *this, int playerIndex);

void Level_streakInc_hook(Level *this, int playerIndex) {
	lua_State *L = getActiveScript(gGame);
	int result = 0;
	
	if (L) {
		result = knLuaCallBool(L, "onStreakInc", KAT_INT, playerIndex, KAT_END);
	}
	
	if (!result) {
		Level_streakInc(this, playerIndex);
	}
}

void (*Level_enterRoom)(Level *this, Room *room);

void Level_enterRoom_hook(Level *this, Room *room) {
	lua_State *L = getActiveScript(gGame);
	
	if (L) {
		knLuaCallVoid(L, "onEnterRoom", KAT_STR, room->name.data ? room->name.data : room->name.cached, KAT_END);
	}
	
	Level_enterRoom(this, room);
}

void (*Player_loadCheckpoint)(Player *this, int checkpointIndex);

void Player_loadCheckpoint_hook(Player *this, int checkpointIndex) {
	lua_State *L = getActiveScript(gGame);
	int result = 0;
	
	if (L) {
		result = knLuaCallBool(L, "onLoadCheckpoint", KAT_INT, checkpointIndex, KAT_END);
	}
	
	if (!result) {
		Player_loadCheckpoint(this, checkpointIndex);
	}
}

void (*Player_reportCheckpoint)(Player *this, int checkpointIndex);

void Player_reportCheckpoint_hook(Player *this, int checkpointIndex) {
	lua_State *L = getActiveScript(gGame);
	int result = 0;
	
	if (L) {
		result = knLuaCallBool(L, "onReportCheckpoint", KAT_INT, checkpointIndex, KAT_END);
	}
	
	if (!result) {
		Player_reportCheckpoint(this, checkpointIndex);
	}
}

const char *KNInitEvents(void) {
	Game_frame = YipHookFunction("_ZN4Game5frameEv", Game_frame_hook, false);
	Level_handleInput = YipHookFunction("_ZN5Level11handleInputERK7QiInput", Level_handleInput_hook, false);
	Level_hitSomething = YipHookFunction("_ZN5Level12hitSomethingEi", Level_hitSomething_hook, false);
	Level_streakAbort = YipHookFunction("_ZN5Level11streakAbortEi", Level_streakAbort_hook, false);
	Level_streakInc = YipHookFunction("_ZN5Level9streakIncEi", Level_streakInc_hook, false);
	Level_enterRoom = YipHookFunction("_ZN5Level9enterRoomEP4Room", Level_enterRoom_hook, false);
	Player_loadCheckpoint = YipHookFunction("_ZN6Player14loadCheckpointEi", Player_loadCheckpoint_hook, false);
	return NULL;
}
