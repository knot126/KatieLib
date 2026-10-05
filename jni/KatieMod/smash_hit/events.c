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

bool callEventFunctionInternal(Game *this, const char *name, va_list args) {
	lua_State *script = getActiveScript(this);
	// const int initial_gettop = lua_gettop(script);
	bool result = false;
	
	if (script) {
		lua_getglobal(script, name);
		
		switch (lua_type(script, -1)) {
			// Handle one function
			case LUA_TFUNCTION: {
				if (knLuaCallIV(script, -1, 1, args)) {
					const char *err = lua_tostring(script, -1);
					LogE("Error in %s: %s", name, err);
				}
				else {
					result = lua_toboolean(script, -1);
				}
				
				lua_pop(script, 1);
				break;
			}
			// Handle table of functions
			case LUA_TTABLE: {
				lua_pushnil(script); // push first key
				
				while (lua_next(script, -2) != 0) {
					va_list xargs;
					va_copy(xargs, args);
					
					if (knLuaCallIV(script, -1, 1, xargs)) {
						const char *err = lua_tostring(script, -1);
						LogE("Error in %s: %s", name, err);
					}
					else {
						result = lua_toboolean(script, -1);
					}
					
					lua_pop(script, 1); // pop result
					va_end(xargs);
					
					if (result) {
						lua_pop(script, 1); // pop the key, since lua won't use it
						break;
					}
				}
				
				lua_pop(script, 1); // pop table
				break;
			}
			default: {
				lua_pop(script, 1); // pop nil/other value
				break;
			}
		}
	}
	
	return result;
}

bool callEventFunction(Game *this, const char *name, ...) {
	va_list args;
	va_start(args, name);
	bool res = callEventFunctionInternal(this, name, args);
	va_end(args);
	return res;
}

void (*Game_frame)(Game *this);

void Game_frame_hook(Game *this) {
	callEventFunction(this, "onFrameStart", KAT_END);
	Game_frame(this);
	callEventFunction(this, "onFrameEnd", KAT_END);
}

void (*Level_handleInput)(Level *this, QiInput *inputSource);

void Level_handleInput_hook(Level *this, QiInput *inputSource) {
	bool override = callEventFunction(gGame, "onHandleInput", KAT_END);
	
	if (!override) {
		Level_handleInput(this, inputSource);
	}
}

void (*Level_hitSomething)(Level *this, int playerIndex);

void Level_hitSomething_hook(Level *this, int playerIndex) {
	bool override = callEventFunction(gGame, "onHitSomething", KAT_INT, playerIndex, KAT_END);
	
	if (!override) {
		Level_hitSomething(this, playerIndex);
	}
}

void (*Level_streakAbort)(Level *this, int playerIndex);

void Level_streakAbort_hook(Level *this, int playerIndex) {
	bool override = callEventFunction(gGame, "onStreakAbort", KAT_INT, playerIndex, KAT_END);
	
	if (!override) {
		Level_streakAbort(this, playerIndex);
	}
}

void (*Level_streakInc)(Level *this, int playerIndex);

void Level_streakInc_hook(Level *this, int playerIndex) {
	bool override = callEventFunction(gGame, "onStreakInc", KAT_INT, playerIndex, KAT_END);
	
	if (!override) {
		Level_streakInc(this, playerIndex);
	}
}

void (*Level_enterRoom)(Level *this, Room *room);

void Level_enterRoom_hook(Level *this, Room *room) {
	callEventFunction(gGame, "onEnterRoom", KAT_STR, room->name.data ? room->name.data : room->name.cached, KAT_END);
	
	Level_enterRoom(this, room);
}

void (*Player_loadCheckpoint)(Player *this, int checkpointIndex);

void Player_loadCheckpoint_hook(Player *this, int checkpointIndex) {
	bool override = callEventFunction(gGame, "onLoadCheckpoint", KAT_INT, checkpointIndex, KAT_END);
	
	if (!override) {
		Player_loadCheckpoint(this, checkpointIndex);
	}
}

void (*Player_reportCheckpoint)(Player *this, int checkpointIndex);

void Player_reportCheckpoint_hook(Player *this, int checkpointIndex) {
	bool result = callEventFunction(gGame, "onReportCheckpoint", KAT_INT, checkpointIndex, KAT_END);
	
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
	Player_reportCheckpoint = YipHookFunction("_ZN6Player16reportCheckpointEi", Player_reportCheckpoint_hook, false);
	return NULL;
}
